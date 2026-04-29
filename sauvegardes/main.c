
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

float solde = 0.0;

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

    fprintf(f, "[%s] %s : %.2f\n", date, type, montant);
    fclose(f);
}

void afficher_historique() {
    FILE *f = fopen("historique.txt", "r");
    if (f == NULL) {
        printf("Aucune transaction.\n");
        return;
    }
    char ligne[100];
    printf("=== HISTORIQUE ===\n");
    while (fgets(ligne, sizeof(ligne), f)) {
        printf("%s", ligne);
    }
    fclose(f);
}

int main() {
    charger_solde();
    int choix;
    float montant;

    printf("=== GESTION FINANCE ===\n");
    printf("Solde actuel : %.2f\n", solde);
    printf("1. Depot\n");
    printf("2. Retrait\n");
    printf("3. Historique\n");
    printf("Votre choix : ");
    scanf("%d", &choix);

    switch(choix) {
        case 1:
            printf("Montant a deposer : ");
            scanf("%f", &montant);
            solde += montant;
            sauvegarder_transaction(montant, "Depot");
            sauvegarder_solde();
            printf("Nouveau solde : %.2f\n", solde);
            break;
        case 2:
            printf("Montant a retirer : ");
            scanf("%f", &montant);
            if (montant > solde) {
                printf("Solde insuffisant.\n");
            } else {
                solde -= montant;
                sauvegarder_transaction(montant, "Retrait");
                sauvegarder_solde();
                printf("Nouveau solde : %.2f\n", solde);
            }
            break;
        case 3:
            afficher_historique();
            break;
        default:
            printf("Choix invalide.\n");
    }
    return 0;
}
