#pragma once

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include "types.h"

typedef struct { size_t alloc; size_t len; char *buf; } strbuf;

extern char strbuf_slopbuf[];
#define STRBUF_INIT { .buf = strbuf_slopbuf }

void strbuf_init(strbuf *sb, size_t hint);
void strbuf_release(strbuf *sb);
void strbuf_reset(strbuf *sb);
void strbuf_grow(strbuf *sb, size_t extra);

void strbuf_insert(strbuf *sb, size_t pos, const void *data, size_t len);
void strbuf_splice(strbuf *sb, size_t pos, size_t len, const void *data, size_t dlen);

static inline void strbuf_swap(strbuf *a, strbuf *b) {
    strbuf tmp = *a;
    *a = *b;
    *b = tmp;
}

static inline size_t strbuf_avail(const strbuf *sb) {
    return sb->alloc ? sb->alloc - sb->len - 1 : 0;
}

void strbuf_trim_trailing_newline(strbuf *sb);

static inline void strbuf_setlen(strbuf *sb, size_t len) {
    sb->len = len;
    if (sb->buf != strbuf_slopbuf)
        sb->buf[len] = '\0';
}

static inline void strbuf_addch(strbuf *sb, int c) {
    if (!strbuf_avail(sb))
        strbuf_grow(sb, 1);
    sb->buf[sb->len++] = c;
    sb->buf[sb->len] = '\0';
}

void strbuf_addchars(strbuf *sb, int c, size_t n);
void strbuf_addstr(strbuf *sb, const char *s);

int strbuf_getline(strbuf *sb, FILE *fp);
void strbuf_detab(strbuf *sb, int tab_width);

bool starts_with(const char *str, const char *prefix);

void strbuf_strip_ansi(strbuf *sb);
