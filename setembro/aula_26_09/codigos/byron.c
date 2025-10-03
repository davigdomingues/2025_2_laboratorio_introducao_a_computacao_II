/* Autores: 
Davi Gabriel Domingues (N° USP: 15447497)
Pedro Martins de Oliveira (N° USP: 13696213) 
*/

#include <ctype.h> // já presente? se não, incluído aqui para trim (pode ser removido se duplicado)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h> // adicionado para medições de tempo (clock)

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
        
    // Desempate por id para garantir ordem estável/determinística (menor id == entrou antes)
    if (brinquedo1->id < brinquedo2->id)
        return -1;
        
    if (brinquedo1->id > brinquedo2->id)
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

    // Usar heap em vez de VLA para evitar estouro de pilha em entradas grandes
    Brinquedo *leftArr = NULL;
    Brinquedo *rightArr = NULL;

    if (n1 > 0) {
        leftArr = (Brinquedo*)malloc(n1 * sizeof(Brinquedo));
        if (!leftArr)
            return; // falha em alocar -> evita comportamento indefinido
    }
    
    if (n2 > 0) {
        rightArr = (Brinquedo*)malloc(n2 * sizeof(Brinquedo));

        if (!rightArr) { 
            free(leftArr); 
            return; 
        }
    }

    for (i = 0; i < n1; i++)
        leftArr[i] = brinquedos[inicio + i];

    for (j = 0; j < n2; j++)
        rightArr[j] = brinquedos[meio + 1 + j];

    i = 0;
    j = 0;
    k = inicio;

    while (i < n1 && j < n2) {
        if (compararBrinquedos(&leftArr[i], &rightArr[j]) <= 0) {
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

    free(leftArr);
    free(rightArr);
}

void ordenacaoViaMergeSort(Brinquedo *brinquedos, int inicio, int fim) {
    if(inicio >= fim) return; // Condição de saída

    int meio = inicio + (fim - inicio)/2;

    ordenacaoViaMergeSort(brinquedos, inicio, meio);
    ordenacaoViaMergeSort(brinquedos, meio + 1, fim);

    junta(brinquedos, inicio, meio, fim);

    return;
}

int particiona(Brinquedo *brinquedo, int inicio, int fim) {
    // função de particionamento para o QuickSort
    // - escolhe o último elemento como pivo (estratégia simples e determinística)
    // - reorganiza o subarray de modo que todos elementos <= pivo fiquem à esquerda,
    //   e os > pivo fiquem à direita, mantendo estabilidade relativa onde possível.
    // - retorna o índice final do pivo após a partição.

    Brinquedo pivo = brinquedo[fim];
    int i = inicio - 1;
    
    for (int j = inicio; j < fim; j++) {
        // se brinquedo[j] é menor ou igual ao pivo segundo nosso critério,
        // incrementa i e troca para mover esse elemento para a "parte menor".
        if (compararBrinquedos(&brinquedo[j], &pivo) <= 0) {
            i++;
            capitaoGinyu(&brinquedo[i], &brinquedo[j]); // troca estável via struct completo
        }
    }
    // coloca o pivo na sua posição ordenada final
    capitaoGinyu(&brinquedo[i + 1], &brinquedo[fim]);
    return i + 1;
}

void ordenacaoViaQuickSort(Brinquedo *brinquedo, int inicio, int fim) {
    // Implementação recursiva do QuickSort:
    // - chama particiona para dividir o array em duas partes em torno do pivo
    // - aplica recursivamente nas subpartes esquerda e direita
    // - condição de parada: subarray com zero ou um elemento (inicio >= fim)
    if (inicio < fim) {
        int pi = particiona(brinquedo, inicio, fim);
        ordenacaoViaQuickSort(brinquedo, inicio, pi - 1);
        ordenacaoViaQuickSort(brinquedo, pi + 1, fim);
    }
}

// Copia n elementos de src para dst
void copiarVetor(Brinquedo *dst, const Brinquedo *src, int n) {
    if (n <= 0) return;
    memcpy(dst, src, n * sizeof(Brinquedo));
}

// Executa um algoritmo sobre uma cópia dos dados originais, retorna tempo em segundos
void executarAlgoritmoCopia(const Brinquedo *orig, int n, int metodo, double *tempo_out) {
    Brinquedo *tmp = (Brinquedo*)malloc(n * sizeof(Brinquedo));
    if (!tmp) {
        *tempo_out = -1.0;
        return;
    }
    copiarVetor(tmp, orig, n);
    clock_t start = clock();
    switch (metodo) {
        case 1: ordenacaoViaBubbleSort(tmp, n); break;
        case 2: ordenacaoViaInsertionSort(tmp, n); break;
        case 3: ordenacaoViaMergeSort(tmp, 0, n - 1); break;
        case 4: ordenacaoViaQuickSort(tmp, 0, n - 1); break;
        default: break;
    }
    clock_t end = clock();
    *tempo_out = (double)(end - start) / (double)CLOCKS_PER_SEC;
    free(tmp);
}

// substitui a main original para suportar --batch sem quebrar o uso original
int main(int argc, char *argv[]) {
    // Modo batch: --batch <lista.txt> [all|1,2,3,4]
    if (argc >= 3 && strcmp(argv[1], "--batch") == 0) {
        const char *listaPath = argv[2];
        const char *algosSpec = (argc >= 4) ? argv[3] : "all";
        FILE *lf = fopen(listaPath, "r");
        if (!lf) {
            fprintf(stderr, "Erro ao abrir lista de testes: %s\n", listaPath);
            return -1;
        }

        // preparar lista de algoritmos a executar
        bool runAlgo[5] = { false, true, true, true, true }; // index 1..4 (default all)
        if (strcmp(algosSpec, "all") != 0) {
            // reset e parse simples "1,2" ou "2"
            for (int i = 0; i <= 4; i++) runAlgo[i] = false;
            char specbuf[128];
            strncpy(specbuf, algosSpec, sizeof(specbuf)-1);
            specbuf[sizeof(specbuf)-1] = '\0';
            char *tok = strtok(specbuf, ",");
            while (tok) {
                int a = atoi(tok);
                if (a >= 1 && a <= 4) runAlgo[a] = true;
                tok = strtok(NULL, ",");
            }
        }

        char pathbuf[1024];
        while (fgets(pathbuf, sizeof(pathbuf), lf)) {
            // trim newline/carriage
            size_t L = strlen(pathbuf);
            while (L > 0 && (pathbuf[L-1] == '\n' || pathbuf[L-1] == '\r')) { pathbuf[--L] = '\0'; }
            if (L == 0) continue;

            FILE *tf = fopen(pathbuf, "r");
            if (!tf) {
                fprintf(stderr, "Falha ao abrir arquivo de teste: %s\n", pathbuf);
                continue;
            }

            int numeroBrinquedos = 0;
            if (fscanf(tf, "%d", &numeroBrinquedos) != 1) {
                fprintf(stderr, "Formato inválido (numeroBrinquedos) em: %s\n", pathbuf);
                fclose(tf);
                continue;
            }

            Brinquedo *orig = (Brinquedo*)malloc(numeroBrinquedos * sizeof(Brinquedo));
            if (!orig) {
                fprintf(stderr, "Falha alocar vetor para: %s\n", pathbuf);
                fclose(tf);
                continue;
            }

            for (int i = 0; i < numeroBrinquedos; i++) {
                orig[i].id = i;
                if (fscanf(tf, "%19s %lf %lf", orig[i].cor, &orig[i].comprimento, &orig[i].nota) != 3) {
                    // se falhar na leitura, preencher com valores neutros e ajustar tamanho real
                    numeroBrinquedos = i;
                    break;
                }
            }
            fclose(tf);

            // executar cada algoritmo solicitado e registrar tempo em stderr (CSV)
            for (int a = 1; a <= 4; a++) {
                if (!runAlgo[a]) continue;
                double t = 0.0;
                executarAlgoritmoCopia(orig, numeroBrinquedos, a, &t);
                fprintf(stderr, "%s,alg%d,%.6f\n", pathbuf, a, t);
            }

            free(orig);
        }

        fclose(lf);
        return 0;
    }

    int numeroBrinquedos = -1, metodoSelecionado = 0;
    if (scanf("%d", &numeroBrinquedos) != 1) return -1;

    // alocação dinâmica e tratamento para caso de insucesso.
    Brinquedo *brinquedo = (Brinquedo*)malloc(numeroBrinquedos*sizeof(Brinquedo));
    if (brinquedo == NULL)
        return -1;

    for (int i = 0; i < numeroBrinquedos; i++) { // loop para leitura formatada dos dados, conforme o runcodes deseja.
        brinquedo[i].id = i;
        scanf("%s %lf %lf", brinquedo[i].cor, &brinquedo[i].comprimento, &brinquedo[i].nota);
    }

    if (scanf("%d", &metodoSelecionado) != 1) metodoSelecionado = 0; // manter compatibilidade

    // Medição de desempenho: inicia o relógio antes de executar o algoritmo.
    clock_t inicio_tempo = clock();

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
            // caso o método selecionado seja inválido, nenhuma ordenação é aplicada.
            break;
    }

    // Encerra a medição e calcula tempo decorrido (segundos).
    clock_t fim_tempo = clock();
    double tempo_decorrido = (double)(fim_tempo - inicio_tempo) / (double)CLOCKS_PER_SEC;
    // Imprime tempo em stderr para não poluir a saída padrão exigida pela plataforma de avaliação.
    fprintf(stderr, "Tempo (s): %.6f\n", tempo_decorrido);
    
    for (int i = 0; i < numeroBrinquedos; i++) // impressão padronizada dos id's associados à ordem de entrada dos brinquedos.
        printf("%d;", brinquedo[i].id); // imprime os ids separados por ponto e vírgula (formato exigido pela plataforma).
    
    printf("\n"); // quebra de linha posta por questão de conformidade à saída desejada pelo runcodes.
    free(brinquedo);

    return 0;
}