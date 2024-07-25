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

typedef struct {
    char nom[50];
    float prix;
} Produit;

Produit* trouver_produit_par_nom(Produit* tableau, int taille, const char* nom) {
    for (int i = 0; i < taille; i++) {
        if (strcmp(tableau[i].nom, nom) == 0) {
            return &tableau[i];
        }
    }
    return NULL;
}

int chargerBaseDeDonnees(Produit *produits, const char *fichierDonnees, char *messageErreur) {
    FILE *f = fopen(fichierDonnees, "r");
    if (f == NULL) {
        sprintf(messageErreur, "Erreur d'ouverture du fichier de base de données : %s", fichierDonnees);
        return -1; // Erreur d'ouverture
    }

    char ligne[100];
    int i = 0;
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        // Supprimer le caractère de nouvelle ligne
        ligne[strcspn(ligne, "\n")] = 0;

        // Séparation des champs par virgule
        char *nom = strtok(ligne, ",");
        if (nom == NULL) {
            sprintf(messageErreur, "Ligne invalide dans le fichier de base de données : %s.", ligne);
            fclose(f);
            return -2; // Erreur de format de ligne
        }

        char *prixStr = strtok(NULL, ",");
        if (prixStr == NULL) {
            sprintf(messageErreur, "Ligne invalide dans le fichier de base de données : %s.", ligne);
            fclose(f);
            return -2; // Erreur de format de ligne
        }
        float prix = atof(prixStr);

        // Stockage des données dans la structure Produit
        strcpy(produits[i].nom, nom);
        produits[i].prix = prix;

        i++;
    }

    fclose(f);
    return i; // Nombre de produits chargés
}

int cree_socket_tcp_ip() {
    int sock;
    struct sockaddr_in adresse;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "Erreur socket\n");
        return -1;
    }

    memset(&adresse, 0, sizeof(struct sockaddr_in));

    adresse.sin_family = AF_INET;

    // Donner un numéro de port disponible quelconque
    adresse.sin_port = htons(0);

    // Aucun contrôle sur l'adresse IP :
    adresse.sin_addr.s_addr = htons(INADDR_ANY);

    if (bind(sock, (struct sockaddr*) &adresse, sizeof(struct sockaddr_in)) < 0) {
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

float traite_commande(Produit *data, int taille, char *produit, char *quantite) {
    Produit* produit_data = trouver_produit_par_nom(data, taille, produit);

    if (produit_data == NULL) {
        return -1;
    }

    return produit_data->prix * strtof(quantite, NULL);
}

void traite_connection(int sock, const char *db_path) {
    char bufferR[BUFFER_SIZE];
    char bufferW[BUFFER_SIZE];
    int nb_cmds;
    int nb;
    socklen_t longueur;
    struct sockaddr_in adresse;

    Produit produits_data[100];
    char messageErreur[100];

    // Affichage de l'adresse machine client
    longueur = sizeof(struct sockaddr_in);
    if (getpeername(sock, (struct sockaddr*) &adresse, &longueur) < 0)  {
        fprintf(stderr, "Erreur getpeername\n");
        return;
    }
    sprintf(bufferW, "IP = %s, Port = %u\n", inet_ntoa(adresse.sin_addr), ntohs(adresse.sin_port));
    printf("Machine distante : %s", bufferW);

    // Chargement de la base de données dans la mémoire
    int produit_charger = chargerBaseDeDonnees(produits_data, db_path, messageErreur);
    if (produit_charger < 0) {
        strcpy(bufferW, messageErreur);
        write(sock, bufferW, strlen(bufferW));
        close(sock);
        return;
    }

    // Réception de la commande client
    nb = read(sock, bufferR, BUFFER_SIZE - 1);
    if (nb < 0) {
        perror("Erreur de lecture");
        close(sock);
        return;
    }
    bufferR[nb] = '\0';

    // Lire le nombre de commandes
    char *ptr = strtok(bufferR, " ");
    if (ptr == NULL) {
        strcpy(bufferW, "Format de commande invalide.");
        write(sock, bufferW, strlen(bufferW));
        close(sock);
        return;
    }
    nb_cmds = atoi(ptr);

    // Calcul du prix total des commandes
    float prix_total = 0.0;
    char *produit_commande;
    char *quantite_commande;

    for (int i = 0; i < nb_cmds; i++) {
        produit_commande = strtok(NULL, " ");
        quantite_commande = strtok(NULL, " ");

        if (produit_commande == NULL || quantite_commande == NULL) {
            sprintf(bufferW, "Commande invalide.");
            write(sock, bufferW, strlen(bufferW));
            close(sock);
            return;
        }

        float prix = traite_commande(produits_data, produit_charger, produit_commande, quantite_commande);
        
        if (prix < 0) {
            sprintf(bufferW, "Produit %s indisponible.", produit_commande);
            write(sock, bufferW, strlen(bufferW));
            close(sock);
            return;
        }

        prix_total += prix;
    }

    // Envoyer le prix total au client
    snprintf(bufferW, BUFFER_SIZE, "Le prix total des commandes est : %.2f", prix_total);
    write(sock, bufferW, strlen(bufferW));
    close(sock);
}

int main() {
    int sock_contact;
    int sock_connectee;
    struct sockaddr_in adresse;
    socklen_t longueur;
    pid_t pid_fils;
    char tmp[50];

    char db_path[] = "./db.csv";

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
            traite_connection(sock_connectee, db_path);
            exit(0);
        } else { // Processus parent
            close(sock_connectee);
        }
    }

    return 0;
}