#include "link_cells_to_nn.h"

int main()
{
    int nb_arr = 3;
    int arr[3] = {784, 784, 26};
    struct neural_network *nn = create_neural_network(nb_arr, arr);
    create_grid("img", nn);
    create_word_list("word_list", nn);
    free_neural_network(nn);
    return 0;
}
