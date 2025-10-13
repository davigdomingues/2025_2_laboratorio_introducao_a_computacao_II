#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // Necessário para o tipo uint32_t usado em util.h
#include "util.h"

void algoritmo_id1_quicksort(int *array, int n);
void algoritmo_id2_mergesort(int *array, int n);
void algoritmo_id3_radixsort(int *array, int n);

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

// Substitui o algoritmo 2 por Counting Sort (estável, O(n + K) em tempo)
// - Funciona para inteiros com possíveis valores negativos (deslocamento por min).
void algoritmo_id2_countingsort(int *array, int n) {
    if (n <= 1) return;

    int minv = array[0], maxv = array[0];
    for (int i = 1; i < n; ++i) {
        if (array[i] < minv) minv = array[i];
        if (array[i] > maxv) maxv = array[i];
    }

    long long range_ll = (long long)maxv - (long long)minv + 1LL;

    size_t K = (size_t)range_ll;
    size_t *count = (size_t *)calloc(K, sizeof(size_t));

    // Conta frequências
    for (int i = 0; i < n; ++i) {
        size_t idx = (size_t)((long long)array[i] - (long long)minv);
        ++count[idx];
    }

    // Prefixo exclusivo -> posições de escrita
    size_t sum = 0;
    for (size_t k = 0; k < K; ++k) {
        size_t c = count[k];
        count[k] = sum;
        sum += c;
    }

    int *tmp = (int *)malloc((size_t)n * sizeof(int));

    // Distribui estavelmente
    for (int i = 0; i < n; ++i) {
        size_t idx = (size_t)((long long)array[i] - (long long)minv);
        tmp[count[idx]++] = array[i];
    }

    memcpy(array, tmp, (size_t)n * sizeof(int));
    free(tmp);
    free(count);
}

// Substitui o algoritmo 3 por Radix Sort LSD (base 256) para int assinados
// - Usa XOR 0x80000000 para ordenar como unsigned e preservar ordem de sinais.
// - Estável, 4 passes fixos, O(4 * (n + 256)).
void algoritmo_id3_radixsort(int *array, int n) {
    if (n <= 1) return;

    size_t N = (size_t)n;
    int *tmp = (int *)malloc(N * sizeof(int));

    size_t count[256];
    for (int pass = 0; pass < 4; ++pass) {
        // Zera contadores
        memset(count, 0, sizeof(count));

        // Conta por byte do valor "biased"
        for (size_t i = 0; i < N; ++i) {
            uint32_t u = ((uint32_t)array[i]) ^ 0x80000000u;
            uint32_t b = (u >> (pass * 8)) & 0xFFu;
            ++count[b];
        }

        // Prefixo exclusivo
        size_t sum = 0;
        for (int b = 0; b < 256; ++b) {
            size_t c = count[b];
            count[b] = sum;
            sum += c;
        }

        // Distribui estavelmente para tmp
        for (size_t i = 0; i < N; ++i) {
            uint32_t u = ((uint32_t)array[i]) ^ 0x80000000u;
            uint32_t b = (u >> (pass * 8)) & 0xFFu;
            tmp[count[b]++] = array[i];
        }

        // Troca buffers para o próximo passe
        int *swap = array;
        array = tmp;
        tmp = swap;
    }

    // 4 passes (par) -> resultado final está de volta no buffer original
    free(tmp);
}

int main() {
    int n, id_algoritmo;
    char tipo_sequencia[10];

    scanf("%d", &n);
    scanf("%9s", tipo_sequencia);
    scanf("%d", &id_algoritmo);

    // Sanitiza n para evitar comportamentos indefinidos (ex.: CRC com tamanho enorme)
    if (n < 0) {
        fprintf(stderr, "Valor de n invalido.\n");
        return 1;
    }

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
    else {
        // Default seguro: random
        int seed = 12345;
        for (int i = 0; i < n; i++)
            sequencia[i] = get_random(&seed, n);
    }

    switch (id_algoritmo) {
        case 1:
            algoritmo_id1_quicksort(sequencia, n);
            break;

        case 2:
            algoritmo_id2_countingsort(sequencia, n);
            break;

        case 3:
            algoritmo_id3_radixsort(sequencia, n);
            break;

        default:
            printf("Algoritmo com ID %d nao implementado.\n", id_algoritmo);
            break;
    }

    init_crc32(); // Inicializa a tabela de CRC32
    uint32_t saida = crc32(0, sequencia, (size_t)n * sizeof(int)); // Calcula o hash
    printf("%08X\n", saida); // Imprime a saída no formato hexadecimal

    free(sequencia);

    return 0;
}