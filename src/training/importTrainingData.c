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

void single_train_cession(const struct neural_network *neural_network, long double learning_rate) {
    char* path = "../../resources/dataset/data/training_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;
    long double sum_loss = 0;
    long double sum = 0;
    while ((entry = readdir(directory)) != NULL)
    {
        char *letter_char = entry->d_name;
        // strcpy(letter_char, entry->d_name);
        if ( strcmp(letter_char, "..") != 0 && strcmp(letter_char, ".") != 0 && strlen(letter_char) != 0)
        {
            char path_w_letter[45];
            snprintf(path_w_letter, sizeof(path_w_letter), "%s%s", path, letter_char);
            DIR *letter = opendir( path_w_letter );

            printf("\tTraining %s {\n", letter_char);
            time_t t_letter = time(NULL);
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
                    // get_nn_input(img, neural_network->inputs);
                    //printf("\t%c\n", letter_char[0]);
                    //time_t t = time(NULL);
                    backprop_update(neural_network, letter_char[0], learning_rate);
                    // minimise_loss(neural_network, letter_char[0], shift, epsilon);
                    // printf("\t%ld sec\n", time(NULL) - t);
                    sum_loss += categorical_cross_entropy(neural_network, letter_char[0]);

                    sum += 1;
                    free_image(img);

                }

            }
            printf("\t %ld sec }\n", time(NULL) - t_letter);
			closedir(letter);
        }
    }
	closedir(directory);
}

void test_neural_network(struct neural_network *neural_network) {
    char* path = "../../resources/dataset/data/testing_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;
    long double total_success = 0;
    long double total_tests = 0;
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
            printf("\tTesting %s {\n", letter_char);
            int success = 0;
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
                    success += output == letter_char[0];
                    total_success += 1;
                    printf("\t\t%c %Lf%\n", output, categorical_cross_entropy(neural_network, letter_char[0]));



                    free_image(img);

                }

            }
            printf("\tSuccess Number : %d }\n", success);
			closedir(letter);
        }
        // free(letter_char);

    }
    printf("Total Success Rate: %Lf% }\n", (total_success*100.0) / total_tests);

	closedir(directory);

}

int main()
{
    long double learning_rate = 0.2;
    int arr[] = {28*28, 40, 30, 26};
    printf("##############################\n               Create NN\n##############################\n");
    struct neural_network *neural_network = create_neural_network(4, arr);
    printf("##############################\n               Train NN\n##############################\n");
    for(int i = 0; i < 20; i++){

        single_train_cession(neural_network, learning_rate);
        learning_rate *= 0.9;
       }
    printf("##############################\n               Test NN\n##############################\n");
    test_neural_network(neural_network);

    printf("##############################\n               export NN\n##############################\n");

    export_neural_network(neural_network);

    printf("##############################\n               Free NN\n##############################\n");
     free_neural_network(neural_network);
    printf("##############################\n               END NN\n##############################\n");
}