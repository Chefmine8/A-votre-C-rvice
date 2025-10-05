//
// Created by Evan on 01/10/2025.
//

#ifndef A_VOTRE_C_RVICE_NEURAL_NETWORK_H
#define A_VOTRE_C_RVICE_NEURAL_NETWORK_H

struct neural_network {

    int input_size;
    long double *inputs;

    int number_of_layers;
    struct layer **layers;

    int output_size;
    long double **outputs;
};

struct neural_network *create_neural_network(int size_of_arr, int arr[size_of_arr]);

void free_neural_network(struct neural_network *neural_network);

void check_neural_network(const struct neural_network *neural_network);

#endif //A_VOTRE_C_RVICE_NEURAL_NETWORK_H