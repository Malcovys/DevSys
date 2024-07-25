#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Produit {
  char nom[50];
  float prix;
} Produit;

int chargerBaseDeDonnees(Produit *produits, char *fichierDonnees, char *messageErreur) {
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

int main() {
  Produit produits[100];
  char messageErreur[100];
  char db_path[] = "./db.csv";

  int produits_charges = chargerBaseDeDonnees(produits, db_path, messageErreur);

  if (produits_charges < 0) {
    printf("%s\n", messageErreur);
    exit(EXIT_FAILURE);
  }

  // Afficher les produits chargés
  for (int i = 0; i < produits_charges; i++) {
    printf("Produit : %s, Prix : %.2f\n", produits[i].nom, produits[i].prix);
  }

  return 0;
}
