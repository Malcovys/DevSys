#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdlib.h>

int main(int argc, char**argv) {
    DIR *diretory; /* flux du repertoire */
    struct dirent *entry;
    int i;

    for (i=1 ; i<argc ; i++)
    {
        diretory = opendir(argv[i]); /* ouverture du répertoire */
        
        if (diretory==NULL) {
            fprintf(stderr, "Erreur d'ouverture du réperoire %s\(\backslash\)n",argv[i]);
            fprintf(stderr, "Droits inssufisant ou répertoire incorrect\n");
            exit(1);
        }

        printf("Répertoire %s\n", argv[i]);

        while ((entry=readdir(diretory)) != NULL) /* on parcourt la liste d'élements dans le répertoire*/
        {
            printf("%s \n", entry->d_name);
        }
    }

    return 0;
}