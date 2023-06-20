#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ftw.h>

static int sum_size = 0;

int print_file_info(const char *fpath, const struct stat *sb, int typeflag) {
    if (!S_ISDIR(sb->st_mode)) { // jeśli to nie jest katalog
        // wypisz informacje o pliku
        printf("%-20s %lld bytes\n", fpath, (long long) sb->st_size); // wypisz rozmiar i nazwę pliku
        sum_size += sb->st_size; // dodaj rozmiar do sumy
    }
    return 0; // kontynuuj przetwarzanie
}

int main(int argc, char *argv[]) {
    if (argc != 2) { // program wymaga podania dokładnie jednego argumentu (ścieżki do katalogu)
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        exit(0);
    }

    // przetwarzaj katalog
    if (ftw(argv[1], print_file_info, 16) == -1) { // przetwarzaj katalog
        perror("ftw");
        exit(0);
    }
     // wypisz sumę rozmiarów
    printf("\nSumaryczny rozmiar plików: %d bytes\n", sum_size);
    exit(EXIT_SUCCESS);
}
