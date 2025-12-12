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
    const char* partial_path = "../../resources/pre_process/output/grid/cells/";
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
                closedir(dir);
                return entry->d_name;
            }
        }
    }
    closedir(dir);
    printf("Folder not found\n");
    return "NOTHING";
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
void create_grid(char* id)
{
    char* partial_path = "../../resources/pre_process/output/grid/cells/";
    char* dir = get_dir_data(id);
    char full_path[1024];
    full_path[0] = 0;

    strcat(full_path,partial_path);
    strcat(full_path,dir);
    strcat(full_path,"/");
    
    printf("Chemin complet : %s\n",full_path);

    int row = get_row(dir);
    int column = get_column(dir);
    printf("Nbr de lignes / colonnes : %d/%d\n",row,column);

    FILE* fichier = fopen(id,"w");
    if(fichier == NULL)
    {
        errx(1,"ouvertur du fichier");
    }

    char* letter = malloc(sizeof(char)*50);

    for(int i = 0;i<10;i++)
    {
        for(int j =0;j<15;j++)
        {
            sprintf(letter,"cell_%d_%d.bmp",j,i);
            printf("Recherche du fichier : %s\n",letter);
            //Image* image = load_image(letter);
            //recup la lettre
            letter[0] = 0;
        }
        fprintf(fichier,"\n");
    }

    free(letter);
    fclose(fichier);



    
}


