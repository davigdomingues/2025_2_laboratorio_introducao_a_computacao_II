#include <stdio.h>
#include <string.h>
#include "agenda.h"

int main() {
    Agenda minhaAgenda;
    criarAgenda(&minhaAgenda);

    int opcao;
    char nome[50], telefone[15];

    do {
        printf("\n--- Menu ---\n");
        printf("1. Adicionar contato\n");
        printf("2. Listar contatos\n");
        printf("3. Buscar contato\n");
        printf("0. Sair\n");
        printf("Escolha: ");
        scanf("%d", &opcao);
        getchar(); // limpa buffer

        switch (opcao) {
            case 1:
                printf("Nome: ");
                fgets(nome, sizeof(nome), stdin);
                
                nome[strcspn(nome, "\n")] = '\0';

                printf("Telefone: ");
                fgets(telefone, sizeof(telefone), stdin);
                
                telefone[strcspn(telefone, "\n")] = '\0';

                adicionarContato(&minhaAgenda, nome, telefone);
                break;
                
            case 2:
                listarContatos(&minhaAgenda);
                break;
                
            case 3:
                printf("Nome para buscar: ");
                fgets(nome, sizeof(nome), stdin);
                
                nome[strcspn(nome, "\n")] = '\0';
                buscarContato(&minhaAgenda, nome);
                break;
                
            case 0:
                printf("Encerrando...\n");
                break;
                
            default:
                printf("Opcao invalida!\n");
        }
        
    } while (opcao != 0);

    return 0;
}