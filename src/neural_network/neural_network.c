//
// Created by Evan on 01/10/2025.
//

#include "neural_network.h"
#include <stdlib.h>
#include <err.h>
#include "layer.h"
#include <stdio.h>
#include <math.h>
#include <time.h>

struct neural_network *create_neural_network(int size_of_arr, int arr[size_of_arr])
{

     if (size_of_arr < 2) {
          errx(EXIT_FAILURE, "Do you have a brain of that size ?");
     }
     struct neural_network *neural_network = malloc(sizeof(struct neural_network));

     neural_network->number_of_layers = size_of_arr - 1;

     neural_network->inputs = malloc(sizeof(float) * arr[0]);
     neural_network->input_size = arr[0];

     neural_network->output_size = arr[size_of_arr - 1];
     neural_network->outputs = malloc(sizeof(float *));

     neural_network->layers = malloc(sizeof(struct layer *) * size_of_arr);

     neural_network->layers[0] = create_layer(arr[0], arr[1]);
     link_layer_input(neural_network->layers[0], neural_network->input_size, &neural_network->inputs);

     for(int i = 2; i < size_of_arr; i++)
     {
          neural_network->layers[i-1] = create_layer(arr[i-1], arr[i]);

          link_layers(&neural_network->layers[i - 2], &neural_network->layers[i-1]);
     }

     link_layer_output(neural_network->layers[size_of_arr - 2], neural_network);
     return neural_network;
}


void free_neural_network(struct neural_network *neural_network) {
     free_layers(neural_network->layers[neural_network->number_of_layers - 1]);
     free(neural_network->inputs);
     free(neural_network);
}


void check_neural_network(const struct neural_network *neural_network) {

     neural_network->inputs[0] = 69;

     ((*(neural_network->outputs))[0]) = 69;
     for(int i = 0; i<neural_network->number_of_layers;i++)
     {
        check_layer(neural_network->layers[i]);
        //print_values(neural_network->layers[i]);
     }

}


void neural_network_calculate_output(const struct neural_network *neural_network) {
     layer_calculate_output( neural_network->layers[neural_network->number_of_layers -1] );
}

/**
 * Calculate the loss of the neural network using the categorical cross-entropy function
 * @param neural_network The neural Network
 * @param expected_output The expected output
 * @return The calculated loss
 */
float calculate_loss(const struct neural_network *neural_network, char expected_output) {

    return -logf((*(neural_network->outputs))[expected_output - 'A']); // + 1e-15f);
    /*float res = 0.0;
    for(int i = 0; i < neural_network->output_size; i++) {
        float y_true = (i == (expected_output - 'A')) ? 1.0 : 0.0;
        float y_pred = (*(neural_network->outputs))[i];
        res += (y_true - y_pred) * (y_true - y_pred);
    }

    return  1.0/((float)neural_network->output_size) * res;*/
}




char get_neural_network_output(const struct neural_network *neural_network) {
     float max = (*neural_network->outputs)[0];
     float max_index = 0;
     for (int i = 1; i < neural_network->output_size; ++i) {
          if (max < (*neural_network->outputs)[i]) {
               max = (*neural_network->outputs)[i];
               max_index = i;
          }
     }
    return 'A' + max_index;
}


int backprop_update_5(struct neural_network *neural_network, char expected_output, float learning_rate)
{
    if (!neural_network || neural_network->number_of_layers <= 0) return -1;

    neural_network_calculate_output(neural_network);

    int L = neural_network->number_of_layers;
    int out_idx = expected_output - 'A';
    if (out_idx < 0) return -1;

    /* allocate deltas per layer */
    float **deltas = malloc(sizeof(float*) * L);
    if (!deltas) return -1;
    for (int li = 0; li < L; ++li) {
        int sz = neural_network->layers[li]->layer_size;
        deltas[li] = calloc(sz, sizeof(float));
        if (!deltas[li]) {
            for (int k = 0; k < li; ++k) free(deltas[k]);
            free(deltas);
            return -1;
        }
    }

    /* output layer delta: a_k - y_k */
    struct layer *out_layer = neural_network->layers[L - 1];
    if (out_idx >= out_layer->layer_size) {
        for (int k = 0; k < L; ++k) free(deltas[k]);
        free(deltas);
        errx(EXIT_FAILURE, "expected_output out of range");
    }
    for (int k = 0; k < out_layer->layer_size; ++k) {
        float a_k = out_layer->outputs[k];
        float y_true = (k == out_idx) ? 1.0f : 0.0f;
        deltas[L - 1][k] = a_k - y_true;
        if (isnanf(deltas[L - 1][k]) || isinff(deltas[L - 1][k])) {
            for (int t = 0; t < L; ++t) free(deltas[t]);
            free(deltas);
            errx(EXIT_FAILURE, "output delta became NaN/Inf");
        }
    }

    /* backpropagate deltas */
    for (int li = L - 2; li >= 0; --li) {
        struct layer *cur = neural_network->layers[li];
        struct layer *next = neural_network->layers[li + 1];
        for (int i = 0; i < cur->layer_size; ++i) {
            float sum = 0.0f;
            for (int k = 0; k < next->layer_size; ++k) {
                sum += next->weights[k][i] * deltas[li + 1][k];
            }
            /* Use ReLU derivative for hidden layers (activation is ReLU) */
            float deriv = cur->z[i] > 0.0f ? 1.0f : 0.0f;
            deltas[li][i] = sum * deriv;
            if (isnanf(deltas[li][i]) || isinff(deltas[li][i])) {
                for (int t = 0; t < L; ++t) free(deltas[t]);
                free(deltas);
                errx(EXIT_FAILURE, "hidden delta became NaN/Inf");
            }
        }
    }

    /* apply gradients */
    for (int li = 0; li < L; ++li) {
        struct layer *layer = neural_network->layers[li];
        float *input_arr = (li == 0) ? neural_network->inputs : neural_network->layers[li - 1]->outputs;
        if (!input_arr) {
            for (int t = 0; t < L; ++t) free(deltas[t]);
            free(deltas);
            errx(EXIT_FAILURE, "input array for layer is NULL");
        }
        for (int i = 0; i < layer->layer_size; ++i) {
            float delta_i = deltas[li][i];

            /* bias update */
            layer->biases[i] -= learning_rate * delta_i;
            if (isnanf(layer->biases[i]) || isinff(layer->biases[i])) {
                for (int t = 0; t < L; ++t) free(deltas[t]);
                free(deltas);
                errx(EXIT_FAILURE, "bias became NaN/Inf during update");
            }

            /* weights update */
            for (int j = 0; j < layer->prev_layer_size; ++j) {
                float grad = delta_i * input_arr[j];
                layer->weights[i][j] -= learning_rate * grad;
                if (isnanf(layer->weights[i][j]) || isinff(layer->weights[i][j])) {
                    for (int t = 0; t < L; ++t) free(deltas[t]);
                    free(deltas);
                    errx(EXIT_FAILURE, "weight became NaN/Inf during update");
                }
            }
        }
    }

    for (int t = 0; t < L; ++t) free(deltas[t]);
    free(deltas);
    return 0;
}



int backprop_update_4(struct neural_network *neural_network, char expected_output, float learning_rate)
{
    if(learning_rate <= 0.0f || learning_rate > 1.0f) {
        errx(EXIT_FAILURE, "Learning rate is invalid: %f", learning_rate);
    }
    neural_network_calculate_output(neural_network);
    // k -> [0, layer_size]
    // j ->  [0, prev_layer_size]
    // L = last layer
    //printf("1\n");
    float *sigma_L_k = malloc(sizeof(float) * neural_network->output_size ); // dE / dz^L_k
    for (int i = 0; i < neural_network->output_size; i++) {
        sigma_L_k[i] = ((*(neural_network->outputs))[i] - (i == (expected_output - 'A')) + 1e-15f); // cross-entropy loss with softmax
        // printf("sigma_L_k[%d]=%f\t neural_network->outputs[%d]=%f\t expected_output=%c actual_output=%c %c\n", i, sigma_L_k[i], i, (*neural_network->outputs)[i], expected_output, get_neural_network_output(neural_network), i + 'A');
        if(isnanf(sigma_L_k[i]) || isinff(sigma_L_k[i]) || sigma_L_k[i] > 1.0e5 || sigma_L_k[i] < -1.0e5) {
            errx(EXIT_FAILURE, "sigma_L_k[%d] = %f\t neural_network->outputs[%d]=%f\t expected_output=%c\n", i, sigma_L_k[i], i, (*neural_network->outputs)[i], expected_output);
        }
    }
    //printf("2\n");
    float *aL1j = neural_network->layers[neural_network->number_of_layers - 2]->outputs; // a^(L-1)_j

    float **dE_WLkj = malloc(sizeof(float*) * neural_network->output_size); // dE / dW^L_kj
    for(int k = 0; k < neural_network->output_size; k++) {
        dE_WLkj[k] = malloc(sizeof(float) * neural_network->layers[neural_network->number_of_layers - 1]->prev_layer_size);
        for(int j = 0; j < neural_network->layers[neural_network->number_of_layers - 1]->prev_layer_size; j++) {
            dE_WLkj[k][j] = sigma_L_k[k] * aL1j[j];
            // if(dE_WLkj[k][j] > 0.001 || dE_WLkj[k][j] < -0.001)
            //    printf("dE_WLkj[%d][%d]=%f\t sigma_L_k[k]=%f\t aL1j[j]=%f\t\tsigma_L_k[i]=%f\n", k, j, dE_WLkj[k][j], sigma_L_k[k], aL1j[j], sigma_L_k[k]);
            if(isnanf(dE_WLkj[k][j]) || isinff(dE_WLkj[k][j]) || dE_WLkj[k][j] > 1.0e5 || dE_WLkj[k][j] < -1.0e5) {
                errx(EXIT_FAILURE, "dE_WLkj[%d][%d] = %f\t sigma_L_k[k]=%f\t aL1j[j]=%f\n", k, j, dE_WLkj[k][j], sigma_L_k[k], aL1j[j]);
            }
        }
    }
    //printf("3\n");
    float *dE_bLk = sigma_L_k; // dE / db^L_k

    for(int k = 0; k < neural_network->output_size; k++) {
        for(int j = 0; j < neural_network->layers[neural_network->number_of_layers - 1]->prev_layer_size; j++) {
            neural_network->layers[neural_network->number_of_layers - 1]->weights[k][j] -= learning_rate * dE_WLkj[k][j];
            //if( -0.001 < (- learning_rate * dE_WLkj[k][j]) || (- learning_rate * dE_WLkj[k][j]) > 0.001) {
            //    printf("-learning_rate=%f\tdE_WLkj[k][j]=%f \n", - learning_rate, dE_WLkj[k][j]);
            //}
            if(isnanf(neural_network->layers[neural_network->number_of_layers - 1]->weights[k][j]) || isinff(neural_network->layers[neural_network->number_of_layers - 1]->weights[k][j]) || neural_network->layers[neural_network->number_of_layers - 1]->weights[k][j] > 1.0e5 || neural_network->layers[neural_network->number_of_layers - 1]->weights[k][j] < -1.0e5) {
                errx(EXIT_FAILURE, "neural_network->layers[%d]->weights[%d][%d] = %f\t dE_WLkj[k][j]=%f\n", neural_network->number_of_layers - 1, k, j, neural_network->layers[neural_network->number_of_layers - 1]->weights[k][j], dE_WLkj[k][j]);
            }
        }
        neural_network->layers[neural_network->number_of_layers - 1]->biases[k] -= learning_rate * dE_bLk[k];
        if(isnanf(neural_network->layers[neural_network->number_of_layers - 1]->biases[k]) || isinff(neural_network->layers[neural_network->number_of_layers - 1]->biases[k]) || neural_network->layers[neural_network->number_of_layers - 1]->biases[k] > 1.0e5 || neural_network->layers[neural_network->number_of_layers - 1]->biases[k] < -1.0e5) {
            errx(EXIT_FAILURE, "neural_network->layers[%d]->biases[%d] = %f\t dE_bLk[k]=%f\n", neural_network->number_of_layers - 1, k, neural_network->layers[neural_network->number_of_layers - 1]->biases[k], dE_bLk[k]);
        }
    }

    // Backprop for last layer is done
    // l-1 -> [L - 1 to 0]
    //printf("4\n");
    for(int l = neural_network->number_of_layers - 1; l > 0; l--) {
        //printf("\t1 %d\n", l);
        float *sigma_l1_j = malloc(sizeof(float) * neural_network->layers[l-1]->layer_size); // sigma^(l-1)_j
        for(int j = 0; j < neural_network->layers[l-1]->layer_size; j++) {
            sigma_l1_j[j] = 0;
            for(int k = 0; k < neural_network->layers[l]->layer_size; k++) {
                sigma_l1_j[j] += sigma_L_k[k] * neural_network->layers[l]->weights[k][j];
            }
            sigma_l1_j[j] *= deriv_ReLU_activation_function(neural_network->layers[l-1]->z[j]);
        }
        //printf("\t2 %d\n", l);
        float **dE_Wlkj = malloc(sizeof(float*) * neural_network->layers[l]->layer_size); // dE / dW^(l-1)_jk
        for(int k = 0; k < neural_network->layers[l]->layer_size; k++) {
            dE_Wlkj[k] = malloc(sizeof(float) * neural_network->layers[l]->prev_layer_size);
            for(int j = 0; j < neural_network->layers[l-1]->layer_size; j++) {
                dE_Wlkj[k][j] = sigma_L_k[k] * neural_network->layers[l-1]->outputs[j];
                // printf("dE_Wlkj[%d][%d]=%f\t sigma_L_k[k]=%f\t neural_network->layers[l-1]->outputs[j]=%f\n", k, j, dE_Wlkj[k][j], sigma_L_k[k], neural_network->layers[l-1]->outputs[j]);
            }
        }
        //printf("\t3 %d\n", l);
        float *dE_blk = sigma_L_k; // dE / db^(l-1)_j

        for(int k = 0; k < neural_network->layers[l]->layer_size; k++) { // Apply the gradients
            for(int j = 0; j < neural_network->layers[l]->prev_layer_size; j++) {
                // printf("neural_network->layers[l]->weights[k][j]=%f\t->\t %f\tdelta=%f\n", neural_network->layers[l]->weights[k][j], neural_network->layers[l]->weights[k][j] - learning_rate * dE_Wlkj[k][j], - learning_rate * dE_Wlkj[k][j]);
                neural_network->layers[l]->weights[k][j] -= learning_rate * dE_Wlkj[k][j];
                if(isnanf(neural_network->layers[l]->weights[k][j]) || isinff(neural_network->layers[l]->weights[k][j]) || neural_network->layers[l]->weights[k][j] > 1.0e5 || neural_network->layers[l]->weights[k][j] < -1.0e5) {
                    errx(EXIT_FAILURE, "neural_network->layers[%d]->weights[%d][%d] = %f\t dE_Wlkj[k][j]=%f\n", l, k, j, neural_network->layers[l]->weights[k][j], dE_Wlkj[k][j]);
                }
            }
            neural_network->layers[l]->biases[k] -= learning_rate * dE_blk[k];
            if(isnanf(neural_network->layers[l]->biases[k]) || isinff(neural_network->layers[l]->biases[k]) || neural_network->layers[l]->biases[k] > 1.0e5 || neural_network->layers[l]->biases[k] < -1.0e5) {
                errx(EXIT_FAILURE, "neural_network->layers[%d]->biases[%d] = %f\t dE_blk[k]=%f\n", l, k, neural_network->layers[l]->biases[k], dE_blk[k]);
            }
        }
        //printf("\t4 %d\n", l);

        free(sigma_L_k); // free things
        sigma_L_k = sigma_l1_j;
        for(int k = 0; k < neural_network->layers[l]->layer_size; k++) {
            free(dE_Wlkj[k]);
        }
        free(dE_Wlkj);
        //printf("\t5 %d\n", l);
    }
    //printf("5\n");
    free(sigma_L_k);
    for(int k = 0; k < neural_network->output_size; k++) {
        free(dE_WLkj[k]);
    }
    free(dE_WLkj);
    //printf("6\n");
    // https://doug919.github.io/notes-on-backpropagation-with-cross-entropy/
    return 0;
}

void export_neural_network(struct neural_network *neural_network, float learning_rate, int batch_size, int nb_sessions, int success)
{
    FILE *file = fopen("exported_neural_network.nnn", "w");

    fprintf(file, "Learning Rate: %f\t| Batch Size: %d\t| Number of sessions : %d\t| Number of Success: %d\n##############################\n", learning_rate, batch_size, nb_sessions, success);

    fprintf(file, "%d ", neural_network->input_size);

    for (int i = 0; i < neural_network->number_of_layers; i++)
    {
        fprintf(file, "%d ", neural_network->layers[i]->layer_size);
    }
    fprintf(file, "\n##############################\n");

    for (int i = 0; i < neural_network->number_of_layers ; ++i)
    {
         fprintf(file, "Bias:\n");
         for (int j = 0; j < neural_network->layers[i]->layer_size; ++j)
         {
            fprintf(file, "\t%f", neural_network->layers[i]->biases[j]);
         }

        fprintf(file, "\nWeights:\n");
        for (int j = 0; j < neural_network->layers[i]->prev_layer_size; ++j)
        {
            for (int k = 0; k < neural_network->layers[i]->layer_size; ++k)
            {
                fprintf(file, "\t%f", neural_network->layers[i]->weights[k][j]);
            }
             fprintf(file, "\n");
        }
         fprintf(file, "\n");
    }

    fclose(file);
}

void import_neural_network(struct neural_network *neural_network, const char *filename)
{
    FILE *file = fopen(filename, "r");

    char str[1000000];
    int is_bias = 0;
    int is_weights = 0;
    int l = 0;
    while(fgets(str, 1000000, file) != NULL)
    {
        if(is_bias)
        {
            for(int i = 0; str[i] != '\0'; i++)
            {
                 char *bias = strtok(&str[i],  "\t ");
                 while(bias != NULL)
                 {
                      neural_network->layers[is_bias - 1]->biases[i] = atof(bias);
                      bias = strtok(NULL, "\t");
                 }
            }
        }

        is_bias str[0] == 'B';
        is_weights str[0] == 'W';
    }

    fclose(file);
}