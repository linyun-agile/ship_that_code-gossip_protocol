#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

/* TODO (why-gossip): implement per the lesson description. */

int main(void) {
    char line[1024];
    while (fgets(line, sizeof line, stdin)) {
        if (line[0] == '\n' || line[0] == 0) continue;
        if (strncmp(line, "ESTIMATE", 8) != 0) {
            printf("invalid input.\n");
            continue;
        }
        char *number_text = line + 8;
        while (isspace((unsigned char)*number_text)) {
            number_text++;
        }
        int num = (int)strtol(number_text, NULL, 10);
        int res = (int)(log2(num) + 0.5);
        printf("rounds=%d for_n=%d\n", res, num);
    }
    return 0;
}
