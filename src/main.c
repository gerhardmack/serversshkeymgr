#include <getopt.h>
#include <stdio.h>
#include <stdbool.h>

#include "extern.h"

struct passwd *getpwent(void);
void endpwent(void);

int main(int argc, char **argv) {

	int o;
	opterr = 0;
	
	while ((o = getopt(argc, argv, "v")) != EOF) {
		switch (o) {
		case 'v':{
			set_debug() ;
			break;
		}
		default:{
			}
		}
	}
	load_keys("/etc/keymgr/keys") ;
	// printf("%s\n",get_key("duarte_laptop") ) ;
	distribute_keys() ;
	return(0) ;

}
