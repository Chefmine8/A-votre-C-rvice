#include "global.h"
#include "parser.h"


void upper(char *grid) {
    while (*grid != '\0') {
        *grid = *grid >= 'a' && *grid <= 'z' ? *grid - 'a' + 'A' : *grid;
        grid++;
    }
}

char **read_file(char *file) {
    FILE *fichier = fopen(file, "r");
    size_t n_ligne = 1;
    size_t n_c = 1;
    size_t i_ligne = 0;
    size_t i_c = 0;
    char **matrice = malloc(n_ligne);
    int c;

    while ((c = fgetc(fichier)) != EOF) {
        if (c == '\n') {
            if (++i_ligne > n_ligne) matrice = realloc(matrice, ++n_ligne * sizeof(char*));
            matrice++;
            n_c = 1;
            *matrice = malloc(n_c);
            continue;
        }
        if (++i_c > n_c) *matrice = realloc(*matrice, ++n_c * sizeof(char));
    }
}



int main() {
    char grid[] = "awerz";
    upper(grid);
    printf("%s", grid);
    return 0;
}
