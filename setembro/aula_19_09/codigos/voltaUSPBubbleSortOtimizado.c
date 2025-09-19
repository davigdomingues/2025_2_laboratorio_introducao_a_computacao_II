//Nome: Davi Gabriel Domingues
//N° USP: 15447497

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int *tamanhos;
    int tamanho;
    int capacidade;
} Grupo;

typedef struct {
    int comparacoes;
    int movimentacoes;
} Dados;

void adicionarTamanho(Grupo *grupo, int tamanho) { /* Função para contabilizar o tamanho de cada grupo, 
sob o princípio de alocação dinâmica */
    if (grupo->tamanho >= grupo->capacidade) {
        grupo->capacidade = (grupo->capacidade == 0) ? 10 : grupo->capacidade * 2; /* Multiplicação em potência de 2 para diminuir o gasto computacional 
	de aumento linear de espaço (procedimento da entrega03 - parte 1). */
        grupo->tamanhos = realloc(grupo->tamanhos, grupo->capacidade*sizeof(int));
    }
    
    grupo->tamanhos[grupo->tamanho] = tamanho;
    grupo->tamanho++;
}

int contarCaracteres(char *nome) { /* Função para contagem dos caracteres de cada nome declarado
no seu respectivo grupo do evento (usp ou externa). */
    int tamanho = 0;
    
    for (int i = 0; nome[i] != '\0'; i++) {
        if (nome[i] != ' ')
            tamanho++;
    }
    
    return tamanho;
}

Dados ordenacaoBubbleSort(int *vetor, int tamanho) { /* Uso do Cocktail Shaker Sort (shake sort),
que é uma otimização do bubble sort */
    Dados dados = {0, 0}; // Inicialização padrão.
	
    if (tamanho <= 1 || vetor == NULL) 
		return dados;

    int inicio = 0;
    int fim = tamanho - 1;

    while (inicio < fim) {
        // Varredura da esquerda para a direita
        for (int i = inicio; i < fim; i++) {
            dados.comparacoes++;
            if (vetor[i] > vetor[i+1]) {
                int temporario = vetor[i];
                vetor[i] = vetor[i+1];
                vetor[i+1] = temporario;
                dados.movimentacoes++;
            }
        }
        // Diminui o fim pois o maior já está no lugar
        fim--;

        // Varredura da direita para a esquerda
        for (int i = fim; i > inicio; i--) {
            dados.comparacoes++;
            if (vetor[i-1] > vetor[i]) {
                int temporario = vetor[i-1];
                vetor[i-1] = vetor[i];
                vetor[i] = temporario;
                dados.movimentacoes++;
            }
        }
        // Aumenta o inicio pois o menor já está no lugar
        inicio++;
    }

    return dados;
}

void imprimirGrupo(char *tipo, Grupo grupo, Dados dados) { // Função padronizada de impressão dos dados.
    printf("%s - [", tipo);
    
    for (int i = 0; i < grupo.tamanho; i++) { /* Loop para percorrer as structs e captar os
	comprimentos de cada nome presente. */
        printf("%d", grupo.tamanhos[i]);
        
        if (i < grupo.tamanho - 1)
            printf(", ");
    }
    
    printf("]\n");
	printf("Comparações: %d, Trocas: %d", dados.comparacoes, dados.movimentacoes);
}

int main() {
    Grupo usp = {NULL, 0, 0};
    Grupo externa = {NULL, 0, 0};
    char buffer[256];
    
    while (fgets(buffer, sizeof(buffer), stdin)) { /* Loop de leitura de dados até lidar com EOF ("End Of File")
		-> comando CTRL+D (terminal Linux), CTRL+D (Windows). */
        char nome[256], tipo[256];
        
        sscanf(buffer, "%[^-]-%s", nome, tipo); // Regex para fazer função split()
        
        int tamanho = contarCaracteres(nome);
        
        if (strcmp(tipo, "usp") == 0)
            adicionarTamanho(&usp, tamanho);
        
        else if (strcmp(tipo, "externa") == 0)
            adicionarTamanho(&externa, tamanho);
    }
    
    Dados dusp = ordenacaoBubbleSort(usp.tamanhos, usp.tamanho);
    Dados dexterna = ordenacaoBubbleSort(externa.tamanhos, externa.tamanho);
    
    imprimirGrupo("USP", usp, dusp);
    printf("\n\n");
    imprimirGrupo("Externa", externa, dexterna);
    printf("\n");
    
    return 0;
}