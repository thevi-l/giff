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

void strbuf_splice(strbuf *sb, size_t pos, size_t len, const void *data, size_t dlen) {
	// if (unsigned_add_overflows(pos, len))
	// 	die("you want to use way too much memory");
	// if (pos > sb->len)
	// 	die("`pos' is too far after the end of the buffer");
	// if (pos + len > sb->len)
	// 	die("`pos + len' is too far after the end of the buffer");
	if (dlen >= len) strbuf_grow(sb, dlen - len);
	memmove(sb->buf + pos + dlen, sb->buf + pos + len, sb->len - pos - len);
	memcpy(sb->buf + pos, data, dlen);
	sb->len = sb->len + dlen - len;
}

void strbuf_insert(strbuf *sb, size_t pos, const void *data, size_t len) {
	strbuf_splice(sb, pos, 0, data, len);
}

void strbuf_trim_trailing_newline(strbuf *sb) {
	if(sb->len>0 && sb->buf[sb->len-1] == '\n'){
		if(--sb->len>0 && sb->buf[sb->len-1] == '\r') --sb->len;
		sb->buf[sb->len]='\0';
	}
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
