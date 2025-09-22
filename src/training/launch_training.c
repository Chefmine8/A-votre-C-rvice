// #inlcude "imageToBitmap.h"
#include <stdio.h>
#include "dirent.h"
#include <string.h>
int main()
{
    struct dirent *letter;
    DIR *dir;
    FILE *file;
    if((dir = opendir("../../resources/dataset/data/testing_data")) != NULL )
    {
        while( (letter = readdir(dir)) != NULL ){
            printf("%s\n", letter->d_name);
            while( (file = readdir( strcat(dir, letter))) != NULL )
            {
                printf("%s\n", file->d_name);
            }
        }
    }else
    {
        printf("nop");
    }

    return 0;
}
