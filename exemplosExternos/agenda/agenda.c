#include <stdio.h>
#include <string.h>
#include "agenda.h"

void criarAgenda(Agenda *a) {
    a->total = 0;
}

void adicionarContato(Agenda *a, const char *nome, const char *telefone) {
    if (a->total < MAX_CONTATOS) {
        strcpy(a->contatos[a->total].nome, nome);
        strcpy(a->contatos[a->total].telefone, telefone);
        a->total++;
        printf("Contato adicionado!\n");
    } 
    
    else
        printf("Agenda cheia!\n");
}

void listarContatos(const Agenda *a) {
    if (a->total == 0) {
        printf("Agenda vazia.\n");
        return;
    }
    
    printf("\n--- Lista de Contatos ---\n");
    
    for (int i = 0; i < a->total; i++)
        printf("%d. %s - %s\n", i + 1, a->contatos[i].nome, a->contatos[i].telefone);
}

void buscarContato(const Agenda *a, const char *nome) {
    for (int i = 0; i < a->total; i++) {
        if (strcmp(a->contatos[i].nome, nome) == 0) {
            printf("Encontrado: %s - %s\n", a->contatos[i].nome, a->contatos[i].telefone);
            return;
        }
    }
    
    printf("Contato nao encontrado.\n");
}