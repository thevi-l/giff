#include <asm-generic/ioctls.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "types.h"
#include "strbuf.h"
#include "strlist.h"

#define DEFAULT     "\x1b[0m"
#define RED_BG      "\x1b[48;2;95;0;0m"
#define RED_HL      "\x1b[48;2;178;38;0m"
#define GREEN_HL    "\x1b[48;2;25;148;0m"
#define BLUE_BG 		"\x1b[44m"
#define GREEN_BG    "\x1b[48;2;0;60;0m"
#define GRAY_FRONT  "\x1b[38;2;108;108;108m"
#define BLUE_FRONT  "\x1b[0;34m"
#define WHITE_FRONT "\x1b[38;2;250;250;250m"

#define IS_DIFF_LINE(line, ch) \
    ((line)[0] == (ch) && !((line)[1] == (ch) && (line)[2] == (ch)))

int getTermWidth() {
	// return 80;
	static int width=0;
	if(!width){
		int fd=open("/dev/tty", O_RDONLY);
		struct winsize w; 
		if (ioctl(fd, TIOCGWINSZ, &w)==0 && w.ws_col >0) width=w.ws_col;
		close(fd);
	}
	return width;
}

b8 parseHunkHeader(const char *string, int *lc_old, int *lc_new) {
    int res = sscanf(string, "@@ -%d,%*d +%d,%*d", lc_old, lc_new);
    return res == 2;
}

// const char *skipPrefix(const char *string, const char *prefix) {
// 	while(*prefix) if(*string++ != *prefix++) return NULL;
// 	return string;
// }

void printHunkHeader(const strbuf *sb, FILE *output){
	strbuf out; strbuf_init(&out,0);
	strbuf_addstr(&out, GRAY_FRONT); strbuf_addstr(&out, "┌──");
	strbuf_addstr(&out, DEFAULT); strbuf_addstr(&out, sb->buf);
	strbuf_addstr(&out, GRAY_FRONT);
	for (int i = sb->len + 3; i < getTermWidth() - 1; i++) strbuf_addstr(&out, "─");
	strbuf_addstr(&out, "┐");strbuf_addstr(&out, DEFAULT);
	fprintf(output, "%s\n", out.buf);
	strbuf_release(&out);
}

void printEndHunk(FILE *output){
	strbuf out; strbuf_init(&out,0);
	strbuf_addstr(&out, GRAY_FRONT);
	strbuf_addstr(&out, "└");
	for (int i = 1; i < getTermWidth() - 1; i++) strbuf_addstr(&out, "─");
	strbuf_addstr(&out, "┘");strbuf_addstr(&out, DEFAULT);
	fprintf(output, "%s\n\n", out.buf);
	strbuf_release(&out);
}

// TODO
void flushHunk(strlist *add, strlist *rm, int *lc_old, int *lc_new, FILE *output)
{
		int width = getTermWidth();
    for (size_t i = 0; i < rm->len; i++) {
        fprintf(output, GRAY_FRONT"│"RED_BG"%3d %3c| "WHITE_FRONT"%-*s"DEFAULT GRAY_FRONT"│"DEFAULT"\n",
            (*lc_old)++, ' ', width-11, rm->buf[i] + 1);
    }
    for (size_t i = 0; i < add->len; i++) {
        fprintf(output, GRAY_FRONT"│"GREEN_BG"%3c %3d| "WHITE_FRONT"%-*s"DEFAULT GRAY_FRONT"│"DEFAULT"\n",
            ' ', (*lc_new)++, width-11, add->buf[i] + 1);
    }
    strlist_clear(add); strlist_clear(rm);
}
int main(void) {
		FILE *out = isatty(fileno(stdout)) ? popen("less -+X -+F", "w") : stdout;
		if(!out) out = stdout;
    int lc_old = -1, lc_new = 1;
    strbuf linebuf; strbuf_init(&linebuf, 0);
		strlist add, rm; strlist_init(&add); strlist_init(&rm);
    while (strbuf_getline(&linebuf, stdin) != EOF) {
        strbuf_detab(&linebuf, 4);
				strbuf_trim_trailing_newline(&linebuf);
				strbuf_strip_ansi(&linebuf);
        if (IS_DIFF_LINE(linebuf.buf, '+')) {
					strlist_append(&add, linebuf.buf);
        }
        else if (IS_DIFF_LINE(linebuf.buf, '-')) {
					if(add.len > 0) flushHunk(&add, &rm, &lc_old, &lc_new, out);
					strlist_append(&rm, linebuf.buf);
        } 
        else {
					if (add.len > 0 || rm.len >0)flushHunk(&add, &rm, &lc_old, &lc_new, out);
					if (starts_with(linebuf.buf, "diff")){
						if(lc_old!=-1){
							printEndHunk(out); lc_old = -1;
						}
						fprintf(out,BLUE_BG"%-*s"DEFAULT,getTermWidth(), linebuf.buf);
					}
					if (starts_with(linebuf.buf, "@@ -")) {
							if(lc_old!=-1){
								printEndHunk(out); lc_old = -1;
							}
							parseHunkHeader(linebuf.buf, &lc_old, &lc_new);
							printHunkHeader(&linebuf, out);
							continue;
					}
					else if (linebuf.buf[0] == ' ') {
							fprintf(out, GRAY_FRONT "│%3d %3d|"DEFAULT " %-*s" GRAY_FRONT"│"DEFAULT"\n",
									lc_old++, lc_new++, (getTermWidth()-11), linebuf.buf + 1);
					} 
        }
	}
		if (add.len > 0 || rm.len >0)flushHunk(&add, &rm, &lc_old, &lc_new, out);
		if(lc_old!=-1)printEndHunk(out);
		if(out!=stdout)pclose(out);
    strbuf_release(&linebuf);
		strlist_release(&add); strlist_release(&rm);
    return 0;
}
