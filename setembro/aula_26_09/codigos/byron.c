#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct { // struct utilizada para padronizar o tratamento dos dados.
	int id;
	double nota;
	char cor[20];
	double comprimento;
} Brinquedo; // ordem a ser considerada: cor, comprimento, nota, id.

// funções auxiliares
void capitaoGinyu (Brinquedo *x, Brinquedo *y) { /* referência de Dragon Ball Z: "trocaaaaaar!", ou seja, realiza a troca
de variáveis no vetor, via uso de variáveis temporárias. */
    Brinquedo temporario = *x;
    *x = *y;
    *y = temporario;
}

int compararBrinquedos(const Brinquedo *brinquedo1, const Brinquedo *brinquedo2) { /* a função vai forçar a estabilidade
em todos os métodos de ordenação. */
/* retorna valor maior que 0, caso o brinquedo1 deva estar à frente do brinquedo2, mas retorna valor menor que zero,
caso contrário */
    int x = strcmp(brinquedo1->cor, brinquedo2->cor); /* função de "string.h" para analisar se os nomes são iguais (0 = iguais,
    1 = primeira string maior, -1 = segunda string maior). Usa-se o critério de comparação lexicográfica para esse tamanho. */
    if (x != 0)
        return x;
        
    if (brinquedo1->comprimento > brinquedo2->comprimento)
        return 1;
        
    if (brinquedo1->comprimento < brinquedo2->comprimento)
        return -1;
        
    // critério da nota é específico: maior nota aparecerá em primeiro na impressão final presente na main().
    if (brinquedo1->nota > brinquedo2->nota)
        return -1;
        
    if (brinquedo1->nota < brinquedo2->nota)
        return 1;
        
    return 0; // significa que os dados são "equivalentes" para o algoritmo de ordenação
}

// uso da variação "ShakeSort", para amortizar o desempenho do bubble sort, mesmo que ainda seja O(n²).
void ordenacaoViaBubbleSort(Brinquedo *brinquedo, int numeroBrinquedos) {
    if (numeroBrinquedos <= 1 || brinquedo == NULL)
        return;

    int inicio = 0, fim = numeroBrinquedos - 1;
    bool trocou = true;

    while (trocou) {
        trocou = false;

        // varredura ocorrerá da esquerda para a direita (move o maior elemento para o fim)
        for (int i = inicio; i < fim; i++) {
            if (compararBrinquedos(&brinquedo[i], &brinquedo[i+1]) > 0) {
                capitaoGinyu(&brinquedo[i], &brinquedo[i+1]);
                trocou = true;
            }
        }

        if (!trocou)
            break;

        // diminui o fim, pois o maior elemento já está na sua posição correta
        fim--;

        // Varredura da direita para a esquerda (move o menor elemento para o início)
        for (int i = fim - 1; i >= inicio; i--) {
            if (compararBrinquedos(&brinquedo[i], &brinquedo[i+1]) > 0) {
                capitaoGinyu(&brinquedo[i], &brinquedo[i+1]);
                trocou = 1;
            }
        }

        // aumenta o início, pois o menor elemento já está na sua posição correta
        inicio++;
    }
}

void ordenacaoViaInsertionSort(Brinquedo *brinquedo, int numeroBrinquedos) { /* foi escolhida a inserção binária, por conta
de seu desempenho amortizado, comparado à inserção direta tradicional */
    for (int i = 1; i < numeroBrinquedos; i++) {
        Brinquedo chave = brinquedo[i];
        int j = i - 1;

        // encontra a posição onde a 'chave' deve ser inserida no subarray chave[0...i-1]
        int inicio = 0, fim = j, posInsercao = i;

        while (inicio <= fim) {
            int meio = inicio + (fim - inicio) / 2;
            
            if (compararBrinquedos(&chave, &brinquedo[meio]) < 0) {
                posInsercao = meio;
                fim = meio - 1;
            }
            
            else
                inicio = meio + 1;
        }

        // deslocamento dos elementos
        for (j = i - 1; j >= posInsercao; j--) {
            brinquedo[j + 1] = brinquedo[j];
        }

        brinquedo[posInsercao] = chave;
    }
}

void junta(Brinquedo *brinquedos, int inicio, int meio, int fim) {
    int i, j, k;
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    // Criação e população de arrays temporários
    Brinquedo leftArr[n1], rightArr[n2];
    for (i = 0; i < n1; i++)
        leftArr[i] = brinquedos[inicio+ i];
    for (j = 0; j < n2; j++)
        rightArr[j] = brinquedos[meio + 1 + j];

    // Explicar melhor o que acontece aqui
    i = 0;
    j = 0;
    k = inicio;
    while (i < n1 && j < n2) {
        if(compararBrinquedos(&leftArr[i], &rightArr[j]) <= 0) {
            brinquedos[k] = leftArr[i];
            i++;
        }
        else {
            brinquedos[k] = rightArr[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        brinquedos[k] = leftArr[i];
        i++;
        k++;
    }

    while (j < n2) {
        brinquedos[k] = rightArr[j];
        j++;
        k++;
    }
}

void ordenacaoViaMergeSort(Brinquedo *brinquedos, int inicio, int fim) {
    printf("%d %d\n", inicio, fim);
    if(inicio >= fim) return; // Condição de saída

    int meio = inicio + (fim - inicio)/2;

    ordenacaoViaMergeSort(brinquedos, inicio, meio);
    ordenacaoViaMergeSort(brinquedos, meio + 1, fim);

    junta(brinquedos, inicio, meio, fim);

    return;
}

int particiona(Brinquedo *brinquedo, int inicio, int fim) {
    Brinquedo pivo = brinquedo[fim];
    int i = inicio - 1;
    
    for (int j = inicio; j < fim; j++) {
        if (compararBrinquedos(&brinquedo[j], &pivo) <= 0) {
            i++;
            capitaoGinyu(&brinquedo[i], &brinquedo[j]);
        }
    }
    capitaoGinyu(&brinquedo[i + 1], &brinquedo[fim]);
    return i + 1;
}

void ordenacaoViaQuickSort(Brinquedo *brinquedo, int inicio, int fim) {
    if (inicio < fim) {
        int pi = particiona(brinquedo, inicio, fim);
        
        ordenacaoViaQuickSort(brinquedo, inicio, pi - 1);
        ordenacaoViaQuickSort(brinquedo, pi + 1, fim);
    }
}

int main() {
	int numeroBrinquedos = -1, metodoSelecionado = 0;
	scanf("%d", &numeroBrinquedos);

    // alocação dinâmica e tratamento para caso de insucesso.
	Brinquedo *brinquedo = (Brinquedo*)malloc(numeroBrinquedos*sizeof(Brinquedo));
	if (brinquedo == NULL)
		return -1;

	for (int i = 0; i < numeroBrinquedos; i++) { // loop para leitura formatada dos dados, conforme o runcodes deseja.
		brinquedo[i].id = i;
		scanf("%s %lf %lf", brinquedo[i].cor, &brinquedo[i].comprimento, &brinquedo[i].nota);
	}

	scanf("%d", &metodoSelecionado); // leitura do método a ser utilizado para a ordenação, conforme o desejo do usuário.
	switch (metodoSelecionado) { // estrutura switch para o condicionamento dos casos de ordenação, no caso, o algoritmo em si.
	    case 1:
	        ordenacaoViaBubbleSort(brinquedo, numeroBrinquedos);
		    break;
		
	    case 2:
    	    ordenacaoViaInsertionSort(brinquedo, numeroBrinquedos);
            break;
        case 3:
            ordenacaoViaMergeSort(brinquedo, 0, numeroBrinquedos - 1);
            break;
        case 4:
            ordenacaoViaQuickSort(brinquedo, 0, numeroBrinquedos - 1);
            break;
        default:
            break;
	}
	
	for (int i = 0; i < numeroBrinquedos; i++) // impressão padronizada dos id's associados à ordem de entrada dos brinquedos.
	    printf("%d;", brinquedo[i].id);
	
	printf("\n"); // quebra de linha posta por questão de conformidade à saída desejada pelo runcodes.
	free(brinquedo);

	return 0;
}