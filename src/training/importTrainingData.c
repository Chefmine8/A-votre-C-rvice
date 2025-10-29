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

void single_train_cession(const struct neural_network *neural_network, long double shift) {
    char* path = "../../resources/dataset/data/training_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;
    long double sum_loss = 0;
    long double sum = 0;
    printf("1\n");
    while ((entry = readdir(directory)) != NULL)
    {
        char *letter_char = entry->d_name;
        // strcpy(letter_char, entry->d_name);
        if ( strcmp(letter_char, "..") != 0 && strcmp(letter_char, ".") != 0 && strlen(letter_char) != 0)
        {
            char path_w_letter[45];
            printf("2\n");
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path, letter_char);
            printf("3\n");
            DIR *letter = opendir( path_w_letter );
            if (letter == NULL) errx(EXIT_FAILURE, "letter is null\n");
            while ((entry2 = readdir(letter)) != NULL)
            {
                char *file_name = entry2->d_name;
                if (strcmp(file_name, "..") != 0 && strcmp(file_name, ".") != 0  && strlen(file_name) != 0) {
                    char path_to_file[257];
                    printf("4\n");
                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s", path_w_letter, "/", file_name);
                    printf("5\n");
                    Image *img = load_image(path_to_file);
                    printf("6\n");
                    get_nn_input(img, neural_network->inputs);
                    printf("7\n");
                    minimise_loss(neural_network, letter_char[0], shift);
                    printf("8\n");
                    sum_loss += categorical_cross_entropy(neural_network, letter_char[0]);
                    printf("9\n");
                    sum += 1;
                    printf("10\n");
                    free_image(img);
                    printf("11\n");
                }

            }
        }
        // free(letter_char);

        printf("%Lg\n", sum_loss / sum);

    }
}
int main()
{
    long double shift = 1.0;
    int arr[] = {2, 4, 2};
    struct neural_network *neural_network = create_neural_network(3, arr);
    check_neural_network(neural_network);
    print_values(neural_network->layers[neural_network->number_of_layers -1]);
    time_t t = time(NULL);
    for (int i = 0; i < 3; ++i) {
        time_t tt = time(NULL);
        single_train_cession(neural_network, shift);
        shift /= 1.01;

        printf("Time to single cession : %lds\n", tt - time(NULL));
    }
    printf("Time to train : %lds\n", t - time(NULL));
    // free_neural_network(neural_network);
}