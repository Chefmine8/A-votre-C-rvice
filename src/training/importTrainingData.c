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
    printf("stc 1\n");
    while ((entry = readdir(directory)) != NULL)
    {
        if ( strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0)
        {
            printf("stc 2\n");
            char path_w_letter[45];
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path, entry->d_name);
            printf("stc 3\n");
            DIR *letter = opendir( path_w_letter );
            while ((entry2 = readdir(letter)) != NULL)
            {
                if (strcmp(entry2->d_name, "..") != 0 && strcmp(entry2->d_name, ".") != 0 ) {
                    printf("stc 4\n");
                    char path_to_file[257];
                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s", path_w_letter, "/", entry2->d_name);
                    printf("stc 5\n");
                    Image *img = load_image(path_to_file);
                    printf("stc 6\n");
                    get_nn_input(img, neural_network->inputs);
                    printf("stc 7\n");
                    minimise_loss(neural_network, entry->d_name[0], shift);
                    printf("stc 8\n");
                    sum_loss += categorical_cross_entropy(neural_network, entry->d_name[0]);
                    sum += 1;
                    printf("stc 9\n");
                    free_image(img);
                    printf("stc 10\n");
                }

            }
        }

    }

    printf("%Lg\n", sum_loss / sum);

}

int main()
{
    long double shift = 1.0;
    int arr[] = {28*28, 26, 26};
    printf("1\n");
    struct neural_network *neural_network = create_neural_network(3, arr);
    printf("2\n");

    time_t t = time(NULL);
    for (int i = 0; i < 3; ++i) {
        time_t tt = time(NULL);
        printf("3\n");
        single_train_cession(neural_network, shift);
        printf("4\n");
        shift /= 1.01;

        printf("Time to single cession : %lds\n", tt - time(NULL));
    }
    printf("Time to train : %lds\n", t - time(NULL));
    free_neural_network(neural_network);
}