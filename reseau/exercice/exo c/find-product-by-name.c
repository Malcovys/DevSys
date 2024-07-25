#include <stdio.h>
#include <string.h>

//Définition du structure du produit
typedef struct {
    char nom[50];
    float prix;
} Produit;

//fonction qui renvoie un pointeur sur la structure
Produit* trouver_produit_par_nom(Produit* tableau, int taille, const char* nom) {
    for (int i = 0; i < taille; i++) {
        if (strcmp(tableau[i].nom, nom) == 0) {
            return &tableau[i];
        }
    }
    return NULL;//Si le produit n'est pas trouvé
}

//Utilisation de la fonction
int main() {
    Produit produits[] = {
        {"Pomme", 0.5},
        {"Banane", 0.3},
        {"Orange", 0.8}
    };

    int taille = sizeof(produits) / sizeof(produits[0]);
    const char* nom_recherche = "Orange";

    Produit* resultat = trouver_produit_par_nom(produits, taille, nom_recherche);

    if (resultat != NULL) {
        printf("Produit trouvé : %s, Prix : %.2f\n", resultat->nom, resultat->prix);
    } else {
        printf("Produit non trouvé.\n");
    }

    return 0;
}
