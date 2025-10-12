#include "neural_network.h"
#include "layer.h"
#include <stdio.h>
#include <time.h>
int main()
{
    int lay[] = {5, 5, 5, 5, 5};
    time_t t_create = time(NULL);
    struct neural_network *test = create_neural_network(4, lay);
    printf("Time Creating the Neural Network: %ldsec\n", (time(NULL) - t_create));
    check_neural_network(test);

    time_t t = time(NULL);
    calculate_output(test);
    printf("Time Calculating output : %ldsec\n", t - time(NULL));

    time_t t_free = time(NULL);
    free_neural_network(test);
    printf("Time Freeing The neural Network: %ldsec\n", time(NULL) - t_free);

    return 0;
} 
