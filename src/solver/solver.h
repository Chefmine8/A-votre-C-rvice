#ifndef PARSER_H
#define PARSER_H
void upper(char *grid);
char **read_file(char *file);
void free_matrix(char **matrice);
struct coord *solver_algorithme(char **grid, char *word);
int *solv_rec(char **grid, char *word, int inc_x, int inc_y, int x, int y,
              int h, int w);

struct coord *solv(char *w);
struct coord {
    int x_0;
    int y_0;
    int x_1;
    int y_1;
};
#endif
