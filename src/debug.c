#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

#include "extern.h"

static int debug_level = 0;

void set_debug()
{
	debug_level++;
}

void debugf(int level, const char *format, ...)
{
	va_list args;
	
	if(level > debug_level) {
		return ;
	}

	va_start(args, format);

	if (vfprintf(stdout, format, args) <= 0) {
		return;
	}

	va_end(args);
}

void errorf(const char *format, ...)
{
	va_list args;
	
	va_start(args, format);

	if (vfprintf(stderr, format, args) <= 0) {
		return;
	}

	va_end(args);
}
