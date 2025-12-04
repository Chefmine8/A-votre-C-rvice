
#include "../training/importTrainingData.h"
#include "neural_network.h"
#include "layer.h"
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <dirent.h>
#include <string.h>
#include <stdint.h>


int main()
{
    srand(NULL);
    int lay[] = {784, 784, 784,26};
    time_t t_create = time(NULL);
    struct neural_network *test = create_neural_network(4, lay);
    printf("Time Creating the Neural Network: %ldsec\n", (time(NULL) - t_create));
    /*
    check_neural_network(test);

    print_values(test->layers[test->number_of_layers-1]);
    */
    printf("\n");
    time_t t_free = time(NULL);
    free_neural_network(test);
    printf("Time Freeing The neural Network: %ldsec\n", time(NULL) - t_free);

    return 0;
} 
