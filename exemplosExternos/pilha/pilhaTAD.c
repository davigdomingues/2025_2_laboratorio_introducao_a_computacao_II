#include <stdio.h>
#include <string.h>
#include "PilhaTAD.h"

void criarPilha(Pilha *p) {
    p->topo = -1;
}

int estaVazia(Pilha *p) {
    return p->topo == -1;
}

int estaCheia(Pilha *p) {
    return p->topo == MAX - 1;
}

void empilhar(Pilha *p, const char *texto) {
    if (!estaCheia(p))
        strcpy(p->itens[++(p->topo)], texto);
    
    else
        printf("Erro: Pilha cheia!\n");
}

void desempilhar(Pilha *p, char *saida) {
    if (!estaVazia(p))
        strcpy(saida, p->itens[(p->topo)--]);
    
    else
        printf("Erro: Pilha vazia!\n");
}