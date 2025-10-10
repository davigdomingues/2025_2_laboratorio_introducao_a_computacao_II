#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // Necessário para o tipo uint32_t usado em util.h
#include "util.h"

void algoritmo_id1_quicksort(int *array, int n);
void algoritmo_id2_mergesort(int *array, int n);
void algoritmo_id3_heapsort(int *array, int n);

// Troca simples de dois inteiros por referência (utilizada pelos algoritmos)
static void swap_int(int *a, int *b) {
    int t = *a; *a = *b; *b = t;
}

// Insertion Sort in-place em [lo, hi]
// Bom desempenho para subarranjos pequenos (usado como cutoff no Quicksort)
static void insertion_sort(int *a, int lo, int hi) {
    for (int i = lo + 1; i <= hi; ++i) {
        int key = a[i];
        int j = i - 1;
        // Desloca elementos maiores que key uma posição à direita
        while (j >= lo && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

// Escolha de pivô por mediana de três: a[lo], a[mid], a[hi]
// Reduz a chance de piores casos em entradas quase ordenadas
static int median_of_three(int *a, int lo, int hi) {
    int mid = lo + ((hi - lo) >> 1);
    if (a[mid] < a[lo]) swap_int(&a[mid], &a[lo]);
    if (a[hi]  < a[mid]) swap_int(&a[hi],  &a[mid]);
    if (a[mid] < a[lo]) swap_int(&a[mid], &a[lo]);
    // Move o pivô (mediana) para a[lo]
    swap_int(&a[lo], &a[mid]);
    return a[lo];
}

// Particionamento de Hoare em [lo, hi]
// Retorna índice j tal que [lo, j] <= pivô e [j+1, hi] >= pivô
static int partition_hoare(int *a, int lo, int hi) {
    int pivot = median_of_three(a, lo, hi);
    int i = lo - 1;
    int j = hi + 1;
    for (;;) {
        // Avança i até encontrar elemento >= pivô
        do { ++i; } while (a[i] < pivot);
        // Regride j até encontrar elemento <= pivô
        do { --j; } while (a[j] > pivot);
        if (i >= j) return j; // região particionada
        swap_int(&a[i], &a[j]);
    }
}

// Quicksort com:
// - cutoff para insertion sort quando o subarray é pequeno
// - particionamento de Hoare
// - recursão na menor partição (tail recursion elimination)
static void quicksort_impl(int *a, int lo, int hi) {
    while (lo < hi) {
        if (hi - lo + 1 <= 16) { // cutoff para pequenos subarrays
            insertion_sort(a, lo, hi);
            break;
        }
        int p = partition_hoare(a, lo, hi);
        // Ordena recursivamente a menor metade para limitar profundidade
        if (p - lo < hi - (p + 1)) {
            quicksort_impl(a, lo, p);
            lo = p + 1; // itera na metade maior
        } else {
            quicksort_impl(a, p + 1, hi);
            hi = p; // itera na metade maior
        }
    }
}

// Interface do algoritmo 1: Quicksort
void algoritmo_id1_quicksort(int *array, int n) {
    if (n > 1) quicksort_impl(array, 0, n - 1);
}

// Mergesort iterativo (bottom-up):
// - Usa buffer temporário 'tmp' de mesmo tamanho que o array
// - width indica o tamanho dos blocos a serem mesclados (1, 2, 4, 8, ...)
// - Estável e O(n log n)
void algoritmo_id2_mergesort(int *array, int n) {
    if (n <= 1) return;
    int *tmp = (int *)malloc((size_t)n * sizeof(int));
    if (!tmp) { // fallback em caso de falta de memória
        algoritmo_id3_heapsort(array, n);
        return;
    }

    for (int width = 1; width < n; width <<= 1) {
        for (int i = 0; i < n; i += (width << 1)) {
            int left = i;
            int mid = i + width;
            int right = i + (width << 1);
            if (mid > n) mid = n;
            if (right > n) right = n;

            // Se o segundo bloco está vazio ou já está em ordem, pula a mescla
            if (mid >= right || array[mid - 1] <= array[mid]) {
                continue;
            }

            // Mescla os blocos [left, mid) e [mid, right) em tmp
            int p = left, q = mid, k = left;
            while (p < mid && q < right) {
                if (array[p] <= array[q]) tmp[k++] = array[p++];
                else tmp[k++] = array[q++];
            }
            while (p < mid) tmp[k++] = array[p++];   // copia resto da esquerda
            while (q < right) tmp[k++] = array[q++]; // copia resto da direita

            // Copia de volta para o array original (mais rápido que loop)
            memcpy(&array[left], &tmp[left], (size_t)(right - left) * sizeof(int));
        }
    }
    free(tmp);
}

// Restaura a propriedade de heap máximo a partir do índice i em um heap de tamanho n
static void sift_down(int *a, int n, int i) {
    int v = a[i];
    int half = n >> 1; // primeiros 'half' índices são nós internos
    while (i < half) {
        int l = (i << 1) + 1;
        int r = l + 1;
        // escolhe o maior filho
        int j = (r < n && a[r] > a[l]) ? r : l;
        if (a[j] <= v) break; // posição correta encontrada
        a[i] = a[j];
        i = j;
    }
    a[i] = v;
}

// Variante de sift-down que insere um valor 'v' começando na raiz (i)
// em um heap de tamanho n, evitando o swap no laço principal do sort.
static void sift_down_with_val(int *a, int n, int i, int v) {
    int half = n >> 1;
    while (i < half) {
        int l = (i << 1) + 1;
        int r = l + 1;
        int j = (r < n && a[r] > a[l]) ? r : l;
        if (a[j] <= v) break;
        a[i] = a[j];
        i = j;
    }
    a[i] = v;
}

// Heapsort in-place e não estável (otimizado: evita swap por extração)
void algoritmo_id3_heapsort(int *array, int n) {
    if (n <= 1) return;
    // Construção do heap em O(n)
    for (int i = (n >> 1) - 1; i >= 0; --i) sift_down(array, n, i);
    // Sort: extrai máximo movendo o "buraco" e inserindo o último elemento
    for (int end = n - 1; end > 0; --end) {
        int max = array[0];
        int v = array[end];
        sift_down_with_val(array, end, 0, v);
        array[end] = max;
    }
}

int main() {
    int n, id_algoritmo;
    char tipo_sequencia[10];

    scanf("%d", &n);
    scanf("%s", tipo_sequencia);
    scanf("%d", &id_algoritmo);

    int *sequencia = (int *)malloc((size_t)n * sizeof(int));
    if (sequencia == NULL && n > 0) {
        fprintf(stderr, "Erro de alocacao de memoria.\n");
        return 1;
    }

    if (strcmp(tipo_sequencia, "sorted") == 0) {
        // Gera sequência já ordenada
        for (int i = 0; i < n; i++)
            sequencia[i] = i + 1;

    } 

    else if (strcmp(tipo_sequencia, "reverse") == 0) {
        // Gera sequência em ordem inversa
        for (int i = 0; i < n; i++)
            sequencia[i] = n - i;

    } 
    
    else if (strcmp(tipo_sequencia, "random") == 0) {
        // Gera sequência aleatória usando a semente especificada
        int seed = 12345;

        for (int i = 0; i < n; i++)
            sequencia[i] = get_random(&seed, n);
    }

    switch (id_algoritmo) {
        case 1:
            algoritmo_id1_quicksort(sequencia, n);
            break;

        case 2:
            algoritmo_id2_mergesort(sequencia, n);
            break;

        case 3:
            algoritmo_id3_heapsort(sequencia, n);
            break;
        default:
            printf("Algoritmo com ID %d nao implementado.\n", id_algoritmo);
            break;
    }

    init_crc32(); // Inicializa a tabela de CRC32// Inicializa a tabela de CRC32
    uint32_t saida = crc32(0, sequencia, (size_t)n * sizeof(int)); // Calcula o hash
    printf("%08X\n", saida); // Imprime a saída no formato hexadecimal

    free(sequencia);

    return 0;
}