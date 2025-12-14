//
// Created by Evan on 01/10/2025.

#ifndef A_VOTRE_C_RVICE_NEURAL_NETWORK_H
#define A_VOTRE_C_RVICE_NEURAL_NETWORK_H

/**
 * The struct of the neural network
 */
struct neural_network
{

    /**
     * Size of inputs
     */
    int input_size;
    /**
     * An array of size input_size
     */
    float *inputs;

    /**
     * Number of layers inside the neural network
     */
    int number_of_layers;
    /**
     * Array of size number_of_layers
     */
    struct layer **layers;

    /**
     * Size of outputs
     */
    int output_size;
    /**
     * A pointer to an array of size output_size
     */
    float **outputs;
};

/**
 * Create a struct neural_network
 * @param size_of_arr the size of arr
 * @param arr an array of ints representing the size of each layer (including
 * inputs and outputs) of the neural network
 * @return the struct neural_network
 */
struct neural_network *create_neural_network(int size_of_arr,
                                             int arr[size_of_arr]);

/**
 * Free correctly a struct neural network
 * @param neural_network The neural network to free
 */
void free_neural_network(struct neural_network *neural_network);

/**
 * Debug function helping to see if the neural network has been built properly
 * @param neural_network The neural network to check
 */
void check_neural_network(const struct neural_network *neural_network);

/**
 * Calculates the output of the neural network
 * @param neural_network The neural network
 */
void neural_network_calculate_output(
    const struct neural_network *neural_network);

float calculate_loss(const struct neural_network *neural_network,
                     char expected_output);

char get_neural_network_output(const struct neural_network *neural_network);

int backprop_update(struct neural_network *neural_network, char expected_output,
                    float learning_rate);

int backprop_update_4(struct neural_network *neural_network,
                      char expected_output, float learning_rate);

void export_neural_network(struct neural_network *neural_network,
                           float learning_rate, int batch_size, int nb_sessions,
                           int success);
#endif // A_VOTRE_C_RVICE_NEURAL_NETWORK_H