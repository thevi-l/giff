#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "strbuf.h"

#define DEFAULT     "\x1b[0m"
#define RED_BG      "\x1b[48;2;95;0;0m"
#define RED_HL      "\x1b[48;2;178;38;0m"
#define GREEN_HL    "\x1b[48;2;25;148;0m"
#define GREEN_BG    "\x1b[48;2;0;95;0m"
#define GRAY_FRONT  "\x1b[38;2;108;108;108m"
#define BLUE_FRONT  "\x1b[0;34m"
#define WHITE_FRONT "\x1b[38;2;250;250;250m"

typedef uint8_t u8;
typedef u8 b8;

#define IS_DIFF_LINE(line, ch) \
    ((line)[0] == (ch) && !((line)[1] == (ch) && (line)[2] == (ch)))

b8 parseHunkHeader(const char* string, int *lc_old, int *lc_new) {
    int res = sscanf(string, "@@ -%d,%*d +%d,%*d", lc_old, lc_new);
    return res == 2;
}

int main(void) {
    int lc_old = 1, lc_new = 1;
    strbuf linebuf = STRBUF_INIT;
    while (strbuf_getline(&linebuf, stdin) != EOF) {
        strbuf_detab(&linebuf, 4);
        if (starts_with(linebuf.buf, "@@ -")) {
            // TODO: flush_stacks(&rm, &add, &lc_old, &lc_new);
            parseHunkHeader(linebuf.buf, &lc_old, &lc_new);
            printf(BLUE_FRONT "%s\n" DEFAULT, linebuf.buf);
            continue;
        }
        if (IS_DIFF_LINE(linebuf.buf, '+')) {
            // TODO: Ajouter à la strlist 'add' au lieu d'imprimer directement
            printf(GREEN_BG GRAY_FRONT "%3c %3d|" WHITE_FRONT " %s\n" DEFAULT,
								' ', lc_new++, linebuf.buf + 1);
        }
        else if (IS_DIFF_LINE(linebuf.buf, '-')) {
            // TODO: Ajouter à la strlist 'rm' au lieu d'imprimer directement
            printf(RED_BG GRAY_FRONT "%3d %3c|" WHITE_FRONT " %s\n" DEFAULT,
								lc_old++, ' ', linebuf.buf + 1);
        } 
        else {
            // TODO: flush_stacks(&rm, &add, &lc_old, &lc_new);
            if (linebuf.buf[0] == ' ') {
                printf(GRAY_FRONT "%3d %3d|" DEFAULT " %s\n",
										lc_old++, lc_new++, linebuf.buf + 1);
            } else {
                printf(BLUE_FRONT "%s\n" DEFAULT, linebuf.buf);
            }
        }
    }

    // TODO: flush_stacks final
    strbuf_release(&linebuf);
    return 0;
}
