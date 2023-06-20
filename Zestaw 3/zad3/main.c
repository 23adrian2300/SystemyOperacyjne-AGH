#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>

// funkcja wyszukująca ciąg znaków w pliku
void search_in_file(char *file_path, char *searching_string) {
    FILE *file;
    size_t length = 0;
    char *line = NULL;
    // otwieranie pliku
    if ((file = fopen(file_path, "r")) == NULL) {
        perror("open");
        exit(1);
    }
    // czytanie linii
    getline(&line, &length, file);
    // porównywanie linii z szukanym ciągiem
    if (strncmp(line, searching_string, strlen(searching_string)) == 0) {
        // wypisanie PID i ścieżki do pliku
        printf("PID: %d, Path: %s\n", getpid(), realpath(file_path, NULL));
    }
    free(line);
    fclose(file);
}


void searcher(char *path, char *searching_string) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }
    struct dirent *entry;
    struct stat file_stat;
    char file_path[PATH_MAX];
    pid_t pid;
    while ((entry= readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
            continue;
        }
        snprintf(file_path, PATH_MAX, "%s/%s", path, entry->d_name);
        if (lstat(file_path, &file_stat) == -1) {
            perror("stat");
            exit(1);
        }
        if (S_ISDIR(file_stat.st_mode)) {
            pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(1);
            } else if (pid == 0) {
                searcher(file_path, searching_string);
                exit(0);
            }
        } else {
            search_in_file(file_path, searching_string);
        }
    }
    closedir(dir);
    while (wait(NULL) > 0);
}


int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Wrong number of arguments");
        return 1;
    }
    char *path = argv[1];
    char *search_string = argv[2];
    if (strlen(search_string) > 255)
    {
        perror("strlen");
        return 1;
    }
    searcher(path, search_string);
    return 0;
}
