#include "solver.h"
#include "../global.h"

void upper(char *grid)
{
    while (*grid != '\0')
    {
        *grid = toupper((unsigned char)*grid);
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
            if (n_c >= capacity_c)
            {
                ++capacity_c;
                current_line = realloc(current_line, capacity_c * sizeof(char));
            }

            current_line[n_c] = '\0';
            upper(current_line);
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

struct coord *solver_algorithme(char **grid, char *word)
{
    int x = 0;
    int y = 0;
    upper(word);

    int height = 0;
    while (grid[height] != NULL)
    {
        height++;
    }
    int width = (height > 0) ? strlen(grid[0]) : 0;

    while (grid[y])
    {
        if (grid[y][x] == word[0])
        {
            if (word[1] == '\0')
            {
                printf("(%i,%i)(%i,%i)\n", x, y, x, y);
                return NULL;
            }

            int directions[8][2] = {{1, 0}, {-1, 0}, {0, 1},  {0, -1},
                                    {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};

            for (int i = 0; i < 8; ++i)
            {
                int inc_x = directions[i][0];
                int inc_y = directions[i][1];

                int next_x = x + inc_x;
                int next_y = y + inc_y;

                if (next_y >= 0 && next_y < height && next_x >= 0 &&
                    next_x < width)
                {
                    if (grid[next_y][next_x] == word[1])
                    {
                        int *result = solv_rec(grid, word + 1, inc_x, inc_y, x,
                                               y, height, width);

                        if (result[0] != -1)
                        {
                            printf("(%i,%i)(%i,%i)\n", x, y, result[0],
                                   result[1]);
                            int x_1 = result[0];
                            int y_1 = result[1];
                            free(result);
                            struct coord *c = malloc(sizeof(struct coord));
                            c->x_0 = x;
                            c->y_0 = y;
                            c->x_1 = x_1;
                            c->y_1 = y_1;
                            return c;
                        }
                        free(result);
                    }
                }
            }
        }
        x++;
        if (grid[y][x] == '\0')
        {
            y++;
            x = 0;
        }
    }
    printf("Not Found");
    return NULL;
}

int *solv_rec(char **grid, char *word, int inc_x, int inc_y, int x, int y,
              int h, int w)
{
    x += inc_x;
    y += inc_y;
    int *i = malloc(2 * sizeof(int));
    i[0] = -1;
    i[1] = -1;

    if (x < 0 || x >= w || y < 0 || y >= h)
    {
        return i;
    }

    if (grid[y][x] == *word)
    {
        if (*(word + 1) == '\0' || *(word + 1) == '\n')
        {
            i[0] = x;
            i[1] = y;
            return i;
        }
        else
        {
            free(i);
            return solv_rec(grid, word + 1, inc_x, inc_y, x, y, h, w);
        }
    }

    return i;
}

struct coord *solv(char *w)
{
    char **grid = read_file("img");
    struct coord *c = solver_algorithme(grid, w);
    free_matrix(grid);
    return c;
}
