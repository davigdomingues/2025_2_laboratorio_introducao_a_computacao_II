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
        grupo->capacidade*2; /* Multiplicação em potência de 2 para diminuir o gasto computacional 
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

Dados ordenacaoBubbleSort(int *vetor, int tamanho) { /* Segue uma lógica simples de analogia das bolhas: 
menores no começo, maiores no final, sendo as bolhas os valores dos dados. */
    Dados dados = {0, 0}; // Inicialização padrão.
    int temporario;
    
    for (int i = 0; i < tamanho - 1; i++) { // Loop para a base de comparação.
        for (int j = 0; j < tamanho - i - 1; j++) { // Loop para o percorrimento comparativo.
            dados.comparacoes++; // Contagem de comparações no algoritmo de ordenação solicitado.
            
            if (vetor[i] > vetor[j+1]) { // Condicional para a operação de deslocamento de valores.
                temporario = vetor[j];
				vetor[j] = vetor[j+1];
				vetor[j+1] = temporario;
				dados.movimentacoes += 3; // Contagem de movimentacoes de dados no vetor no algoritmo de ordenação solicitado.
            }
        }
    }
    
    return dados;
}

void imprimirGrupo(char *tipo, Grupo grupo) { // Função padronizada de impressão dos dados.
    printf("%s - [", tipo);
    
    for (int i = 0; i < grupo.tamanho; i++) { /* Loop para percorrer as structs e captar os
	comprimentos de cada nome presente. */
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
    
    while (fgets(buffer, sizeof(buffer), stdin)) { /* Loop de leitura de dados até lidar com EOF ("End Of File")
		-> comando CTRL+D (windows). */
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
    printf("\n");
    imprimirGrupo("Externa", externa, dexterna);
    printf("\n");
    
    return 0;
}
