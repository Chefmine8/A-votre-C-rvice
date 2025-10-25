#include "neural_network.h"
#include "layer.h"
#include <stdio.h>
#include <time.h>
int main()
{
    int lay[] = {1, 2, 3, 4, 5};
    time_t t_create = time(NULL);
    struct neural_network *test = create_neural_network(5, lay);
    printf("Time Creating the Neural Network: %ldsec\n", (time(NULL) - t_create));
    check_neural_network(test);

    time_t t = time(NULL);
    neural_network_calculate_output(test);
    printf("Time Calculating output : %ldsec\n", t - time(NULL));
    for (int i = 0; i < test->output_size; ++i) {
        printf("%Lg ", (*test->outputs)[i]);
    }
    printf("\n");
    time_t t_free = time(NULL);
    free_neural_network(test);
    printf("Time Freeing The neural Network: %ldsec\n", time(NULL) - t_free);

    return 0;
} 
