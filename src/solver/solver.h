#ifndef PARSER_H
#define PARSER_H
void upper(char *grid);
char **read_file(char *file);
void free_matrix(char **matrice);
void solver_algorithme(char **grid, char *word);
int *solv_rec(char **grid, char *word, int inc_x, int inc_y, int x, int y,
              int h, int w);
#endif
