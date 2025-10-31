#include "importTrainingData.h"
#include "../core/image.h"
#include "../neural_network/layer.h"
#include "../neural_network/neural_network.h"
#include "../pre_process/scale.h"
#include "err.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void single_train_cession(const struct neural_network *neural_network,
                          long double shift)
{
    char *path = "../../resources/dataset/data/training_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;
    long double sum_loss = 0;
    long double sum = 0;
    while ((entry = readdir(directory)) != NULL)
    {
        char *letter_char = entry->d_name;
        // strcpy(letter_char, entry->d_name);
        if (strcmp(letter_char, "..") != 0 && strcmp(letter_char, ".") != 0 &&
            strlen(letter_char) != 0)
        {
            char path_w_letter[45];
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path,
                     letter_char);
            DIR *letter = opendir(path_w_letter);
            if (letter == NULL)
                errx(EXIT_FAILURE, "letter is null\n");
            while ((entry2 = readdir(letter)) != NULL)
            {
                char *file_name = entry2->d_name;
                if (strcmp(file_name, "..") != 0 &&
                    strcmp(file_name, ".") != 0 && strlen(file_name) != 0)
                {
                    char path_to_file[257];
                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s",
                             path_w_letter, "/", file_name);
                    Image *img = load_image(path_to_file);
                    for (int i = 0; i < img->width; i++)
                    {
                        neural_network->inputs[i] = img->pixels[0][i].r / 255.0;
                    }
                    // get_nn_input(img, neural_network->inputs);
                    minimise_loss(neural_network, letter_char[0], shift);
                    sum_loss += categorical_cross_entropy(neural_network,
                                                          letter_char[0]);

                    sum += 1;
                    free_image(img);
                }
            }
        }
        // free(letter_char);

        // printf("%Lg\n", sum_loss / sum);
    }
}

void test_neural_network(struct neural_network *neural_network)
{
    char *path = "../../resources/dataset/data/testing_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;
    while ((entry = readdir(directory)) != NULL)
    {
        char *letter_char = entry->d_name;
        // strcpy(letter_char, entry->d_name);
        if (strcmp(letter_char, "..") != 0 && strcmp(letter_char, ".") != 0 &&
            strlen(letter_char) != 0)
        {
            char path_w_letter[45];
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path,
                     letter_char);
            DIR *letter = opendir(path_w_letter);
            if (letter == NULL)
                errx(EXIT_FAILURE, "letter is null\n");
            while ((entry2 = readdir(letter)) != NULL)
            {
                char *file_name = entry2->d_name;
                if (strcmp(file_name, "..") != 0 &&
                    strcmp(file_name, ".") != 0 && strlen(file_name) != 0)
                {
                    char path_to_file[257];
                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s",
                             path_w_letter, "/", file_name);
                    Image *img = load_image(path_to_file);
                    for (int i = 0; i < img->width; i++)
                    {
                        neural_network->inputs[i] = img->pixels[0][i].r / 255.0;
                    }
                    // get_nn_input(img, neural_network->inputs);
                    neural_network_calculate_output(neural_network);

                    char output = get_neural_network_output(neural_network);
                    printf("Expected: %c, Got: %c at %Lg%;\t other at %lg%\n",
                           letter_char[0], output,
                           (*neural_network->outputs)[letter_char[0] - 'A'] *
                               100.0,
                           (*neural_network
                                 ->outputs)[letter_char[0] - 'A' + 1 % 2] *
                               100.0);

                    free_image(img);
                }
            }
        }
        // free(letter_char);
    }
}

int main()
{
    long double shift = 1.0;
    int arr[] = {5, 2};
    struct neural_network *neural_network = create_neural_network(2, arr);
    // check_neural_network(neural_network);
    print_values(neural_network->layers[neural_network->number_of_layers - 1]);
    time_t t = time(NULL);
     for (int i = 0; i < 10000; ++i)
     {
         single_train_cession(neural_network, shift);
         shift /= 1.001;
     }

    printf("Time to train : %lds\n", time(NULL) - t);

    test_neural_network(neural_network);
    // free_neural_network(neural_network);
}