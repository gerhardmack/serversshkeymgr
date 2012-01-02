#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "extern.h"

char *strdup(const char *s);

struct valid_shells_t {
	char *shell ;
	struct valid_shells_t *next ;
} ;

static struct valid_shells_t *valid_shells = NULL ;

static void store_shell(char *shell) 
{
	struct valid_shells_t *newe, *temp ;
	
	newe = calloc (1, sizeof (struct valid_shells_t)) ;
	newe->shell = calloc(1,strlen(shell)) ;
	strncpy(newe->shell, shell, strlen(shell)) ;
	debugf(3, "Allowed Shell: %s\n", newe->shell) ;
	if (valid_shells == NULL) {
		valid_shells = newe ;
		return ;
	} 
	
	temp = valid_shells ;

	while(temp->next != NULL) {
		temp = temp->next ;
	}

	temp->next = newe ;	
}

bool valid_shell(char *shell) 
{
	struct valid_shells_t *temp ;
		
	temp = valid_shells ;

	while(temp != NULL) {
		if(strncmp(temp->shell, shell, sizeof(temp->shell)) == 0) {
			return(true) ;
		}
		temp = temp->next ;
	}
	
	return(false) ;
}

int read_shells()
{
	FILE *shells;
	char line[1000];

	if ((shells = fopen("/etc/shells", "r")) == NULL) {
		errorf("Cannot open /etc/shells!\n");
		return (-1);
	}

	while ((fgets(line, 1000, shells)) != NULL) {
		if (line[0] == '#') {
			continue;
		}
		if (line[strlen(line) - 1] == '\n') {
			line[strlen(line) - 1] = '\0';
		}
		
		store_shell(line) ;
	}

	(void) fclose(shells);

	return(0);
}
