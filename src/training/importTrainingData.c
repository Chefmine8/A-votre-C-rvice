#include "../pre_process/image.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>

void import()
{
    // Image *img = load_image(path);

    DIR *directory = opendir("../../resources/dataset/data/training_data/");
    struct dirent *entry;
    struct dirent *entry2;

    while ((entry = readdir(directory)) != NULL)
    {
        printf("%s\n", entry->d_name);

        if ( strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0)
        {
            printf("aaaaa");
            char* path = strcat( "../../resources/dataset/data/training_data/", entry->d_name);
            printf("bbbbb");
            DIR *letter = opendir( path );
            printf("%s\n", path);
            while ((entry2 = readdir(letter)) != NULL)
            {
                printf("%s\n", entry2->d_name);
            }
        }

    }

}

int main()
{
    import();
}