#include "neural_network.h"
#include "layer.h"
#include <stdio.h>
#include <time.h>
int main()
{
    int lay[] = {748 ,128, 26};
    struct neural_network *test = create_neural_network(3, lay);
    neural_network_calculate_output(test);
    b_p(test,'E',0.01);
    free_neural_network(test);
    printf("Time Freeing The neural Network: \n");

    return 0;
} 

