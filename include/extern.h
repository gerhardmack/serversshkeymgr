
extern bool valid_shell(char *) ;
extern int read_shells(void) ;
extern int distribute_keys(void) ;
extern void load_keys(const char *) ;
extern char *get_key(const char *name) ;
extern void set_debug(void) ;
extern void debugf(int level, const char *format, ...) __attribute__ ((format(printf, 2, 3)))  ;
extern void errorf(const char *format, ...) __attribute__ ((format(printf, 1, 2)))  ;
