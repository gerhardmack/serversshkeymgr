#include <sys/stat.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <stdbool.h>

#include "extern.h"

struct passwd *getpwent(void);
void endpwent(void);
int seteuid(gid_t egid); 

#if 0
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

static void list_keys()
{
	struct public_keys_t *tmp;

	tmp = public_keys;

	while (tmp != NULL) {
		printf("name: %s\n", tmp->name);
		tmp = tmp->next;
	}
}
#endif

static void load_user(char *user, int uid, char *dir)
{
	struct dirent *entry;
	struct stat statbuf;
	char *pbuff ;
	size_t plen ;
	DIR *d;
	
	char sshdir[] = "/.ssh" ;

	plen=strlen(dir)+strlen(sshdir)+1 ;
	
	
	pbuff=calloc(1, plen) ;
	
	snprintf(pbuff, plen, "%s%s", dir,sshdir) ;
	d = opendir(pbuff);

	if (d == NULL ) {
		debugf(2, "Unable to open %s\n", pbuff) ;
		free(pbuff) ;
		return;
	}
	if(chdir(dir) == -1) {
		debugf(2, "Unable to change directory to %s\n", pbuff) ;
		free(pbuff) ;
		return ;
	}
	
	free(pbuff) ;
	
	if (seteuid(uid) == -1) {
		debugf(0, "Unable to set uid to %i\n", uid) ;
		return ;
	}
	

	if (d == NULL) {
		return;
	}

	printf("%s %i %s\n", user, uid, dir);

	while ((entry = readdir(d)) != NULL) {
		if (entry->d_name[0] == '.') {
			continue;
		}
		stat(entry->d_name, &statbuf);

		if (S_ISDIR(statbuf.st_mode)) {
			//load_keys(entry->d_name);
		} else {
			//if (is_pubkey(entry->d_name)) {
				//load_pubkey(entry->d_name);
			//}
		}
	}

	closedir(d);

	if (seteuid(0) == -1) {
		debugf(0, "Unable to set uid to 0\n" ) ;
		return ;
	}
	
}

int distribute_keys()
{
	struct passwd *user;

	read_shells();

	for (user = getpwent(); user; user = getpwent()) {
		if (valid_shell(user->pw_shell)) {
			debugf(2,"User: \"%s\" has a valid shell\n", user->pw_name) ;
			load_user(user->pw_name, user->pw_uid, user->pw_dir) ;
		} else {
			debugf(3,"User: \"%s\" has an invalid shell\n", user->pw_name) ;
		}
	}

	endpwent();
	return (0);
}
