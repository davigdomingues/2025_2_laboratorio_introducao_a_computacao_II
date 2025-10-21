#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // Necessario para o tipo uint32_t usado em util.h
#include "util.h"

/*
    Programa geral de ordenacao:
    - Gera uma sequencia (sorted/reverse/random) de inteiros 1..n
    - Ordena com um algoritmo (Quicksort/Radix sort/Counting sort)
    - Emite um CRC32 do conteudo final para verificacao rapida

    Notas de desempenho:
    - Quicksort: Hoare + mediana de tres + cutoff p/ insertion. O(n log n) medio; O(n^2) pior (mitigado pelo pivo).
    - Radix sort (MSD, American flag): base 256, in-place, nao estavel. O(w*n), w = bytes.
    - Counting sort: O(n + k). Eficiente em intervalo compacto; fallback p/ quicksort se k grande.
*/

void algoritmo_id1_quicksort(int *array, int n);
void algoritmo_id2_radixsort(int *array, int n);
void algoritmo_id3_introsort(int *array, int n);

// Troca dois inteiros por referencia
static void swap_int(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

// Insertion sort in-place em [lo, hi]; bom para subarranjos pequenos (cutoff)
static void insertion_sort(int *a, int lo, int hi) {
    // O(m^2) no pior caso (m = hi - lo + 1)
    for (int i = lo + 1; i <= hi; ++i) {
        int key = a[i], j = i - 1;
        
        // Desloca elementos maiores que key uma posicao a direita
        // Invariante: a[lo..j] ja esta ordenado e todos > key serao movidos
        while (j >= lo && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }

        a[j + 1] = key;
    }
}

// Mediana de tres: escolhe pivo entre a[lo], a[mid], a[hi] e move para a[lo]
static int median_of_three(int *a, int lo, int hi) {
    // Seleciona a mediana entre a[lo], a[mid], a[hi] e a move para a[lo] como pivo.
    // Ajuda a evitar piores casos em entradas ja (quase) ordenadas.
    int mid = lo + ((hi - lo) >> 1);
    if (a[mid] < a[lo]) 
        swap_int(&a[mid], &a[lo]);

    if (a[hi]  < a[mid]) 
        swap_int(&a[hi],  &a[mid]);

    if (a[mid] < a[lo]) 
        swap_int(&a[mid], &a[lo]);

    // Move o pivo (mediana) para a[lo]
    swap_int(&a[lo], &a[mid]);
    return a[lo];
}

// Particionamento de Hoare em [lo, hi]
// Retorna indice j tal que [lo, j] <= pivo e [j+1, hi] >= pivo
static int partition_hoare(int *a, int lo, int hi) {
    // Particionamento de Hoare:
    // - Mantem i avancando ate a[i] >= pivo e j recuando ate a[j] <= pivo; troca quando i < j.
    // - Retorna j tal que [lo..j] <= pivo e [j+1..hi] >= pivo (intervalos podem se sobrepor em valores iguais).
    int pivot = median_of_three(a, lo, hi), i = lo - 1, j = hi + 1;

    for (;;) {
        // Avanca i ate encontrar elemento >= pivo
        do { 
            ++i; 
        } while (a[i] < pivot);

        // Regride j ate encontrar elemento <= pivo
        do { 
            --j; 
        } while (a[j] > pivot);

        if (i >= j) 
            return j; // regiao particionada

        swap_int(&a[i], &a[j]);
    }
}

// Quicksort com:
// - cutoff para insertion sort quando o subarray e pequeno
// - particionamento de Hoare
// - recursao na menor particao (tail recursion elimination)
static void quicksort_impl(int *a, int lo, int hi) {
    // Estrategias:
    // - Cutoff para insertion sort quando subarray e pequeno (reduz overhead e melhora localidade)
    // - Particionamento de Hoare (menos swaps em media)
    // - Recursao sempre na menor metade (eliminacao de recursao de cauda), limitando profundidade para O(log n)
    while (lo < hi) {
        if (hi - lo + 1 <= 16) { // cutoff para pequenos subarrays (valor empirico)
            insertion_sort(a, lo, hi);
            break;
        }
        
        int p = partition_hoare(a, lo, hi);

        // Ordena recursivamente a menor metade para limitar profundidade
        if (p - lo < hi - (p + 1)) {
            quicksort_impl(a, lo, p);
            lo = p + 1; // itera na metade menor
        }
        
        else {
            quicksort_impl(a, p + 1, hi);
            hi = p; // itera na metade maior
        }
    }
}

// Interface do Quicksort
void algoritmo_id1_quicksort(int *array, int n) {
    // Guarda de sanidade: nada a ordenar para n <= 1
    if (n > 1) 
        quicksort_impl(array, 0, n - 1);
}

// Mapeia int assinado para unsigned (flip do bit de sinal) e extrai byte em 'shift'
static unsigned byte_of(int v, int shift) {
    // Mapeia a ordenacao de int assinados para unsigned via flip do bit de sinal
    uint32_t u = ((uint32_t)v) ^ 0x80000000u;
    return (unsigned)((u >> shift) & 0xFFu);
}

static void radix_msd_afs(int *a, int lo, int hi, int shift) { // [lo, hi)
    const int THRESH = 32;
    int len = hi - lo;

    if (len <= 1 || shift < 0) 
        return;

    if (len <= THRESH) { 
        insertion_sort(a, lo, hi - 1); 
        return; 
    }

    int count[256] = {0}, start[256], nextp[256];

    // Contagem por bucket
    for (int i = lo; i < hi; ++i)
        ++count[byte_of(a[i], shift)];

    // Prefixos (posicoes de inicio)
    int sum = lo;

    for (int b = 0; b < 256; ++b) {
        start[b] = sum;
        nextp[b] = sum;
        sum += count[b];
    }

    // Permutacao in-place (cycle leader)
    for (int b = 0; b < 256; ++b) {
        int i = start[b], end = start[b] + count[b];

        while (i < end) {
            unsigned db = byte_of(a[i], shift);
            if ((int)db == b)
                ++i;
            
            else {
                int dest = nextp[db]++;
                swap_int(&a[i], &a[dest]);
            }
        }
    }

    // Recursao por bucket no proximo byte
    if (shift > 0) {
        for (int b = 0; b < 256; ++b) {
            int s = start[b], c = count[b];

            if (c > 1)
                radix_msd_afs(a, s, s + c, shift - 8);
        }
    }
}

// Radix sort MSD in-place (American flag sort, base 256)
void algoritmo_id2_radixsort(int *array, int n) {
    if (n <= 1) 
        return;

    radix_msd_afs(array, 0, n, 24);
}

// log2 inteiro de n (n > 0); usado para profundidade máxima do quicksort
static int ilog2(int n) {
    int lg = 0;
    while (n > 1) { n >>= 1; ++lg; }
    return lg;
}

// Heapsort em subarray [lo..hi], indices inclusivos
static void heap_sift_down(int *a, int lo, int hi, int i0) {
    int n = hi - lo + 1, i = i0;

    for (;;) {
        int l = 2 * i + 1, r = l + 1, m = i;

        if (l < n && a[lo + l] > a[lo + m]) 
            m = l;

        if (r < n && a[lo + r] > a[lo + m]) 
            m = r;

        if (m == i) 
            break;

        swap_int(&a[lo + i], &a[lo + m]);
        i = m;
    }
}

static void heap_heapify(int *a, int lo, int hi) {
    int n = hi - lo + 1;

    for (int i = (n >> 1) - 1; i >= 0; --i)
        heap_sift_down(a, lo, hi, i);
}

static void heap_sort_range(int *a, int lo, int hi) {
    if (hi - lo + 1 <= 1) 
        return;

    heap_heapify(a, lo, hi);

    for (int end = hi; end > lo; --end) {
        swap_int(&a[lo], &a[end]);        // move maior para o fim
        // reduzir heap para [lo..end-1]; sift_down a partir da raiz (0)
        int n = end - lo, i = 0;
        for (;;) {
            int l = 2 * i + 1, r = l + 1, m = i;

            if (l < n && a[lo + l] > a[lo + m]) 
                m = l;

            if (r < n && a[lo + r] > a[lo + m]) 
                m = r;

            if (m == i) 
                break;

            swap_int(&a[lo + i], &a[lo + m]);
            i = m;
        }
    }
}

// Introsort (quicksort + heapsort + insertion sort), excelente desempenho pratico e limite O(n log n)
// Usa particionamento de Hoare ja existente; fallback para heapsort quando estourar profundidade
static void introsort_impl(int *a, int lo, int hi, int depth_limit) {
    while (lo < hi) {
        int len = hi - lo + 1;

        if (len <= 16) { // cutoff para insertion sort
            insertion_sort(a, lo, hi);
            return;
        }

        if (depth_limit == 0) {
            heap_sort_range(a, lo, hi);
            return;
        }

        --depth_limit;
        int p = partition_hoare(a, lo, hi);

        // recursiona na menor metade para limitar profundidade
        if (p - lo < hi - (p + 1)) {
            introsort_impl(a, lo, p, depth_limit);
            lo = p + 1;
        } 
        
        else {
            introsort_impl(a, p + 1, hi, depth_limit);
            hi = p;
        }
    }
}

// Interface do Introsort (algoritmo 3)
void algoritmo_id3_introsort(int *array, int n) {
    if (n <= 1) 
        return;

    int depth = (ilog2(n) << 1); // 2 * floor(log2(n))
    introsort_impl(array, 0, n - 1, depth);
}

int main() {
    int n, id_algoritmo;
    char tipo_sequencia[10];

    // Leitura: usa %9s para evitar overflow do buffer de 'tipo_sequencia'
    scanf("%d", &n);
    scanf("%9s", tipo_sequencia);
    scanf("%d", &id_algoritmo);

    // Evita overflow de size_t e acesso invalido no CRC quando n < 0
    if (n < 0) {
        fprintf(stderr, "Valor de n invalido.\n");
        return 1;
    }

    int *sequencia = (int *)malloc((size_t)n * sizeof(int));
    if (sequencia == NULL && n > 0) {
        fprintf(stderr, "Erro de alocacao de memoria.\n");
        return 1;
    }

    // Geracao da sequencia
    if (strcmp(tipo_sequencia, "sorted") == 0) {
        // Gera sequencia ja ordenada
        for (int i = 0; i < n; i++)
            sequencia[i] = i + 1;

    }

    else if (strcmp(tipo_sequencia, "reverse") == 0) {
        // Gera sequencia em ordem inversa
        for (int i = 0; i < n; i++)
            sequencia[i] = n - i;

    }

    else if (strcmp(tipo_sequencia, "random") == 0) {
        // Gera sequencia aleatoria usando a semente especificada
        // get_random(state, max) devolve valores em [1..max]
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

    // Selecao do algoritmo de ordenacao
    switch (id_algoritmo) {
        case 1:
            algoritmo_id1_quicksort(sequencia, n);
            break;

        case 2:
            algoritmo_id2_radixsort(sequencia, n);
            break;

        case 3:
            algoritmo_id3_introsort(sequencia, n);
            break;

        default:
            printf("Algoritmo com ID %d nao implementado.\n", id_algoritmo);
            break;
    }

    // Hash de verificacao (independente do algoritmo), util para validar resultados em testes automatizados
    init_crc32(); // Inicializa a tabela de CRC32
    uint32_t saida = crc32(0, sequencia, (size_t)n * sizeof(int)); // Calcula o hash
    printf("%08X\n", saida); // Imprime a saida no formato hexadecimal

    // Libera recursos
    free(sequencia);

    return 0;
}