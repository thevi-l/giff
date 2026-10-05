#include <stdlib.h>
#include <string.h>
#include "strbuf.h"

// Définition unique de slopbuf
char strbuf_slopbuf[1];

void strbuf_init(strbuf *sb, size_t hint) {
	strbuf blank = STRBUF_INIT;
	memcpy(sb, &blank, sizeof(*sb));
	if(hint) strbuf_grow(sb, hint);
}

void strbuf_release(strbuf *sb) {
    if (sb->buf != strbuf_slopbuf)
        free(sb->buf);
    sb->buf = strbuf_slopbuf;
    sb->alloc = sb->len = 0;
}

void strbuf_reset(strbuf *sb) {
    strbuf_setlen(sb, 0);
}

void strbuf_grow(strbuf *sb, size_t extra) {
    if (sb->len + extra + 1 <= sb->alloc)
        return;

    if (sb->buf == strbuf_slopbuf)
        sb->buf = NULL;

    size_t new_alloc = (sb->alloc + extra + 1) * 2;
    if (new_alloc < 64)
        new_alloc = 64;

    char *tmp = realloc(sb->buf, new_alloc);
    if (!tmp)
        return;

    sb->buf = tmp;
    sb->alloc = new_alloc;
    if (sb->len == 0)
        sb->buf[0] = '\0';
}

void strbuf_addchars(strbuf *sb, int c, size_t n) {
    strbuf_grow(sb, n);
    memset(sb->buf + sb->len, c, n);
    strbuf_setlen(sb, sb->len + n);
}

void strbuf_addstr(strbuf *sb, const char *s) {
    if (!s) return;
    size_t len = strlen(s);
    strbuf_grow(sb, len);
    memcpy(sb->buf + sb->len, s, len);
    strbuf_setlen(sb, sb->len + len);
}

int strbuf_getline(strbuf *sb, FILE *fp) {
    int c = fgetc(fp);
    if (c == EOF) return EOF;
    strbuf_reset(sb);
    while (c != EOF && c != '\n') {
        if (c != '\r') // Gestion CRLF Windows
            strbuf_addch(sb, c);
        c = fgetc(fp);
    }
    return 0;
}

void strbuf_detab(strbuf *sb, int tab_width) {
    if (!sb || tab_width <= 0) return;

    strbuf out = STRBUF_INIT;
    for (size_t i = 0; i < sb->len; i++) {
        if (sb->buf[i] == '\t') {
            size_t pad = tab_width - (out.len % tab_width);
            strbuf_addchars(&out, ' ', pad);
        } else {
            strbuf_addch(&out, sb->buf[i]);
        }
    }
    strbuf_swap(sb, &out);
    strbuf_release(&out);
}

bool starts_with(const char *str, const char *prefix) {
    for (; *prefix; str++, prefix++) {
        if (*str != *prefix)
            return false;
    }
    return true;
}
