#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256

int cree_socket_tcp_ip() {
    int sock;
    struct sockaddr_in adresse;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Erreur socket\n");
        return -1;
    }

    memset(&adresse, 0, sizeof(struct sockaddr_in));

    adresse.sin_family = AF_INET;

    // donner un numro de port disponible quelconque
    adresse.sin_port = htons(0);

    // aucun contrle sur l'adresse IP :
    adresse.sin_addr.s_addr = htons(INADDR_ANY);

    // Autre exemple :
    // connexion sur le port 33016 fix
    // adresse.sin_port = htons(33016);
    // depuis localhost seulement :
    // inet_aton("127.0.0.1", &adresse.sin_addr);

    if (bind(sock, (struct sockaddr*) &adresse,sizeof(struct sockaddr_in)) < 0) {
        close(sock);
        fprintf(stderr, "Erreur bind\n");
        return -1;
    }

    return sock;
}

int affiche_adresse_socket(int sock, char *buffer) {
    struct sockaddr_in adresse;
    socklen_t longueur;

    longueur = sizeof(struct sockaddr_in);

    if (getsockname(sock, (struct sockaddr*)&adresse, &longueur) < 0) {
        fprintf(stderr, "Erreur getsockname\n");
        return -1;
    }

    sprintf(buffer, "IP = %s, Port = %u", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));

    return 0;
}

void traite_connection(int sock) {
    struct sockaddr_in adresse;
    socklen_t longueur;
    char bufferR[BUFFER_SIZE];
    char bufferW[BUFFER_SIZE];
    char tmp[50];
    int nb;

    longueur = sizeof(struct sockaddr_in);

    // Récupération de l'adresse du pair connecté
    if (getpeername(sock, (struct sockaddr*) &adresse, &longueur) < 0)  {
        fprintf(stderr, "Erreur getpeername\n");
        return;
    }

    // Formatage de l'adresse IP et du port du pair dans bufferW
    sprintf(bufferW, "IP = %s, Port = %u\n", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));

    // Affichage de l'adresse machine client
    printf("Machine distante : %s", bufferW);

    // Préparation du message de bienvenue et de la demande d'entrée utilisateur
    strcpy(bufferW, "Votre adresse : "); // copy le texte dans bufferW
    affiche_adresse_socket(sock, tmp);
    strcat(bufferW, tmp); // Ajoute tpm à bufferW
    strcat(bufferW, "Veuillez entrer une phrase : "); // Ajoute le texte dans bufferW
    write(sock, bufferW, BUFFER_SIZE); // Envoie du contenu de bufferW au client contecté

    nb = read(sock, bufferR, BUFFER_SIZE); // Lit et attribut les données du socket à bufferR
    printf("L'utilisateur distant a tapé : %s\n", bufferR);// Affichage des données de bufferR

    sprintf(bufferW, "Vous avez tapé : %s\n", bufferR); // Attribut la valeur de droite à bufferW
    strcat(bufferW, "Appuyez sur entrée pour terminer\n"); // Ajout du texte à bufferW
    write(sock, bufferW, BUFFER_SIZE); // Envoie du contenu de bufferW au client contecté
}

int main() {
    int sock_contact;
    int sock_connectee;
    struct sockaddr_in adresse;
    socklen_t longueur;
    pid_t pid_fils;
    char tmp[50];

    sock_contact = cree_socket_tcp_ip();

    if (sock_contact < 0) return -1;

    listen(sock_contact, 5);

    affiche_adresse_socket(sock_contact, tmp);
    printf("Local address : %s\n", tmp);

    while (1) {
        longueur = sizeof(struct sockaddr_in);
        sock_connectee = accept(sock_contact, (struct sockaddr*)&adresse, &longueur);

        if (sock_connectee < 0) {
            fprintf(stderr, "Erreur accept\n");
            return -1;
        }

        pid_fils = fork();

        if (pid_fils == -1) {
            fprintf(stderr, "Erreur fork\n");
            return -1;
        }

        if (pid_fils == 0) { // Processus fils
            close(sock_contact);
            traite_connection(sock_connectee);
            exit(0);
        } else { // Processus parent
            close(sock_connectee);
        }
    }

    return 0;
}
