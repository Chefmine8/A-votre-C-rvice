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

    res->outputs = calloc(sizeof(long double ) , layer_size);
    res->is_output_layer = false;

    res->prev_layer_size = prev_layer_size;
    res->layer_size = layer_size;

    res->inputs = NULL;

    res->weights = malloc(layer_size * sizeof(long double *));

    for (int i = 0; i < layer_size; i++)
    {
        if(isnanf(res->outputs[i])){
            errx(EXIT_FAILURE, "layer->outputs is nan");
        }
        res->weights[i] = malloc(prev_layer_size * sizeof(long double));
        for (int j = 0; j < prev_layer_size; j++)
        {
            res->weights[i][j] = (long double)rand() / (long double)RAND_MAX * 2 - 1;
        }
    }

    res->biases = malloc(layer_size * sizeof(long double));

    for (int i = 0; i < layer_size; i++)
    {
        res->biases[i] =  (long double)rand() / (long double)RAND_MAX * 2 - 1;
    }

    return res;
}


void link_layers(struct layer **back_layer, struct layer **front_layer)
{
    printf("link layer 1 1\n");
    printf("link layer %p\n", (*back_layer));
    printf("link layer %p\n", (*front_layer));
    if ((*back_layer)->layer_size != (*front_layer)->prev_layer_size) {
        errx(EXIT_FAILURE, "Trying to link two incompatible layers ! back_layer->layer_size (= %d) != front_layer->prev_layer_size (= %d)",(*back_layer)->layer_size, (*front_layer)->prev_layer_size );
    }
    // free((*front_layer)->prev_layer);
    printf("link layer 1 2\n");
    (*front_layer)->prev_layer = *back_layer;
    printf("link layer 1 3\n");
    (*front_layer)->inputs = &(*back_layer)->outputs;

    printf("link layer 1 end\n");
}


void link_layer_output(struct layer *layer, const struct neural_network *neural_network)
{
    printf("1\n");
    printf("nn: %p, l: %p\n", neural_network, layer);
    printf("nn: %d, l: %d\n", neural_network->output_size, layer->layer_size);
    if (neural_network->output_size != layer->layer_size) {
        errx(EXIT_FAILURE, "layer not the same size as the outputs");
    }

    printf("2\n");
    if(layer->outputs != NULL){
        free(layer->outputs);
    }
    printf("3 %p\n", neural_network->outputs);
    *neural_network->outputs = layer->outputs;
    printf("4\n");
    layer->is_output_layer = true;
    printf("end\n");
}


void link_layer_input(struct layer *layer, int input_size, long double **inputs)
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
long double ReLU_activation_function(long double input)
{
    return input > 0 ? input : 0;
}


/**
 * Activation function used for the last layer (output) of the neural network
 * @param layer The layer
 */

long double soft_max_activation_function_part1(long double input)
{
    return exp(input);
}

void soft_max_activation_function_part2(long double **arr, int size, long double sum) {
    for (int i = 0; i < size; i++) {
        (*arr)[i] = (*arr)[i] / sum;
    }
}


void layer_calculate_output(const struct layer *layer)
{
    if (layer->prev_layer != NULL) {
        layer_calculate_output(layer->prev_layer);
    }

    long double sum = 0;
    for (int i = 0; i < layer->layer_size; i++)
    {
        if(isnanf(layer->outputs[i])){
            layer->outputs[i] = 0;
            //errx(EXIT_FAILURE, "layer->outputs[i] is nan");
        }

        long double output = 0;
        for (int j = 0; j < layer->prev_layer_size; j++)
        {
            output += (*layer->inputs)[j] * layer->weights[i][j];
        }

        layer->outputs[i] = layer->is_output_layer ? soft_max_activation_function_part1(output) : ReLU_activation_function(output);
        sum += layer->outputs[i];
    }

    if (layer->is_output_layer) {
        soft_max_activation_function_part2(&(layer->outputs), layer->layer_size, sum);
    }

}


void free_layers(struct layer *layer) {
    printf("fl 1.%d\n", layer->layer_size);
    if (layer->prev_layer != NULL) {
        printf("fl recccall.%d\n", layer->layer_size);
        free_layers(layer->prev_layer);
    }
    printf("fl 2.%d\n", layer->layer_size);
    free((layer->biases));
    printf("fl 3.%d\n", layer->layer_size);
    free(layer->outputs);

    printf("fl 4.%d\n", layer->layer_size);

    printf("fl END.%d\n", layer->layer_size);
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
        printf("output: %Lg\n", layer->outputs[i]);
        printf("bias: %Lg\nweights{\n", layer->biases[i]);
        for (int j = 0; j < layer->prev_layer_size; ++j) {
            printf("%Lg,\n", layer->weights[i][j]);
        }
        printf("}\n\n");
    }

    printf("end print layer %d\n", layer->layer_size);
}