#include "link_cells_to_nn.h"

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


