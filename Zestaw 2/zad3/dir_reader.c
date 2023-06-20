#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>


int main(){
    // otwarcie katalogu
    DIR *dir = opendir(".");
    // sprawdzenie czy sie udalo
    if (dir == NULL) {
        printf("Error opening directory");
        exit(0);
    }
    // zmienna do przechowywania rozmiaru
    long long size = 0;

    // struktury do przechowywania informacji o pliku
    struct dirent *file;
    struct stat infromation;
    // czytanie plikow z katalogu
    while ((file = readdir(dir))!= NULL){
        if (stat(file->d_name, &infromation) !=0 ){
            printf("Error while reading file informations");
            exit(0);
        }
        if (!S_ISDIR(infromation.st_mode)){
            size += infromation.st_size;
            printf("[%s]: %lld\n", file->d_name, (long long)infromation.st_size);
        }

    }

    // wypisanie rozmiaru
    printf("Total size: %lld\n", size);
    closedir(dir);
    return 0;
}