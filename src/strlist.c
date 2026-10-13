#include "strlist.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

void strlist_init(strlist *sl) {
	strlist blank = STRLIST_INIT;
	memcpy(sl, &blank, sizeof(*sl));
}

void strlist_clear(strlist *sl) {
	for (size_t i = 0; i<sl->len; i++) free(sl->buf[i]);
	sl->len = 0;
}

void strlist_release(strlist *sl) {
	strlist_clear(sl);
	free(sl->buf); sl->buf = NULL;
	sl->alloc=0;
}

b8 strlist_alloc(strlist *sl, size_t len) {
	if(sl->alloc>=len) return 1;
	size_t new_alloc = sl->alloc ? sl->alloc * 2 : 8;
	while(new_alloc < len) new_alloc *= 2;
	char **buf = realloc(sl->buf, new_alloc * sizeof(*buf));
	if(!buf) return 0;
	sl->buf = buf; sl->alloc = new_alloc;
	return 1;
}

b8 strlist_append(strlist *sl, const char *str) {
	if(!strlist_alloc(sl, sl->len+1)) return 0;
	sl->buf[sl->len] = malloc(strlen(str)+1);
	strcpy(sl->buf[sl->len++] , str);
	return 1;
}
