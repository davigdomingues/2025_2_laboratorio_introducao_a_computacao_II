#include <stdio.h>
#include <string.h>
#include "ponto.h"

int main() {
    int totalPontos;
    float x, y;
    char opcao[4];

    printf("Quantos pontos deseja criar? ");
    scanf("%d", &totalPontos);

    Ponto pontos[totalPontos];

    for (int i = 0; i < totalPontos; i++) {
        printf("Informe as coordenadas do ponto %d: ", i + 1);
        scanf("%f %f", &x, &y);
        criarPonto(&pontos[i], x, y);
    }

    do {
        printf("\n--- Menu ---\n");
        printf("1. Ver todos os pontos\n");
        printf("2. Alterar um ponto\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%3s", opcao);

        if (strcmp(opcao, "1") == 0) {
            for (int i = 0; i < totalPontos; i++) {
                printf("Ponto %d: ", i + 1);
                mostrarPonto(&pontos[i]);
                printf("\n");
            }
        } 
        else if (strcmp(opcao, "2") == 0) {
            int indice;
            printf("Qual ponto deseja alterar (1 a %d)? ", totalPontos);
            scanf("%d", &indice);

            if (indice >= 1 && indice <= totalPontos) {
                printf("Digite as novas coordenadas: ");
                scanf("%f %f", &x, &y);
                alterarPonto(&pontos[indice - 1], x, y);
                printf("Ponto alterado!\n");
            } else {
                printf("Indice invalido!\n");
            }
        }

    } while (strcmp(opcao, "0") != 0);

    printf("Tenha um bom dia :)\n");
    return 0;
}