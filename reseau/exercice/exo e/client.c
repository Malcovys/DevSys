#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define BUFFER_SIZE 256

// Fonction pour créer une socket client TCP et se connecter au serveur
int cree_socket_tcp_client(const char *adresse, const char *port) {
    int sock;
    struct sockaddr_in server_addr;
    
    // Création de la socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Erreur socket\n");
        return -1;
    }
    
    // Configuration de l'adresse du serveur
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(port));
    inet_pton(AF_INET, adresse, &server_addr.sin_addr);
    
    // Connexion au serveur
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        fprintf(stderr, "Erreur connect\n");
        close(sock);
        return -1;
    }
    
    return sock;
}

// Fonction pour envoyer les commandes au serveur et lire les réponses
void traite_commande(int sock, char **cmds, int nb_cmds) {
    char bufferW[BUFFER_SIZE];
    char bufferR[BUFFER_SIZE];
    int total_length = 0;

    // Initialiser bufferW avec le nombre de commandes
    snprintf(bufferW, BUFFER_SIZE, "%d ", nb_cmds);
    total_length = strlen(bufferW);

    // Ajouter les commandes au buffer
    for (int i = 0; i < nb_cmds; i++) {
        // Ajouter la commande à bufferW
        strncat(bufferW, cmds[i], BUFFER_SIZE - total_length - 1);
        total_length += strlen(cmds[i]);

        // Ajouter un espace entre les commandes, sauf après la dernière
        if (i < nb_cmds - 1) {
            strncat(bufferW, " ", BUFFER_SIZE - total_length - 1);
            total_length += 1;
        }
    }

    // Envoi de la commande au serveur
    write(sock, bufferW, total_length + 1);  // total_length + 1 pour inclure le caractère de fin de chaîne

    // Lecture de la réponse du serveur
    int n = read(sock, bufferR, BUFFER_SIZE - 1);
    bufferR[n] = '\0';
    printf("%s\n", bufferR);

    close(sock);
}

int main(int argc, char** argv) {
    int sock;
    int indexer = 0;
    char user_cmd[255];
    
    // Vérification du nombre d'arguments
    if (argc < 5 || ((argc - 1) % 2) != 0) {
        fprintf(stderr, "Usage : %s adresse port produit quantité [produit quantité ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    
    // Création de la socket client et connexion au serveur
    sock = cree_socket_tcp_client(argv[1], argv[2]);
    if (sock < 0) return -1;
    
    // Préparation des commandes utilisateur
    int nb_cmds = (argc - 3) / 2; //nombre de commande
    char *cmds[nb_cmds];
    for (int i = 3; i < argc; i += 2) {
        snprintf(user_cmd, sizeof(user_cmd), "%s %s", argv[i], argv[i + 1]); // impression cmd user (produit quantite) dans user_cmd
        cmds[indexer] = strdup(user_cmd);  // Duplication de la chaîne de caractères
        indexer++;
    }
    
    // Envoi des commandes au serveur et traitement des réponses
    traite_commande(sock, cmds, indexer);
    
    // Libération de la mémoire allouée
    for (int i = 0; i < indexer; i++) {
        free(cmds[i]);
    }
    
    return 0;
}
