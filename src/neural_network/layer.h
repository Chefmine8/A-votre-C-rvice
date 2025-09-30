//
// Created by Evan on 30/09/2025.
//

#ifndef A_VOTRE_C_RVICE_LAYER_H
#define A_VOTRE_C_RVICE_LAYER_H
struct layer;

struct layer create_layer(const int prev_layer_size, const int layer_size, int *input, int **weight, int *biases);

int *calculate_outputs(const struct layer layer);

#endif //A_VOTRE_C_RVICE_LAYER_H