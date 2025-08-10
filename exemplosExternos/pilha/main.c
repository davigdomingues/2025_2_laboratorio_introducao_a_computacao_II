#include <stdio.h>
#include "PilhaTAD.h"

int main() {
    Pilha historico;
    char acao[100];

    criarPilha(&historico);

    empilhar(&historico, "Escreveu: Olá mundo");
    empilhar(&historico, "Escreveu: Texto exemplo");
    empilhar(&historico, "Apagou uma linha");

    while (!estaVazia(&historico)) {
        desempilhar(&historico, acao);
        printf("Desfazer -> %s\n", acao);
    }

    return 0;
}