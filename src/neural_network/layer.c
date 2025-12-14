//
// Created by Evan on 30/09/2025.
#include "layer.h"
#include <err.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

struct layer *create_layer(const int prev_layer_size, const int layer_size)
{

    struct layer *res = malloc(sizeof(struct layer));

    if (res == NULL)
    {
        errx(EXIT_FAILURE, "malloc failed");
    }

    res->prev_layer = NULL;

    res->outputs = malloc(layer_size * sizeof(float));
    res->z = malloc(layer_size * sizeof(float));
    if (res->outputs == NULL)
    {
        errx(EXIT_FAILURE, "malloc failed");
    }

    res->is_output_layer = false;

    res->prev_layer_size = prev_layer_size;

    res->layer_size = layer_size;

    res->inputs = NULL;

    res->weights = malloc(layer_size * sizeof(float *));

    // float limit = sqrtf(2.0 / (prev_layer_size + layer_size));
    for (int i = 0; i < layer_size; i++)
    {
        res->weights[i] = malloc(prev_layer_size * sizeof(float));

        if (res->weights[i] == NULL)
        {
            errx(EXIT_FAILURE, "malloc failed");
        }
        for (int j = 0; j < prev_layer_size; j++)
        {
            res->weights[i][j] =
                ((float)rand() / (float)RAND_MAX * 2.0 -
                 1.0); // ((double)rand() / RAND_MAX) * 2 * limit - limit; //
            // if(res->weights[i][j] < -1.0 || res->weights[i][j] > 1.0) {
            //     printf("weight[%d][%d]=%f\n", i, j, res->weights[i][j]);
            // }
        }
    }

    res->biases = malloc(layer_size * sizeof(float));

    for (int i = 0; i < layer_size; i++)
    {
        res->biases[i] = (float)rand() / (float)RAND_MAX * 2 - 1;
        if (res->biases[i] < -1.0 || res->biases[i] > 1.0)
        {
            printf("biases[%d]=%f\n", i, res->biases[i]);
        }
    }

    return res;
}

void link_layers(struct layer **back_layer, struct layer **front_layer)
{
    if ((*back_layer)->layer_size != (*front_layer)->prev_layer_size)
    {
        errx(EXIT_FAILURE,
             "Trying to link two incompatible layers ! back_layer->layer_size "
             "(= %d) != front_layer->prev_layer_size (= %d)",
             (*back_layer)->layer_size, (*front_layer)->prev_layer_size);
    }
    // free((*front_layer)->prev_layer);
    (*front_layer)->prev_layer = *back_layer;
    (*front_layer)->inputs = &(*back_layer)->outputs;
}

void link_layer_output(struct layer *layer,
                       const struct neural_network *neural_network)
{
    if (neural_network->output_size != layer->layer_size)
    {
        errx(EXIT_FAILURE, "layer not the same size as the outputs");
    }
    *(neural_network->outputs) = layer->outputs;
    layer->is_output_layer = true;
}

void link_layer_input(struct layer *layer, int input_size, float **inputs)
{
    if (input_size != layer->prev_layer_size)
    {
        errx(EXIT_FAILURE, "prev_layer_size not the same size as the inputs");
    }

    layer->inputs = inputs;
}

/**
 * Activation function used inside the neural network
 * @param layer The layer.
 */
void ReLU_activation_function(const struct layer *layer)
{
    for (int i = 0; i < layer->layer_size; i++)
    {
        layer->outputs[i] = layer->z[i] > 0 ? layer->z[i] : 0;
    }
}

float deriv_ReLU_activation_function(float input) { return input > 0 ? 1 : 0; }

float sigmoid_activation_function(float input)
{
    return 1.0f / (1.0f + expf(-input));
}

float deriv_sigmoid(float input)
{
    float sig = sigmoid_activation_function(input);
    return sig * (1.0f - sig);
}

/**
 * Activation function used for the last layer (output) of the neural network
 * @param layer The layer
 */

void softmax_activation_function(const struct layer *layer, float max)
{
    float sum = 0;
    for (int i = 0; i < layer->layer_size; i++)
    {
        layer->outputs[i] = expf(layer->z[i] - max);
        if (isnanf(layer->outputs[i]) || isinff(layer->outputs[i]))
        {

            errx(EXIT_FAILURE,
                 "layer nb %d : expf(layer->z[%d]=%f - max=%f) = %f\n",
                 layer->layer_size, i, layer->z[i], max, layer->outputs[i]);
        }
        sum += layer->outputs[i];
        if (isnanf(sum) || isinff(sum))
        {

            errx(EXIT_FAILURE,
                 "layer nb %d : sum after adding layer->outputs[%d]=%f is nan "
                 "or inf: sum=%f\n",
                 layer->layer_size, i, layer->outputs[i], sum);
        }
    }
    for (int i = 0; i < layer->layer_size; i++)
    {
        float old_output = layer->outputs[i];
        layer->outputs[i] = layer->outputs[i] / sum;
        if (isnanf(layer->outputs[i]) || isinff(layer->outputs[i]))
        {

            errx(EXIT_FAILURE,
                 "layer nb %d : layer->outputs[%d]=%f / sum=%f = %f\n",
                 layer->layer_size, i, old_output, sum, layer->outputs[i]);
        }
    }
}

float dot_product(const float *a, const float *b, int size)
{
    float res = 0;
    for (int i = 0; i < size; i++)
    {
        res += a[i] * b[i];
    }
    return res;
}

void layer_calculate_output(const struct layer *layer)
{
    if (layer->prev_layer != NULL)
    {
        layer_calculate_output(layer->prev_layer);
    }

    float max = -10000;
    for (int i = 0; i < layer->layer_size; i++)
    {
        float output = 0.0;
        for (int j = 0; j < layer->prev_layer_size; j++)
        {

            output += (*(layer->inputs))[j] * layer->weights[i][j];
            // printf("layer[%d]\tinput[%d]=%f\t*\tweight[%d][%d]=%f\t=\t%f\t->
            // output=%f\n", layer->layer_size, j, (*(layer->inputs))[j], i, j,
            // layer->weights[i][j], (*(layer->inputs))[j] *
            // layer->weights[i][j], output);
            if (output < -1e10 || output > 1e10 || isnanf(output) ||
                isinff(output))
            {
                errx(EXIT_FAILURE,
                     "layer nb %d : output is nan at ((*layer->inputs)[%d]=%f) "
                     "* (weight[%d][%d]=%f) = %f %f\n",
                     layer->layer_size, i, (*(layer->inputs))[j], i, j,
                     layer->weights[i][j],
                     (*(layer->inputs))[j] < 0.00001
                         ? 0
                         : (*(layer->inputs))[j] * layer->weights[i][j],
                     output);
            }
        }
        // printf("output=%f\tbias=%f\n", output, layer->biases[i]);
        layer->z[i] = output + layer->biases[i];
        // printf("max=%f layer->z[%d]=%f\n", max, i, layer->z[i]);
        if (max < layer->z[i])
        {
            max = layer->z[i];
        }
        // printf("output + layer->biases[i] = %f\n", output +
        // layer->biases[i]);

        // if(layer->outputs[i] < -1e10 || layer->outputs[i] > 1e10 ||
        // isnanf(layer->outputs[i]) || isinff(layer->outputs[i]) ) {
        //     printf("layer nb %d : layer->outputs[%d]
        //     layer->is_output_layer=%d output=%f\n", layer->layer_size, i,
        //     layer->is_output_layer, output);
        // }
        if (isnanf(layer->z[i]) || isinff(layer->z[i]) ||
            layer->z[i] < -1.0e5 || layer->z[i] > 1.0e5)
        {

            errx(EXIT_FAILURE, "layer nb %d : output=%flayer->z[i]=%f",
                 layer->layer_size, output, layer->z[i]);
        }
    }
    if (layer->is_output_layer)
    {
        softmax_activation_function(layer, max);
    }
    else
    {
        ReLU_activation_function(layer);
    }
}

void free_layers(struct layer *layer)
{
    if (layer->prev_layer != NULL)
    {
        free_layers(layer->prev_layer);
    }
    free((layer->biases));
    free(layer->outputs);
    free(layer->z);
    for (int i = 0; i < layer->layer_size; i++)
    {
        free(layer->weights[i]);
    }
    free(layer->weights);
    free(layer);
}

void check_layer(const struct layer *layer)
{
    layer_calculate_output(layer);
    printf("---------- layer nb: %d ----------\n", layer->layer_size);

    if (layer->prev_layer != NULL)
    {
        printf("input connected: %d\n",
               (*layer->inputs) == (layer->prev_layer->outputs));
    }

    printf("input is input of nn: %d\n", *layer->inputs[0] == 69);

    printf("output is output of nn: %d\n\n",
           layer->outputs[0] == 69 && layer->is_output_layer);

    if (layer->prev_layer != NULL)
    {
        check_layer(layer->prev_layer);
    }
}

void print_values(const struct layer *layer)
{
    printf("print layer %d\n", layer->layer_size);
    if (layer->prev_layer != NULL)
    {
        print_values(layer->prev_layer);
    }

    printf("########################################\n");
    for (int i = 0; i < layer->layer_size; ++i)
    {
        printf("output: %f\n", layer->outputs[i]);
        printf("bias: %f\nweights{\n", layer->biases[i]);
        for (int j = 0; j < layer->prev_layer_size; ++j)
        {
            printf("%f,\n", layer->weights[i][j]);
        }
        printf("}\n\n");
    }

    printf("end print layer %d\n", layer->layer_size);
}
