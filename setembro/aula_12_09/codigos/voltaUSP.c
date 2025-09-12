#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int *tamanhos;
    int tamanho;
    int capacidade;
} Grupo;

void adicionarTamanho(Grupo *grupo, int tamanho) {
    if (grupo->tamanho >= grupo->capacidade) {
        grupo->capacidade += 10;
        grupo->tamanhos = realloc(grupo->tamanhos, grupo->capacidade*sizeof(int));
    }
    
    grupo->tamanhos[grupo->tamanho] = tamanho;
    grupo->tamanho++;
}

int contarCaracteres(char *nome) {
    int tamanho = 0;
    
    for (int i = 0; nome[i] != '\0'; i++) {
        if (nome[i] != ' ')
            tamanho++;
    }
    
    return tamanho;
}

void imprimirGrupo(char *tipo, Grupo grupo) {
    printf("%s - [", tipo);
    
    for (int i = 0; i < grupo.tamanho; i++) {
        printf("%d", grupo.tamanhos[i]);
        
        if (i < grupo.tamanho - 1)
            printf(", ");
    }
    
    printf("]");
}

int main() {
    Grupo usp = {NULL, 0, 0};
    Grupo externa = {NULL, 0, 0};
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), stdin)) {
        char nome[256], tipo[256];
        
        sscanf(buffer, "%[^-]-%s", nome, tipo); // Regex para fazer função split()
        
        int tamanho = contarCaracteres(nome);
        
        if (strcmp(tipo, "usp") == 0)
            adicionarTamanho(&usp, tamanho);
        
        else if (strcmp(tipo, "externa") == 0)
            adicionarTamanho(&externa, tamanho);
    }
    
    imprimirGrupo("USP", usp);
    printf("\n");
    imprimirGrupo("Externa", externa);
    printf("\n");
    
    return 0;
}
