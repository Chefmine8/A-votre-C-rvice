#include "importTrainingData.h"
#include "../core/image.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "../neural_network/neural_network.h"
#include "../pre_process/scale.h"
#include "../pre_process/grayscale.h"
#include <time.h>
#include "../neural_network/layer.h"
#include "err.h"
#include <setjmp.h>

Image *** DATASET;
char *EXPECTED_OUTPUTS;

void load_dataset()
{
    char* path = "../../resources/dataset/data/training_data/";
    DIR *directory = opendir(path);
    struct dirent *entry;
    struct dirent *entry2;

    DATASET = malloc(26 * sizeof(Image**));
    EXPECTED_OUTPUTS = malloc(26 * sizeof(char));
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
                    if(img != NULL){
                        grayscale_image(img);
                        Image *tmp = sauvola(img, 12, 128, 0.07);
                        free_image(img);
                        img = tmp;
                        EXPECTED_OUTPUTS[row] = letter_char[0];
                    }

                    printf("%c\n", EXPECTED_OUTPUTS[row]);
                    print_image(img);

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
        for(int j = 0; j < 570; j++)
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

void print_image(Image *img) {
    if(img == NULL) {
        return;
    }
    for(int i = 0; i < img->height; i++) {
        for(int j = 0; j < img->width; j++) {
            if(img->pixels[i][j].r > 0.5f) {
                printf("#");
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

int single_train_cession( struct neural_network *neural_network, float learning_rate, int batch_size) {
    int returned_value = 0;
    for(int b = 0; b < batch_size && returned_value == 0; b++) {
        char expected_output;
        Image *img = get_image(&expected_output);
        if(img != NULL) {
            get_nn_input(img, neural_network->inputs);
            returned_value = backprop_update_4(neural_network, expected_output, learning_rate);
        }
    }
    return returned_value;
}

int all_dataset_train_session(struct neural_network *neural_network, float learning_rate) {
    int returned_value = 0;
    for(int i = 0; i < 26 && returned_value == 0; i++) {
        for(int j = 0; j < 570 && returned_value == 0; j++) {
            char expected_output = i + 'A';
            Image *img = DATASET[i][j];
            if(img != NULL){
                get_nn_input(img, neural_network->inputs);
                returned_value = backprop_update_4(neural_network, expected_output, learning_rate);
            }
        }
    }
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

            printf("Testing letter %s\n", letter_char);
            while ((entry2 = readdir(letter)) != NULL)
            {
                char *file_name = entry2->d_name;
                if (strcmp(file_name, "..") != 0 && strcmp(file_name, ".") != 0  && strlen(file_name) != 0) {
                    char path_to_file[257];
                    snprintf(path_to_file, sizeof(path_to_file), "%s%s%s", path_w_letter, "/", file_name);
                    Image *img = load_image(path_to_file);

                    print_image(img);
                    if(img != NULL){
                        get_nn_input(img, neural_network->inputs);
                    }


                    neural_network_calculate_output(neural_network);
                    // printf("\n");
                    char output = get_neural_network_output(neural_network);
                    printf("%c ", output);
                    total_success += output == letter_char[0];



                    free_image(img);

                }

            }
            printf("\n");
			closedir(letter);
        }
        // free(letter_char);

    }


	closedir(directory);

    // if(isnan(total_success)){
    //     errx(EXIT_FAILURE, "total_success is nan\n");
    //}
    // printf("\t\ttotal_success=%d, %f%\n", total_success, ((float)total_success) / (28*26) * 100.0);
    return total_success;
}

int main()
{
    //load_dataset();
    int nb_sessions = 1000;
    int batch_size  = 1;

    float learning_rate = 0.01;
    int layer_1 = 50;
    int layer_2 = 40;

    int arr[] = {28*28, 50, 40, 26};

    int success = 0;

    struct neural_network *neural_network = create_neural_network(4, arr);
    import_neural_network(neural_network, "exported_neural_network.nn");
    export_neural_network(neural_network, 0, 0, 0, 0);
    /*
    int max_success = 78;
    for(int i = 0; i < 10000; i++) {

        // single_train_cession(neural_network, learning_rate, batch_size);
        success = test_neural_network(neural_network);
        if(max_success < success) {
            max_success = success;
            export_neural_network(neural_network, 0, 0, 0, success);
        }
        free_neural_network(neural_network);
    }
    */

    //test_neural_network(neural_network);

    int returned_value = 0;

    int total_success = 0;

    for(int i = 0; i < nb_sessions && returned_value == 0; i++) {
        printf("i'm in\n");

        returned_value = all_dataset_train_session(neural_network, learning_rate); //, batch_size);
        //printf("%f%\n", (float)i /(float) nb_sessions);
        //returned_value = single_train_cession(neural_network, learning_rate, batch_size);
        //learning_rate *= 0.99;
        success = test_neural_network(neural_network);
        total_success += success;
        //printf(" Average nb of success : %f\n", ((float)total_success) / ((float)i + 1));
        // learning_rate *= 0.999;
    }

    success = test_neural_network(neural_network);
    export_neural_network(neural_network, learning_rate, batch_size, nb_sessions, success);

    free_neural_network(neural_network);

    //free_dataset();

}
