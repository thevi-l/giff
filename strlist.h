#pragma once
#include <stddef.h>
#include "types.h"

typedef struct {size_t alloc; size_t len; char **buf;} strlist;
#define STRLIST_INIT { .buf=NULL }

void strlist_init(strlist *sl);
void strlist_release(strlist *sl);
void strlist_clear(strlist *sl);
b8 strlist_alloc(strlist *sl, size_t len);
b8 strlist_append(strlist *sl, const char *str);
