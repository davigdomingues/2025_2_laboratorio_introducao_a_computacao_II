#ifndef AGENDA_H
#define AGENDA_H

#define MAX_CONTATOS 50

typedef struct {
    char nome[50];
    char telefone[15];
} Contato;

typedef struct {
    Contato contatos[MAX_CONTATOS];
    int total;
} Agenda;

// Funções do TAD
void criarAgenda(Agenda *a);
void adicionarContato(Agenda *a, const char *nome, const char *telefone);
void listarContatos(const Agenda *a);
void buscarContato(const Agenda *a, const char *nome);

#endif