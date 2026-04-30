#include <stdio.h>
#include <stdlib.h>

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

    int c;
    while((c = fgetc(in)) != EOF)
        fputc(c, stdout);

    if(in != stdin)
        fclose(in);

    return 0;
}
