//
// Created by Evan on 30/09/2025.
//

#ifndef A_VOTRE_C_RVICE_LAYER_H
#define A_VOTRE_C_RVICE_LAYER_H



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
    long double **inputs;

    /**
     * weights[layer_size][prev_layer_size]
     */
    long double **weights;


    /**
     * biases[layer_size]
     */
    long double *biases;

    /**
     * /!\ USE update_outputs() BEFORE USING!
     * pointer of output[layer_size]
     */
    long double *outputs;
};

/**
 *
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
 * @param output_size the size of outputs (for security)
 * @param outputs the array of size output_size that will contain the output of the neural network
 */
void link_layer_output(struct layer *layer, int output_size, long double **outputs);

/**
 * Link the last layer of the neural network to an array
 * @param layer the layer to link
 * @param input_size the size of inputs (for security)
 * @param inputs the array of size input_size that will contain the input of the neural network
 */
void link_layer_input(struct layer *layer, int input_size, long double **inputs);

/**
 *
 * @param layer The layer to calculate the output.
 * @return The output. \n /!\ DON'T FORGET TO FREE THE OUTPUT ONCE DONE
 */
void update_outputs(const struct layer *layer);

void free_layers(struct layer *layer);

void check_layer(const struct layer *layer);

#endif //A_VOTRE_C_RVICE_LAYER_H