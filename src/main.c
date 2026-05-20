#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tally.h"

int main(int argc, char *argv[]) {
    const char *Usage =
        "Usage: tally <filename> = count lines\nUsage: tally -w "
        "<filename> = count words\nUsage: tally -c <filename> "
        "= count characters\n";

    if (argc < 2) {
        FILE *fp = stdin;
        long lines = tally_count_lines(fp);
        printf("%ld", lines);
        return EXIT_SUCCESS;
    }
    int start;
    long total = 0;
    int error = 0;
    long max = 0;

    if (argv[1][0] == '-' && argv[1][2] == '\0') {
        start = 2;
    } else {
        start = 1;
    }

    if (start == 2 && argc == 2) {
        if (strcmp(argv[1], "-w") == 0) {
            FILE *fp = stdin;
            long words = tally_count_words(fp);
            printf("%ld", words);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[1], "-c") == 0) {
            FILE *fp = stdin;
            long characters = tally_count_characters(fp);
            printf("%ld", characters);
            return EXIT_SUCCESS;
        } else {
            fprintf(stderr, "%s", Usage);
            return EXIT_FAILURE;
        }
    }

    for (int i = start; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (!fp) {
            error = 1;
            continue;
        }
        if (strcmp(argv[1], "-w") == 0) {
            long words = tally_count_words(fp);
            fclose(fp);

            if (words > max) {
                max = words;
            }
            continue;
        } else if (strcmp(argv[1], "-c") == 0) {
            long characters = tally_count_characters(fp);
            fclose(fp);

            if (characters > max) {
                max = characters;
            }
            continue;

        } else {
            long lines = tally_count_lines(fp);
            fclose(fp);

            if (lines > max) {
                max = lines;
            }
            continue;
        }
    }
    int width = (max > 0) ? (int)log10((double)max) + 1 : 1;

    for (int i = start; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (!fp) {
            perror(argv[i]);
            error = 1;
            continue;
        }
        if (strcmp(argv[1], "-w") == 0) {
            long words = tally_count_words(fp);
            fclose(fp);

            if (words < 0) {
                fprintf(stderr, "tally: internal error while counting\n");
                continue;
            }
            total += words;

            printf("%*ld\t%s\n", width, words, argv[i]);
            continue;
        } else if (strcmp(argv[1], "-c") == 0) {
            long characters = tally_count_characters(fp);
            fclose(fp);

            if (characters < 0) {
                fprintf(stderr, "tally: internal error while counting\n");
                continue;
            }
            total += characters;

            printf("%*ld\t%s\n", width, characters, argv[i]);
            continue;

        } else {
            long lines = tally_count_lines(fp);
            fclose(fp);

            if (lines < 0) {
                fprintf(stderr, "tally: internal error while counting\n");
                continue;
            }
            total += lines;

            printf("%*ld\t%s\n", width, lines, argv[i]);
            continue;
        }
    }

    printf("\n%*ld\ttotal\n", width, total);

    if (error) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
