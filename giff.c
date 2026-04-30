#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT "\x1b[0m"
#define RED_BG "\x1b[48;2;59;11;0m"
#define RED_HL "\x1b[48;2;178;38;0m"
#define GREEN_HL "\x1b[48;2;25;148;0m"
#define GREEN_BG(string) "\x1b[48;2;10;59;0m" string "\x1b[0m"
#define BLUE_FRONT(string) "\x1b[0;34m" string DEFAULT
#define GRAY_BG "\x1b[48;5;253m"

int findNthOccur(char* string, int* res, char c, int n_occur) {
	int occur = 0;
	for (int i = 0; i < strlen(string); i++) {
		if (string[i] == c) {
			occur++;
			if (occur == n_occur) {
				*res = i;
				return 1;
			}
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
    FILE *in = stdin;

    if(argc == 2)
    {
        in = fopen(argv[1], "r");
        if(in == NULL)
        {
            perror("fopen");
            return 1;
        }
    }

		char* line = NULL;
		size_t len = 0;
		ssize_t n_read = 0;
		int occur;
		char first;
		while((n_read = getline(&line, &len, in)) != -1){
			first = n_read > 0 ? line[0] : '\0';
			if ((first == '+') || (first == '-')){
				// fwrite(line, n_read, 1, stdout);
				printf(GREEN_BG("%s"), line);
			} else if (first == '@' && findNthOccur(line, &occur, '@', 4)) {
				fputs("\x1b[0;34m", stdout);
				fwrite(&line[occur+2], 1, strlen(line) - occur - 2, stdout);
				fputs("\x1b[0m", stdout);
				// printf("%s%s%s", GRAY_BG,line,DEFAULT);
			}
		}
		fputs(DEFAULT, stdout);
		free(line);
		fclose(in);
		// int c, p;
		// while((c = fgetc(in)) != EOF){
		// 	if (c == '+'&& p == '\n'){
		//
		// 	}
		// }
    // int c;
    // while((c = fgetc(in)) != EOF)
    //     fputc(c, stdout);
    //
    // if(in != stdin)
    //     fclose(in);

    return 0;
}
