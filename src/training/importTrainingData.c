#include "../pre_process/image.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

void import(char* path)
{
    // Image *img = load_image(path);

    DIR *directory = opendir("../../resources/dataset/data/training_data/");
    struct dirent *entry;
    struct dirent *entry2;

    while ((entry = readdir(directory)) != NULL)
    {
        if (entry->d_name != ".." && entry->d_name != ".")
        {
            char* path = strcat( "../../resources/dataset/data/training_data/", entry->d_name);
            DIR *letter = opendir( path );

            while ((entry2 = readdir(letter)) != NULL)
            {
                printf("%s\n", entry2->d_name);
            }
        }
        printf("%s\n", entry->d_name);
    }

}

int main()
{
    import("");
}