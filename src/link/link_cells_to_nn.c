
#include "link_cells_to_nn.h"

#include "../neural_network/neural_network.h"
#include "../core/image.h"
#include "../pre_process/scale.h"

/*
il faut une fonction qui prend en paramétre le lien où ce trouve les images des lettres, 
tu parcours toutes les images et tu les transmets au réseaux puis tu prend le proba 
la pluis élevé du reseau et tu le met dans un fichier txt qui represente la grille.

*/


/*

*/
char* get_dir_data(char* identifiant)
{
    const char* partial_path = "../ui/output/";
    char* id = identifiant;
    DIR *dir;
    struct dirent *entry;

    dir = opendir(partial_path);

    if(!dir)
    {
        errx(42,"dir");
        return NULL;
    }

    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_type == DT_DIR)
        {
            if(strncmp(entry->d_name,id,strlen(id)) ==0)
            {
                printf("Folder found : %s\n",entry->d_name);
                char* temp = malloc(sizeof(char)*256);
                sprintf(temp,"%s",entry->d_name);
                closedir(dir);
                return temp;
            }
        }
    }
    closedir(dir);
    printf("Folder not found\n");
    return NULL;
}

int get_row(char* str)
{
    size_t i = 0;
    while(str[i] != '_')
        i++;
    i++;
    size_t j = 0;
    char buf[20];
    while(str[i]!='_')
    {
        buf[j] = str[i];
        i++;
        j++;
    }
    buf[j] = 0;
    return atoi(buf);
}

int get_column(char* str)
{
    size_t i = 0;
    while(str[i] != '_')
        i++;
    i++;
    size_t j = 0;
    char buf[20];
    while(str[i]!='_')
    {
        i++;
    }
    i++;
    while(str[i] != 0)
    {
        buf[j] = str[i];
        i++;
        j++;
    }
    buf[j] = 0;
    return atoi(buf);
}
void create_grid(char* id,struct neural_network* neural_net)
{
    printf("\n\nGrid correction\n\n");
    // recup du chemin
    char* partial_path = "../ui/output/";
    char* dir = get_dir_data(id);
    char* full_path = malloc(sizeof(char) * 300);   
    sprintf(full_path,"%s%s/",partial_path,dir);
    
    printf("Chemin complet : %s\n",full_path);

    // recup des lignes
    int row = get_row(dir);
    int column = get_column(dir);
    printf("Nbr de lignes / colonnes : %d/%d\n",row,column);

    FILE* fichier = fopen(id,"w");
    if(fichier == NULL)
    {
        errx(1,"ouvertur du fichier");
    }

    char* letter = malloc(sizeof(char)*500);
    long double* input_nn = malloc(sizeof(long double)*784);

    Image* image;
    for(int i = 1;i<=row;i++) // row
    {
        for(int j =1;j<=column;j++) // col
        {
            sprintf(letter,"%scell_%d_%d.png",full_path,j,i); // reversed for the Q
            printf("Recherche du fichier : %s\n",letter);
            image = load_image(letter);
            printf("image loaded\n");
            get_nn_input(image,neural_net->inputs);
            printf("nn input get\n");
            char letter_output = get_neural_network_output(neural_net);
            printf("lettre recuperer : %c\n",letter_output);

            //recup la lettre
            //print la lettre dans le fichier
            letter[0] = 0;
            fprintf(fichier,"%c",letter_output);
        }
        fprintf(fichier,"\n");
    }
    free(image);
    free(dir);
    free(full_path);
    free(input_nn);
    free(letter);


    fclose(fichier);

}

char* get_word_letter(int n_word,int n_letter)
{
    const char* partial_path = "../ui/output/word_list";
    DIR *dir;
    struct dirent *entry;
    char image_search[512];
    sprintf(image_search,"word_%d_letter_%d.png",n_word,n_letter);
    dir = opendir(partial_path);

    if(!dir)
    {
        errx(42,"dir don't open");
        return NULL;
    }

    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_type == DT_REG)
        {
            if(strcmp(entry->d_name,image_search) == 0)
            {
                printf("Lettre trouve : %s\n",entry->d_name);
                char* temp = malloc(sizeof(char)*256);
                sprintf(temp,"%s",entry->d_name);
                closedir(dir);
                return temp;
            }
        }
    }
    closedir(dir);
    printf("Letter %d_%d not found\n\n",n_word,n_letter);
    return NULL;
}

void create_word_list(char* id,struct neural_network* neural_net)
{
    printf("\n\nCreation de la liste de mots\n\n");
    char* partial_path = "../ui/output/";
    char* dir = id;
    char* full_path = malloc(sizeof(char) * 300);   
    sprintf(full_path,"%s%s/",partial_path,dir);
    
    printf("Chemin complet : %s\n",full_path);

    FILE* fichier = fopen("word_list_file","w");
    if(fichier == NULL)
    {
        errx(1,"ouvertur du fichier");
    }
    int i = 1;
    int j = 1;
    int ok = 1; // 1 if word left / 0 if we are at the end of the word / -1 otherwise

    char* letter;
    char* full_path_letter = malloc(sizeof(char)*512);

    Image* image;

    while(ok >= 0)
    {
        while(ok == 1)
        {
            letter = get_word_letter(i,j);
            if(letter == NULL)
            {
                if(j == 1)
                    ok = -1;
                else
                    ok = 0;
            }
            else
            {
                full_path_letter[0] = 0;
                sprintf(full_path_letter,"%s%s",full_path,letter);
                printf("Recherche de la lettre : %s\n",letter);
                image = load_image(full_path_letter);
                printf("Letter loaded\n");
                get_nn_input(image,neural_net->inputs);
                printf("nn input get\n");
                char letter_output = get_neural_network_output(neural_net);
                printf("lettre recuperer : %c\n\n",letter_output);
                j += 1;
                fprintf(fichier,"%c",letter_output);
            }
        }
        if(ok == 0)
        {
            ok = 1;
            j = 1;
            i += 1;
            fprintf(fichier,"\n");
        }
    }
    free(full_path_letter);
    free(letter);
    free(image);

    fclose(fichier);

}
