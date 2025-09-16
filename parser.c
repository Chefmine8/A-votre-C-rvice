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
    int c;
    size_t n_c = 1;
    size_t n_ligne = 1;
    char **matrice = malloc(n_ligne * sizeof(char *));
    char **final = matrice;
    char *tmp = malloc(n_c * sizeof(char));
    *matrice = tmp;
    while ((c = fgetc(fichier)) != EOF) {
        if (c == '\n') {
            **matrice = '\0';
            matrice = realloc(matrice, ++n_ligne * sizeof(char*));
            matrice++;
            n_c = 1;
            *matrice = malloc(n_c * sizeof(char));
            continue;
        }
        *matrice = realloc(*matrice, ++n_c * sizeof(char));
        **matrice++ = (char)c;
    }
    *matrice = NULL;
    return final;
}



int main() {
    char **test = read_file("test");
    printf("%s", *test);
    return 0;
}
