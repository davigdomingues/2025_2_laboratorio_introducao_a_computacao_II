#ifndef PILHATAD_H
#define PILHATAD_H

#define MAX 100

typedef struct {
    char itens[MAX][100]; // cada item é uma string
    int topo;
} Pilha;

// Funções do TAD
void criarPilha(Pilha *p);
int estaVazia(Pilha *p);
int estaCheia(Pilha *p);
void empilhar(Pilha *p, const char *texto);
void desempilhar(Pilha *p, char *saida);

#endif