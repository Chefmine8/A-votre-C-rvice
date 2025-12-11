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
    return DATASET[row][rand() % 27];
}

int single_train_cession(const struct neural_network *neural_network, float learning_rate, int batch_size) {
    printf("Training cession with learning_rate=%f, batch_size=%d\n", learning_rate, batch_size);
    int returned_value = 0;
    for(int b = 0; b < batch_size && returned_value == 0; b++) {
        char expected_output;
        Image *img = get_image(&expected_output);

        get_nn_input(img, neural_network->inputs);

        returned_value = backprop_update(neural_network, expected_output, learning_rate);
    }
    printf("\t\tcession over\n");
    return returned_value;
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

                    neural_network_calculate_output(neural_network);

                    char output = get_neural_network_output(neural_network);
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
    printf("\t\ttotal_success=%d, %f%\n", total_success, ((float)total_success) / 728.0 * 100.0);
    return total_success;
}

int main()
{
    load_dataset();
    int max_success = 0;
    for(float learning_rate = 0.001; learning_rate < 1.0; learning_rate += 0.001){
        for(float batch_size = 5; batch_size < 100; batch_size++){
            for(int layer_1 = 40; layer_1 < 100; layer_1++){
                for(int layer_2 = 40; layer_2 < 100; layer_2++){
                    for(int nb_sessions = 100; nb_sessions < 1000; nb_sessions+= 100){

                        int arr[] = {28*28, layer_1, layer_2, 26};
                        struct neural_network *neural_network = create_neural_network(4, arr);
                        int returned_value = 0;
                        for(int i = 0; i < nb_sessions && returned_value == 0; i++)
                        {
                            returned_value = single_train_cession(neural_network, learning_rate, batch_size);
                        }
                        if(returned_value == 0)
                        {
                            int success = test_neural_network(neural_network);
                            if(success > max_success)
                            {
                                max_success = success;
                                export_neural_network(neural_network, learning_rate, batch_size, nb_sessions, success);
                                printf("New model: layer1=%d, layer2=%d, learning_rate=%f, batch_size=%f, nb_sessions=%d, nb of success= %f / 728 = %f%\n", layer_1, layer_2, learning_rate, batch_size, nb_sessions, success, success / 728.0 * 100.0);
                            }
                        }
                        free_neural_network(neural_network);
                    }
                }
            }
        }
    }





    free_dataset();

    /*
    float max_success = 0.0;
    for(int layer_1 = 5; layer_1 < 100; layer_1++)
    {
        for(int layer_2 = 5; layer_2 < 100; layer_2++)
        {
            for(float learning_rate = 0.001; learning_rate < 1.0; learning_rate += 0.001)
            {
                for(float learning_rate_decay = 0.999; learning_rate_decay > 0.0; learning_rate_decay -= 0.001)
                {
                    for(int nb_sessions = 1; nb_sessions < 15; nb_sessions++)
                    {
                        int arr[] = {28*28, layer_1, layer_2, 26};
                        struct neural_network *neural_network = create_neural_network(4, arr);
                        float current_learning_rate = learning_rate;
                        int returned_value = 0;
                        for(int i = 0; i < nb_sessions && returned_value == 0; i++)
                        {
                            returned_value = single_train_cession(neural_network, current_learning_rate);
                            current_learning_rate *= learning_rate_decay;
                        }
                        if(returned_value == 0)
                        {
                            float success = test_neural_network(neural_network);
                            if(success > max_success)
                            {
                                max_success = success;
                                export_neural_network(neural_network, learning_rate, learning_rate_decay, nb_sessions, success / 728.0L);
                                printf("New best model: layer1=%d, layer2=%d, learning_rate=%f, learning_rate_decay=%f, nb_sessions=%d, nb of success= %f / 728 = %f%\n", layer_1, layer_2, learning_rate, learning_rate_decay, nb_sessions, success, success / 728.0 * 100.0);
                            }
                        }
                        free_neural_network(neural_network);
                    }
                }
            }
        }
    }
    */
}