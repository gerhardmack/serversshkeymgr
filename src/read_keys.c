#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#include "extern.h"

#define MAXKEYSIZE 20000

struct public_keys_t {
	char *name;
	char *key;
	struct public_keys_t *next;
};

static struct public_keys_t *public_keys = NULL;

static void load_pubkey(const char *fname)
{
	FILE *pkeyfile;
	char line[MAXKEYSIZE];
	struct public_keys_t *newk, *tmp;
	size_t nsize;

	if ((pkeyfile = fopen(fname, "r")) == NULL) {
		errorf("Cannot open %s/%s\n", getcwd(NULL, 0), fname);
		return;
	}

	nsize = strlen(fname) - 3;

	newk = calloc(1, sizeof(struct public_keys_t));
	newk->name = calloc(1, nsize);
	strncpy(newk->name, fname, nsize - 1);
	newk->name[nsize] = '\0';

	while ((fgets(line, MAXKEYSIZE, pkeyfile)) != NULL) {
		if (line[0] == '#' || line[0] == ' ') {
			continue;
		}
		if (line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = '\0';
		}

		nsize = strlen(line);
		newk->key = calloc(1, nsize);
		strncpy(newk->key, line, nsize);
		debugf(1, "Loaded key for user: %s\n", newk->name) ;
		debugf(2, "key: %s\n\n", newk->key) ;
		break;
	}

	(void) fclose(pkeyfile);

	if (newk->key == NULL) {
		errorf("Error reading %s/%s\n", getcwd(NULL, 0), fname);
		free(newk->name);
		free(newk);
		return;
	}

	if (public_keys == NULL) {
		public_keys = newk;
		return;
	}

	tmp = public_keys;

	while (tmp->next != NULL) {
		tmp = tmp->next;
	}

	tmp->next = newk;
}

static bool is_pubkey(const char *fname)
{
	int len;
	len = strlen(fname);

	if (strncmp(&fname[len - 4], ".pub", sizeof(5)) == 0) {
		return (true);
	}

	return (false);
}

char *get_key(const char *name) 
{
	struct public_keys_t *tmp;

	tmp = public_keys; 
	        
	while (tmp != NULL) {
		if(strncmp(name, tmp->name, strlen(name)) == 0) {
			return(tmp->key) ;
		}
		tmp = tmp->next;
	}
	
	return(NULL) ;
}

void load_keys(const char *directory)
{
	struct dirent *entry;
	struct stat statbuf;
	DIR *d;

	d = opendir(directory);
	chdir(directory);
	opendir(".");

	if (d == NULL) {
		return;
	}

	while ((entry = readdir(d)) != NULL) {
		if (entry->d_name[0] == '.') {
			continue;
		}
		stat(entry->d_name, &statbuf);

		if (S_ISDIR(statbuf.st_mode)) {
			debugf(4, "found a directory %s\n", entry->d_name) ;
			load_keys(entry->d_name);
		} else {
			if (is_pubkey(entry->d_name)) {
				debugf(4, "%s is a key\n", entry->d_name);
				load_pubkey(entry->d_name);
			} else {
				debugf(4, "%s is not a key\n", entry->d_name);
				}
		}
	}

	closedir(d);
	chdir("..");
}
