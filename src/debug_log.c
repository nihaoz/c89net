#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "debug_log.h"

static int _account_symbol(char *s, char ch)
{
	int c = 0;
	while(*s)
		if(*s++ == ch)
			c++;
	return c;
}

static char *_find_symbol(char *s, char ch)
{
	if(*s == ch)
		return s;
	while(*s)
		if(*++s == ch)
 			return s;
	return NULL;
}

static void _print_time(FILE *fp, const char *fmt)
{
	clock_t clk = clock();
	fprintf(fp, fmt, clk);
}

void time_tag_warning()
{
	_print_time(FORMATLOG_FP, "[\033[;33m%08ld\033[0m]: ");
}

void time_tag_error()
{
	_print_time(FORMATLOG_FP, "[\033[;31m%08ld\033[0m]: ");
}

void format_log(const char *fmt, ...)
{
	char *p, *f, *pf;
	int n;
	/* time_t t;
	 * time(&t);
	 */
	clock_t clk = clock();
	va_list ap;
	f = pf = (char*)malloc(strlen(fmt) + 1);
	strcpy(f, fmt);
	/* fprintf(FORMATLOG_FP, "[%ld:%ld] : ", t, clk); */
	fprintf(FORMATLOG_FP, "[\033[0;36m%08ld\033[0m]: ", clk);
	va_start(ap, fmt);
	n = _account_symbol(f, '%');
	p = _find_symbol(f, '%');
	if(p == NULL){
		fprintf(FORMATLOG_FP, "%s\n", f);
		free(pf);
		return;
	}
	*p = '\0';
	fprintf(FORMATLOG_FP, "%s", f);
	f = p;
	*f = '%';
	while(n-- > 1){
		p = _find_symbol(f + 1, '%');
		if(p == NULL){
			free(pf);
			return;
		}
		*p = '\0';
		fprintf(FORMATLOG_FP, f, va_arg(ap, void*));
		f = p;
		*f = '%';
	}
	fprintf(FORMATLOG_FP, f, va_arg(ap, void*));
	fprintf(FORMATLOG_FP, "\n");
	free(pf);
	va_end(ap);
}
