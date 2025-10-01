//
// Created by Evan on 01/10/2025.
//

#include "neural_network.h"
#include "layer.h"

struct neural_network {

     int input_size;
     int *inputs;
     struct layer *layers;

     int output_size;
     int *outputs;
};

struct neural_network *create_network(int size_of_arr, int*arr)
{

}