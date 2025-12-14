//
// Created by Evan on 30/09/2025.
//

#ifndef A_VOTRE_C_RVICE_LAYER_H
#define A_VOTRE_C_RVICE_LAYER_H

#include <stdbool.h>
#include "neural_network.h"

/**
 * struct representing a layer of the neural network
 */
struct layer
{
    /**
     * Previous layer, NULL if input of neural network
     */
    struct layer *prev_layer;

    /**
     * The layer_size of the previous layer (or input of the neural network).
     */
    int prev_layer_size;

    /**
     * Number of neuron on this layer.
     */
    int layer_size;


    /**
     * pointer of inputs[prev_layer_size]
     */
    float **inputs;

    /**
     * weights[layer_size][prev_layer_size]
     */
    float **weights;


    /**
     * biases[layer_size]
     */
    float *biases;

    /**
     * /!\ USE layer_calculate_output() BEFORE USING!
     *  output[layer_size]
     */
    float *outputs;

    bool is_output_layer;
};

/**
 * create a struct layer
 * @param prev_layer_size size of the previous layer
 * @param layer_size size of this layer
 * @return the layer struct
 */
struct layer *create_layer(const int prev_layer_size, const int layer_size);

/**
 * Link two internals layers together
 * @param back_layer the layer closest to the input
 * @param front_layer the layer closest to the output
 */
void link_layers(struct layer **back_layer, struct layer **front_layer);

/**
 * Link the last layer of the neural network to an array
 * @param layer the layer to link
 * @param neural_network the neural network the layer will belong to
 */
void link_layer_output(struct layer *layer, const struct neural_network *neural_network);

/**
 * Link the last layer of the neural network to an array
 * @param layer the layer to link
 * @param input_size the size of inputs (for security)
 * @param inputs the array of size input_size that will contain the input of the neural network
 */
void link_layer_input(struct layer *layer, int input_size, float **inputs);

/**
 * Update the layer->outputs array, use before reading the values
 * @param layer The layer
 */
void layer_calculate_output(const struct layer *layer);

/**
 * Free the struct layer and all of its arrays
 * @param layer The layer to free
 */
void free_layers(struct layer *layer);

/**
 * Debug function helping to see if the layer has been build properly
 * @param layer The layer
 */
void check_layer(const struct layer *layer);

void print_values(const struct layer *layer);

float dot_product(const float *a, const float *b, int size);

float deriv_sigmoid(float input);
#endif //A_VOTRE_C_RVICE_LAYER_H