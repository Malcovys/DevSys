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

int cree_socket_tcp_client(int argc, char** argv) {
    struct sockaddr_in adresse;
    int sock;

    // Vérifie si le nombre d'arguments est correct
    if (argc != 3) {
        fprintf(stderr, "Usage : %s adresse port\n", argv[0]);
        exit(0);
    }

    // Crée un socket TCP (AF_INET pour IPv4, SOCK_STREAM pour TCP)
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Erreur socket\n");
        return -1;
    }

    // Initialise la structure sockaddr_in à zéro
    memset(&adresse, 0, sizeof(struct sockaddr_in));

    // Définit la famille d'adresses à AF_INET (IPv4)
    adresse.sin_family = AF_INET;

    // Convertit le numéro de port de la chaîne de caractères à un entier, puis le convertit en format réseau
    adresse.sin_port = htons(atoi(argv[2]));

    // Convertit l'adresse IP de la chaîne de caractères en format réseau et la stocke dans la structure sockaddr_in
    inet_aton(argv[1], &adresse.sin_addr);

    // Tente de se connecter au serveur spécifié par l'adresse et le port
    if (connect(sock, (struct sockaddr*) &adresse, sizeof(struct sockaddr_in)) < 0) {
        close(sock);
        fprintf(stderr, "Erreur connect\n");
        return -1;
    }

    // Retourne le descripteur du socket si la connexion est réussie
    return sock;
}

int affiche_adresse_socket(int sock) {
    struct sockaddr_in adresse;
    socklen_t longueur;

    longueur = sizeof(struct sockaddr_in);

    if (getsockname(sock, (struct sockaddr*)&adresse, &longueur) < 0)   {
        fprintf(stderr, "Erreur getsockname\n");
        return -1;
    }

    printf("IP = %s, Port = %u\n", inet_ntoa(adresse.sin_addr),ntohs(adresse.sin_port));

    return 0;
}

void traite_connection (int sock) {

}

int main(int argc, char**argv)  {
    int sock;
    char buffer[BUFFER_SIZE];

    sock = cree_socket_tcp_client(argc, argv);

    if (sock < 0)   {
        puts("Erreur connection socket client");
    }
}