//
// Created by Evan on 30/09/2025.
#include "layer.h"
#include <stdlib.h>
#include <err.h>
#include <stdio.h>
#include <math.h>


struct layer *create_layer(const int prev_layer_size, const int layer_size)
{

    struct layer *res = malloc(sizeof(struct layer));

    if (res == NULL) {
        errx(EXIT_FAILURE, "malloc failed");
    }

    res->prev_layer = NULL;

    res->outputs = malloc(layer_size * sizeof(float));

    if(res->outputs == NULL){
        errx(EXIT_FAILURE, "malloc failed");
    }

    res->is_output_layer = false;

    res->prev_layer_size = prev_layer_size;

    res->layer_size = layer_size;


    res->inputs = NULL;

    res->weights = malloc(layer_size * sizeof(float *));


    for (int i = 0; i < layer_size; i++)
    {
        res->weights[i] = malloc(prev_layer_size * sizeof(float));

        if(res->weights[i] == NULL){
            errx(EXIT_FAILURE, "malloc failed");
        }
        for (int j = 0; j < prev_layer_size; j++)
        {
            res->weights[i][j] = ((float)rand() / (float)RAND_MAX * 2.0 - 1.0);
        }
    }

    res->biases = malloc(layer_size * sizeof(float));

    for (int i = 0; i < layer_size; i++)
    {
        res->biases[i] =  (float)rand() / (float)RAND_MAX * 2 - 1;
    }

    return res;
}


void link_layers(struct layer **back_layer, struct layer **front_layer)
{
    if ((*back_layer)->layer_size != (*front_layer)->prev_layer_size) {
        errx(EXIT_FAILURE, "Trying to link two incompatible layers ! back_layer->layer_size (= %d) != front_layer->prev_layer_size (= %d)",(*back_layer)->layer_size, (*front_layer)->prev_layer_size );
    }
    // free((*front_layer)->prev_layer);
    (*front_layer)->prev_layer = *back_layer;
    (*front_layer)->inputs = &(*back_layer)->outputs;
}


void link_layer_output(struct layer *layer, const struct neural_network *neural_network)
{
    if (neural_network->output_size != layer->layer_size) {
        errx(EXIT_FAILURE, "layer not the same size as the outputs");
    }
    *neural_network->outputs = layer->outputs;
    layer->is_output_layer = true;
}


void link_layer_input(struct layer *layer, int input_size, float **inputs)
{
    if (input_size != layer->prev_layer_size) {
        errx(EXIT_FAILURE, "prev_layer_size not the same size as the inputs");
    }


    layer->inputs = inputs;
}

/**
 * Activation function used inside the neural network
 * @param layer The layer.
 */
float ReLU_activation_function(float input)
{
    return input > 0 ? input : 0;
}


/**
 * Activation function used for the last layer (output) of the neural network
 * @param layer The layer
 */

float soft_max_activation_function(float input)
{
    // printf("softmax input:\t%f\t->\t%f\n", input, 1.0 / (1.0 + exp(-input)));
    return 1.0 / (1.0 + exp(-input));
}
/*
void soft_max_activation_function_part2(float **arr, int size, float sum) {
    for (int i = 0; i < size; i++) {
        (*arr)[i] = (*arr)[i] / sum;
        if( isnanf((*arr)[i]) || isinff(sum) ) {
            for(int j = 0; j < size; j++){
                printf("%d:\t%f\n", j, (*arr)[j]);
            }
            errx(EXIT_FAILURE, "softmax output is nan at index %d: sum=%f", i, sum);
        }
    }
}
*/


 layer_calculate_output(const struct layer *layer)
{
    if (layer->prev_layer != NULL) {
        layer_calculate_output(layer->prev_layer);
    }

    float sum = 0;
    for (int i = 0; i < layer->layer_size; i++)
    {
        float output = 0;
        for (int j = 0; j < layer->prev_layer_size; j++)
        {


            output += (*(layer->inputs))[j] * layer->weights[i][j];

            if(isnanf(output) || isinff(output) ) {
                errx(EXIT_FAILURE, "layer nb %d : output is nan at ((*layer->inputs)[%d]=%f) * (weight[%d][%d]=%f) = %f %f\n", layer->layer_size, i, (*(layer->inputs))[j], i, j, layer->weights[i][j], (*(layer->inputs))[i] < 0.00001 ? 0 : (*(layer->inputs))[i] * layer->weights[i][j], output);
            }
        }

        layer->outputs[i] = layer->is_output_layer ? soft_max_activation_function(output) : ReLU_activation_function(output);
        if(isnanf(layer->outputs[i]) || isinff(layer->outputs[i]) ) {
            errx(EXIT_FAILURE, "layer nb %d : layer->outputs[%d] layer->is_output_layer=%d output=%f", layer->layer_size, i, layer->is_output_layer, output);
        }
        sum += layer->outputs[i];
    }
    /*
    if (layer->is_output_layer) {
        soft_max_activation_function_part2(&(layer->outputs), layer->layer_size, sum);
    }
    */
}


void free_layers(struct layer *layer) {
    if (layer->prev_layer != NULL) {
        free_layers(layer->prev_layer);
    }
    free((layer->biases));
    free(layer->outputs);

    for (int i = 0; i < layer->layer_size; i++) {
        free(layer->weights[i]);
    }
    free(layer->weights);
    free(layer);
}


void check_layer(const struct layer *layer) {
    layer_calculate_output(layer);
    printf("---------- layer nb: %d ----------\n", layer->layer_size);

    if (layer->prev_layer != NULL) {
        printf("input connected: %d\n",  (*layer->inputs) == (layer->prev_layer->outputs) );
    }

    printf("input is input of nn: %d\n", *layer->inputs[0] == 69 );

    printf("output is output of nn: %d\n\n", layer->outputs[0] == 69 && layer->is_output_layer );

    if (layer->prev_layer != NULL) {
        check_layer(layer->prev_layer);

    }

}


void print_values(const struct layer *layer) {
    printf("print layer %d\n", layer->layer_size);
    if (layer->prev_layer != NULL) {
        print_values(layer->prev_layer);
    }

    printf("########################################\n");
    for (int i = 0; i < layer->layer_size; ++i) {
        printf("output: %f\n", layer->outputs[i]);
        printf("bias: %f\nweights{\n", layer->biases[i]);
        for (int j = 0; j < layer->prev_layer_size; ++j) {
            printf("%f,\n", layer->weights[i][j]);
        }
        printf("}\n\n");
    }

    printf("end print layer %d\n", layer->layer_size);
}
