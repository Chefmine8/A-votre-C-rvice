#include "importTrainingData.h"
#include "../core/image.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "../neural_network/neural_network.h"
#include "../pre_process/scale.h"
#include <time.h>
#include "../neural_network/layer.h"
void single_train_cession(struct neural_network *neural_network, long double shift)
{
    char* path = "../../resources/dataset/data/training_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;
    long double sum_loss = 0;
    long double sum = 0;

    while ((entry = readdir(directory)) != NULL)
    {
        if ( strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0)
        {
            char path_w_letter[45];
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path, entry->d_name);

            DIR *letter = opendir( path_w_letter );
            while ((entry2 = readdir(letter)) != NULL)
            {
                if (strcmp(entry2->d_name, "..") != 0 && strcmp(entry2->d_name, ".") != 0 ) {
                    char path_to_file[50];
                    snprintf(path_to_file, sizeof(path_to_file), "%s\n", entry2->d_name);
                    Image *img = load_image(path_to_file);
                    get_nn_input(img, neural_network->inputs);

                    minimise_loss(neural_network, entry->d_name[0], shift);
                    sum_loss += categorical_cross_entropy(neural_network, entry->d_name[0]);
                    sum += 1;
                    free_image(img);
                }

            }
        }

    }

    printf("%Lg\n", sum_loss / sum);

}

int main()
{
    long double shift = 10.0;
    int arr[] = {28*28, 29*29, 30*30, 30*30, 26*26, 26};
    struct neural_network *neural_network = create_neural_network(6, arr);
    time_t t = time(NULL);
    for (int i = 0; i < 1000; ++i) {

        single_train_cession(neural_network, shift);
        shift /= 1.1;
    }
    printf("Time to train : %lds\n", t - time(NULL));
}