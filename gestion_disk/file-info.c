#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char**argv) {

    struct stat st; /* pour récupérer les informations sur un fichier */
    struct tm *temps; /* pour traduire les dates (voir ctime(3)) */

    if (argc != 2) {
        fprintf(stderr, "Usage : %s nom_de_fichier\n", argv[0]);
        exit(1);
    }

    if (stat(argv[1], &st)!= 0) {
        perror("Erreur d'accès au fichier\n");
        exit(1);
    }

    if (S_ISDIR(st.st_mode)) {
        printf("Le nom %s correspond à un répertoire\n", argv[1]);
    }

    if (S_ISREG(st.st_mode)) {
        printf("%s est un fichier ordinaire\n", argv[1]);
        printf("La taille du fichier en octets est %ld\n", st.st_size);
        temps = localtime(&st.st_mtime);
        printf("Le jour de dernière mofification est %d/%d/%d\n",
        temps->tm_mday, temps->tm_mon+1, temps->tm_year+1900);
    }

    return 0;

}