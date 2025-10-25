//
// Created by Evan on 01/10/2025.

#ifndef A_VOTRE_C_RVICE_NEURAL_NETWORK_H
#define A_VOTRE_C_RVICE_NEURAL_NETWORK_H

/**
 * The struct of the neural network
 */
struct neural_network {

    int input_size;
    long double *inputs;

    int number_of_layers;
    struct layer **layers;

    int *size_of_layers;
    long double **layers_middle_array;

    int output_size;
    long double **outputs;
};

/**
 * Create a struct neural_network
 * @param size_of_arr the size of arr
 * @param arr an array of ints representing the size of each layer (including inputs and outputs) of the neural network
 * @return the struct neural_network
 */
struct neural_network *create_neural_network(int size_of_arr, int arr[size_of_arr]);

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
void calculate_output(const struct neural_network *neural_network);

#endif //A_VOTRE_C_RVICE_NEURAL_NETWORK_H