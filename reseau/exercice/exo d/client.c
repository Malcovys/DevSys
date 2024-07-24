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

void traite_commande (int sock, char* product, char* quantity) {
    int nb;
    char bufferR[BUFFER_SIZE];
    char bufferW[BUFFER_SIZE];

    sprintf(bufferW, "%s %s", product, quantity);
    write(sock, bufferW, BUFFER_SIZE);

    nb = read(sock, bufferR, BUFFER_SIZE);
    printf("%s\n", bufferR);
}

int main(int argc, char**argv)  {
    int sock;

    sock = cree_socket_tcp_client(argc, argv);

    if(argc != 5) {
        fprintf(stderr, "Usage : %s adresse port product quantity\n", argv[0]);
        exit(0);
    }

    if (sock < 0) return -1;

    traite_commande(sock, argv[3], argv[4]);

    return 0;
}