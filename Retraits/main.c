
#include <stdio.h>
#include <stdlib.h>

float solde = 1000.0; // solde initial

void retirer_argent() {
    float montant;
    printf("=== RETRAIT ===\n");
    printf("Solde actuel : %.2f\n", solde);
    printf("Montant a retirer : ");
    scanf("%f", &montant);

    if (montant <= 0) {
        printf("Montant invalide.\n");
    } else if (montant > solde) {
        printf("Solde insuffisant.\n");
    } else {
        solde -= montant;
        printf("Retrait effectue. Nouveau solde : %.2f\n", solde);
    }
}

int main() {
    retirer_argent();
    return 0;
}
