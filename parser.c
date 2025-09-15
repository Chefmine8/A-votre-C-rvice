#include "global.h"
#include "parser.h"


void upper(char *grid) {
    while (*grid != '\0') {
        *grid = *grid >= 'a' && *grid <= 'z' ? *grid - 'a' + 'A' : *grid;
        grid++;
    }
}

void main() {
    char *grid = "awerz";
    printf("%s", upper(grid));
}
