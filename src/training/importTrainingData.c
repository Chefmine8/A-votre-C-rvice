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

int single_train_cession(const struct neural_network *neural_network, long double learning_rate) {
    char* path = "../../resources/dataset/data/training_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;


    int returned_value = 0;
    while ((entry = readdir(directory)) != NULL && returned_value == 0)
    {
        char *letter_char = entry->d_name;

        if ( strcmp(letter_char, "..") != 0 && strcmp(letter_char, ".") != 0 && strlen(letter_char) != 0)
        {
            char path_w_letter[45];
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path, letter_char);
            DIR *letter = opendir( path_w_letter );


            time_t t_letter = time(NULL);
            while ((entry2 = readdir(letter)) != NULL && returned_value == 0)
            {
                char *file_name = entry2->d_name;
                if (strcmp(file_name, "..") != 0 && strcmp(file_name, ".") != 0  && strlen(file_name) != 0) {
                    char path_to_file[257];
                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s", path_w_letter, "/", file_name);
                    Image *img = load_image(path_to_file);
                    for(int i = 0; i < img->width; i++) {
                        neural_network->inputs[i] = img->pixels[0][i].r / 255.0;
                    }


                    returned_value = backprop_update(neural_network, letter_char[0], learning_rate);




                    free_image(img);

                }

            }

			closedir(letter);
        }
    }
	closedir(directory);

    return returned_value;
}

long double test_neural_network(struct neural_network *neural_network) {
    char* path = "../../resources/dataset/data/testing_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;
    long double total_success = 0;
    printf("Testing {\n");
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

                    for(int i = 0; i < img->width; i++) {
                        neural_network->inputs[i] = img->pixels[0][i].r / 255.0;
                    }

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
    printf("total_success=%Lf, total_tests=%Lf\n", total_success, 728.0L);
    return total_success;
}

int main()
{
    long double max_success = 0.0;
    for(int layer_1 = 28; layer_1 < 28*28; layer_1++)
    {
        for(int layer_2 = 26; layer_2 < 28*26; layer_2++)
        {
            for(long double learning_rate = 0.001; learning_rate < 1.0; learning_rate += 0.001)
            {
                for(long double learning_rate_decay = 0.999; learning_rate_decay > 0.0; learning_rate_decay -= 0.001)
                {
                    for(int nb_sessions = 1; nb_sessions < 50; nb_sessions++)
                    {
                        int arr[] = {28*28, layer_1, layer_2, 26};
                        struct neural_network *neural_network = create_neural_network(4, arr);
                        long double current_learning_rate = learning_rate;
                        int returned_value = 0;
                        for(int i = 0; i < nb_sessions && returned_value == 0; i++)
                        {
                            returned_value = single_train_cession(neural_network, current_learning_rate);
                            current_learning_rate *= learning_rate_decay;
                        }
                        if(returned_value == 0)
                        {
                            long double success = test_neural_network(neural_network);
                            if(success > max_success)
                            {
                                max_success = success;
                                export_neural_network(neural_network, learning_rate, learning_rate_decay, nb_sessions, success / 728.0L);
                                printf("New best model: layer1=%d, layer2=%d, learning_rate=%Lf, learning_rate_decay=%Lf, nb_sessions=%d, nb of success= %Lf / 728 = %Lf%\n", layer_1, layer_2, learning_rate, learning_rate_decay, nb_sessions, success, success / 728.0L * 100.0L);
                            }
                        }
                        free_neural_network(neural_network);
                    }
                }
            }
        }
    }
}