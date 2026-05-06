#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float solde = 0.0;

/* ========== SAUVEGARDE ========== */
void sauvegarder_solde() {
    FILE *f = fopen("solde.txt", "w");
    if (f != NULL) {
        fprintf(f, "%.2f", solde);
        fclose(f);
    }
}

void charger_solde() {
    FILE *f = fopen("solde.txt", "r");
    if (f != NULL) {
        fscanf(f, "%f", &solde);
        fclose(f);
    }
}

void sauvegarder_transaction(float montant, char type[]) {
    FILE *f = fopen("historique.txt", "a");
    if (f == NULL) return;
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    char date[30];
    strftime(date, 30, "%d/%m/%Y %H:%M:%S", tm_info);
    fprintf(f, "[%s] %s : %.2f FCFA\n", date, type, montant);
    fclose(f);
}

/* ========== DEPOT ========== */
void deposer_argent() {
    float montant;
    printf("\n=== DEPOT ===\n");
    printf("Solde actuel : %.2f FCFA\n", solde);
    printf("Montant a deposer : ");
    scanf("%f", &montant);
    if (montant <= 0) {
        printf("Montant invalide.\n");
    } else {
        solde += montant;
        sauvegarder_transaction(montant, "Depot");
        sauvegarder_solde();
        printf("Depot effectue. Nouveau solde : %.2f FCFA\n", solde);
    }
}

/* ========== RETRAIT ========== */
void retirer_argent() {
    float montant;
    printf("\n=== RETRAIT ===\n");
    printf("Solde actuel : %.2f FCFA\n", solde);
    printf("Montant a retirer : ");
    scanf("%f", &montant);
    if (montant <= 0) {
        printf("Montant invalide.\n");
    } else if (montant > solde) {
        printf("Solde insuffisant.\n");
    } else {
        solde -= montant;
        sauvegarder_transaction(montant, "Retrait");
        sauvegarder_solde();
        printf("Retrait effectue. Nouveau solde : %.2f FCFA\n", solde);
    }
}

/* ========== SOLDE ========== */
void afficher_solde() {
    printf("\n=== SOLDE ===\n");
    printf("Solde actuel : %.2f FCFA\n", solde);
}

/* ========== HISTORIQUE ========== */
void afficher_historique() {
    FILE *f = fopen("historique.txt", "r");
    printf("\n=== HISTORIQUE DES TRANSACTIONS ===\n");
    if (f == NULL) {
        printf("Aucune transaction enregistree.\n");
        return;
    }
    char ligne[100];
    while (fgets(ligne, sizeof(ligne), f)) {
        printf("%s", ligne);
    }
    fclose(f);
}

/* ========== MENU PRINCIPAL ========== */
int main() {
    charger_solde();
    int choix;

    do {
        printf("\n=============================\n");
        printf("  GESTION FINANCIERE\n");
        printf("=============================\n");
        printf("Solde actuel : %.2f FCFA\n", solde);
        printf("1. Depot\n");
        printf("2. Retrait\n");
        printf("3. Afficher solde\n");
        printf("4. Historique des transactions\n");
        printf("0. Quitter\n");
        printf("Votre choix : ");
        scanf("%d", &choix);

        switch(choix) {
            case 1: deposer_argent(); break;
            case 2: retirer_argent(); break;
            case 3: afficher_solde(); break;
            case 4: afficher_historique(); break;
            case 0: printf("Au revoir !\n"); break;
            default: printf("Choix invalide.\n");
        }
    } while (choix != 0);

    return 0;
}
