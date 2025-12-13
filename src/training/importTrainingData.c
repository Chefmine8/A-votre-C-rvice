#include "importTrainingData.h"
#include "../core/image.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "../neural_network/neural_network.h"
#include "../pre_process/scale.h"
#include <time.h>
#include "../neural_network/layer.h"
#include "err.h"
#include <setjmp.h>

Image *** DATASET;

void load_dataset()
{
    char* path = "../../resources/dataset/data/training_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;

    DATASET = malloc(26 * sizeof(Image**));
    if(DATASET == NULL){
        errx(EXIT_FAILURE, "malloc failed");
    }
    int row = 0;
    while ((entry = readdir(directory)) != NULL)
    {
        char *letter_char = entry->d_name;

        if ( strcmp(letter_char, "..") != 0 && strcmp(letter_char, ".") != 0 && strlen(letter_char) != 0)
        {
            char path_w_letter[45];

            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path, letter_char);
            DIR *letter = opendir( path_w_letter );
            int column = 0;

            DATASET[row] = malloc(570 * sizeof(Image*));

            if(DATASET[row] == NULL){
                errx(EXIT_FAILURE, "malloc failed");
            }
            while ((entry2 = readdir(letter)) != NULL)
            {
                char *file_name = entry2->d_name;
                if (strcmp(file_name, "..") != 0 && strcmp(file_name, ".") != 0  && strlen(file_name) != 0) {
                    char path_to_file[257];

                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s", path_w_letter, "/", file_name);

                    Image *img = load_image(path_to_file);

                    DATASET[row][column] = img;

                    column++;
                }

            }
            closedir(letter);
            row++;
        }
    }

    closedir(directory);
}

void free_dataset()
{
    for(int i = 0; i < 26; i++)
    {
        for(int j = 0; j < 27; j++)
        {
            free_image(DATASET[i][j]);
        }
        free(DATASET[i]);
    }
    free(DATASET);
}

Image* get_image(char *expected_output)
{
    int row = rand() % 26;
    *expected_output = 'A' + row;
    int col = rand() % 570;
    // printf("Getting image %d/%d\n", row, col);
    return DATASET[row][col];
}


void zero_vector(double *v, int size)
{
    for (int i = 0; i < size; i++)
        v[i] = 0.0;
}

void zero_matrix(double **m, int rows, int cols)
{
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            m[i][j] = 0.0;
}



int test_neural_network(struct neural_network *neural_network) {
    char* path = "../../resources/dataset/data/testing_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;

    int total_success = 0;
    while ((entry = readdir(directory)) != NULL)
    {
        char *letter_char = entry->d_name;
        // strcpy(letter_char, entry->d_name);
        if ( strcmp(letter_char, "..") != 0 && strcmp(letter_char, ".") != 0 && strlen(letter_char) != 0)
        {
            char path_w_letter[45];
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path, letter_char);
            DIR *letter = opendir( path_w_letter );
            if (letter == NULL) errx(EXIT_FAILURE, "letter is null\n");

            while ((entry2 = readdir(letter)) != NULL)
            {
                char *file_name = entry2->d_name;
                if (strcmp(file_name, "..") != 0 && strcmp(file_name, ".") != 0  && strlen(file_name) != 0) {
                    char path_to_file[257];
                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s", path_w_letter, "/", file_name);
                    Image *img = load_image(path_to_file);

                    get_nn_input(img, neural_network->inputs);
                    for(int i = 0; i < neural_network->input_size; i++){
                        if(isnanf(neural_network->inputs[i])){
                            errx(EXIT_FAILURE, "Input is nan for image %s\n", path_to_file);
                        }
                    }


                    neural_network_calculate_output(neural_network);
                    printf("\n");
                    char output = get_neural_network_output(neural_network);
                    printf("Expected: %s, Got: %c\n", letter_char, output);
                    total_success += output == letter_char[0];



                    free_image(img);

                }

            }

			closedir(letter);
        }
        // free(letter_char);

    }


	closedir(directory);

    // if(isnan(total_success)){
    //     errx(EXIT_FAILURE, "total_success is nan\n");
    //}
    printf("\t\ttotal_success=%d, %f%\n", total_success, ((float)total_success) / (28*26) * 100.0);
    return total_success;
}
void train_nn(
    struct neural_network *nn,
    int epochs,
    double learning_rate
)
{

    double *dZ2 = malloc(sizeof(double) * 26);
    double *db2 = malloc(sizeof(double) * 26);
    double **dW2 = malloc(sizeof(double*) * 26);
    for (int i = 0; i < 26; i++)
        dW2[i] = malloc(sizeof(double) * 128);

    double *dA1 = malloc(sizeof(double) * 128);
    double *dZ1 = malloc(sizeof(double) * 128);
    double *db1 = malloc(sizeof(double) * 128);
    double **dW1 = malloc(sizeof(double*) * 128);
    for (int i = 0; i < 128; i++)
        dW1[i] = malloc(sizeof(double) * 784);

    double *dA0 = malloc(sizeof(double) * 784);
    printf("fini de malloc\n");
    for (int e = 0; e < epochs; e++) 
    {
        double epoch_loss = 0.0;
        for(int i = 0; i < 26; i++) 
        {
            for(int j = 0; j < 570; j++) 
            {
                char expected = i + 'A';
                printf("\n\nTry : %d with %c\n\n",i*570+j,expected);
                Image *img = DATASET[i][j];
                if(img != NULL)
                {
                    get_nn_input(img, nn->inputs);
                    printf("image loaded\n");
                }
                
                printf("feed forward\n");
                get_neural_network_output(nn);

                printf("calculate loss\n");
                epoch_loss += calculate_loss(nn, expected);

                zero_vector(db2, 26);
                zero_matrix(dW2, 26, 128);
                zero_vector(db1, 128);
                zero_matrix(dW1, 128, 784);

                printf("softmax\n");
                softmax_cross_entropy_backward(nn, expected, dZ2);

                printf("linear output\n");
                linear_backward_output(nn, dZ2, dW2, db2, dA1);

    
                printf("Relu\n");
                relu_backward(
                    dA1,
                    nn->layers[0]->outputs,
                    dZ1,
                    128
                );
                
                printf("linear hidden\n");
                linear_backward_hidden(nn, dZ1, dW1, db1, dA0);


                printf("update output\n");
                update_layer(
                    nn->layers[1]->weights,
                    nn->layers[1]->biases,
                    dW2,
                    db2,
                    26,
                    128,
                    learning_rate
                );

                printf("update hidden\n");
                update_layer(
                    nn->layers[0]->weights,
                    nn->layers[0]->biases,
                    dW1,
                    db1,
                    128,
                    784,
                    learning_rate
                );
            }
        }
        

        printf("Epoch %d | loss = %f\n", e, epoch_loss / (570*26));
    }


    free(dZ2); free(db2);
    for (int i = 0; i < 26; i++) free(dW2[i]);
    free(dW2);

    free(dA1); free(dZ1); free(db1); free(dA0);
    for (int i = 0; i < 128; i++) free(dW1[i]);
    free(dW1);
}

int main()
{
    load_dataset();
    int nb_sessions = 100;
    int batch_size  = 1;

    float learning_rate = 0.1;

    int arr[] = {28*28, 128, 26};
    struct neural_network *neural_network = create_neural_network(3, arr);
    int success = 0;
    int returned_value = 0;

    int total_success = 0;
    train_nn(neural_network,nb_sessions,learning_rate);
    for(int i = 0; i < nb_sessions && returned_value == 0; i++) {
        printf("i'm in\n");

        //returned_value = all_dataset_train_session(neural_network, learning_rate); //, batch_size);
        success = test_neural_network(neural_network);
        total_success += success;
        printf(" Average nb of success : %f\n", ((float)total_success) / ((float)i + 1));
        // learning_rate *= 0.999;
    }

    export_neural_network(neural_network, learning_rate, batch_size, nb_sessions, success);

    free_neural_network(neural_network);
    free_dataset();
    printf("\n\nfinish\n\n");
}
