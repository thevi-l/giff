#include <stddef.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define DEFAULT "\x1b[0m"
#define RED_BG "\x1b[48;2;95;0;0m"
#define RED_HL "\x1b[48;2;178;38;0m"
#define GREEN_HL "\x1b[48;2;25;148;0m"
#define GREEN_BG "\x1b[48;2;0;95;0m"
#define GREEN_HUNK_BG "\x1b[48;2;0;75;0m"
#define BLUE_FRONT(string) "\x1b[0;34m" string DEFAULT
#define GRAY_BG "\x1b[48;5;253m"
#define GRAY_FRONT "\x1b[38;2;108;108;108m"
#define WHITE_FRONT "\x1b[38;2;250;250;250m"

int parseHunkHeader(char* string, int *hunk_h) {
	int res;
	res =	sscanf(string,
			"@@ -%d,%d +%d,%d",
			hunk_h, (hunk_h+1), (hunk_h+2), (hunk_h+3));
	return res == 4;
}

int itoa(int i, char **string_buffer){
	if (string_buffer == NULL)
		return 0;
	int i_length = snprintf(NULL, 0, "%d", i);
	int s_lentgh = i_length + 1;
	*string_buffer = malloc(s_lentgh);
	if (*string_buffer == NULL)
		return 0;
	sprintf(*string_buffer, "%d", i);
	return 1;
}

void printLineCount(int *hunk_header, char *bg){
	
}

// TODO (tayheau): count number of ins/del per file
void handleFileHeader(char *string){
	char *fileName;
	if (*string == '-'){
		sscanf(string, "--- a/%s", fileName);
		printf("%s\n", fileName);
	}
}

void print_block(char *string, int *hunk_header){
	if(*string == '-') {
		fputs(RED_BG, stdout);
		fputs("\x1b[K", stdout);
		fputs(GRAY_FRONT, stdout);
		printf("%3d %3c│", hunk_header[0]++, ' ');
		fputs(WHITE_FRONT, stdout);
		printf("%s%s\n", string, DEFAULT);
	} else {
		fputs(GREEN_BG, stdout);
		fputs("\x1b[K", stdout);
		fputs(GRAY_FRONT, stdout);
		printf("%3c %3d│", ' ', hunk_header[2]++);
		fputs(WHITE_FRONT, stdout);
		printf("%s%s\n", string, DEFAULT);
	}
}

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

void handle_print(char* str, char first){
	if (*str == '-') {
		
	}

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
		size_t n_read = 0;
		int occur;
		char first;
// TODO (tayheau): go to a len 2 hunk header 
// TODO (tayheau): rotate to a stack logic
		int hunk_header[4] = {-1};
		while((n_read = getline(&line, &len, in)) != -1){
			first = n_read > 0 ? line[0] : '\0';
			if ((first == '+') || (first == '-')){
				*strchr(line, '\n') = '\0';
				print_block(line, hunk_header);
			} else if (first == '@' && findNthOccur(line, &occur, '@', 4)) {
				fputs("\x1b[0;34m", stdout);
				parseHunkHeader(line, hunk_header);
				fwrite(&line[occur+2], 1, strlen(line) - occur - 2, stdout);
				fputs("\x1b[0m", stdout);
			} else {
				if (hunk_header[0] != -1) {
					fputs(GRAY_FRONT, stdout);
					printf("%3d %3d│", hunk_header[0]++, hunk_header[2]++);
					fputs(WHITE_FRONT, stdout);
					printf("%s%s", line, DEFAULT);
				} else {
					// TODO (tayheau): better handle file header than this
					printf("      %s", line);
				}
			}
		}
		// fputs(DEFAULT, stdout);
		free(line);
		fclose(in);
    return 0;
}
