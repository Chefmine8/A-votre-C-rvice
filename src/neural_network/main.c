#include "layer.h"
#include "neural_network.h"
#include <stdio.h>
#include <time.h>
int main()
{
    int lay[] = {784, 784, 26};
    // time_t t_create = time(NULL);
    struct neural_network *test = create_neural_network(3, lay);
    // printf("Time Creating the Neural Network: %ldsec\n", (time(NULL) -
    // t_create)); check_neural_network(test);

    neural_network_calculate_output(test);

    printf("Calculated loss for A %f", calculate_loss(test, 'A'));

    printf("\n");
    time_t t_free = time(NULL);

    free_neural_network(test);
    printf("\nTime Freeing The neural Network: %ldsec\n", time(NULL) - t_free);

    return 0;
}
