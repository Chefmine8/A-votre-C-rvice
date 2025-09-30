#include "global.h"
#include "parser.h"


void upper(char *grid)
{
    while (*grid != '\0')
    {
        *grid = *grid >= 'a' && *grid <= 'z' ? *grid - 'a' + 'A' : *grid;
        ++grid;
    }
}

char **read_file(char *file)
{
    FILE *fichier = fopen(file, "r");
    if (!fichier)
    {
        return NULL;
    }

    int c;
    size_t n_line = 0;
    char **matrice = malloc(sizeof(char *));

    size_t n_c = 0;
    size_t capacity_c = 12;
    char *current_line = malloc(capacity_c * sizeof(char));

    while ((c = fgetc(fichier)) != EOF)
    {
        if (c == '\n')
        {
            current_line[n_c] = '\0';
            matrice = realloc(matrice, (n_line + 2) * sizeof(char *));

            capacity_c = n_c;
            n_c = 0;

            matrice[n_line] = current_line;
            ++n_line;
            current_line = malloc(capacity_c * sizeof(char));
            continue;
        }

        if (n_c >= capacity_c)
        {
            ++capacity_c;
            current_line = realloc(current_line, capacity_c * sizeof(char));
        }

        current_line[n_c] = (char)c;
        ++n_c;
    }

    if (n_c > 0)
    {
        current_line[n_c] = '\0';
        matrice = realloc(matrice, (n_line + 2) * sizeof(char *));
        matrice[n_line] = current_line;
        ++n_line;
    }
    else
    {
        free(current_line);
    }

    matrice[n_line] = NULL;

    fclose(fichier);
    return matrice;
}

void free_matrix(char **matrice)
{
    if (!matrice)
    {
        return;
    }

    for (size_t i = 0; matrice[i] != NULL; ++i)
    {
        free(matrice[i]);
    }
    free(matrice);
}

int main()
{
    char **test = read_file("test");
    char **tmp = test;
    while (*test)
    {
        printf("%s\n", *test);
        ++test;
    }
    free_matrix(tmp);
    return 0;
}
