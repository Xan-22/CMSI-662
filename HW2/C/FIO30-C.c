#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    if (argc < 2 || argc > 12) {
        puts("Usage: Enter a number of words (up to 10), followed by the words to be printed");
        return 1;
    }
    size_t num_words = atoi(argv[1]);
    if (num_words <= 0 || num_words > argc - 2) {
        puts("Invalid number of words");
        return 1;
    }
    printf("Entered %ld words: ", num_words);
    for (int i = 0; i < num_words; i++) {
        if (i > 0) {
            fputs(" ", stdout);
        }
        fputs(argv[i + 2], stdout);
    }
    fputs("\n", stdout);
}