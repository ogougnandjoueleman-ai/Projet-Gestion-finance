#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_TRANSACTIONS 100

typedef struct {
    char type[20];
    float montant;
    char date[30];
} Transaction;

Transaction historique[MAX_TRANSACTIONS];
int nb_transactions = 0;

void enregistrer_transaction(char type[], float montant) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    Transaction t1;
    sprintf(t1.type, "%s", type);
    t1.montant = montant;
    strftime(t1.date, 30, "%d/%m/%Y %H:%M:%S", tm_info);

    historique[nb_transactions] = t1;
    nb_transactions++;
}

void afficher_historique() {
    printf("=== HISTORIQUE DES TRANSACTIONS ===\n");
    if (nb_transactions == 0) {
        printf("Aucune transaction.\n");
        return;
    }
    for (int i = 0; i < nb_transactions; i++) {
        printf("%d. [%s] %.2f - %s\n", i+1,
               historique[i].type,
               historique[i].montant,
               historique[i].date);
    }
}

int main() {
    enregistrer_transaction("Depot", 500.0);
    enregistrer_transaction("Retrait", 200.0);
    afficher_historique();
    return 0;
}
