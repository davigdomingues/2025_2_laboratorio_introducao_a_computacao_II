#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h> // Necessário para o tipo uint32_t usado em util.h
#include "util.h"

/*
    Programa geral de ordenação:
    - Gera uma sequência (sorted/reverse/random) de inteiros 1..n
    - Ordena com um entre três algoritmos (Quicksort/Mergesort/Heapsort)
    - Emite um hash CRC32 do conteúdo final como verificação rápida de corretude

    Observações de desempenho:
    - Quicksort (Hoare + mediana de três + cutoff para insertion sort) costuma ser o mais rápido em média
      O(n log n) típico, O(n^2) no pior caso (mitigado pela escolha do pivô).
    - Mergesort (bottom-up iterativo, com runs pequenas pré-ordenadas por insertion sort)
      O(n log n) no pior/medio/melhor, estável; usa buffer auxiliar e alterna ponteiros (ping-pong).
    - Heapsort (com construção Floyd e cutoff para insertion na cauda pequena)
      O(n log n) no pior/medio/melhor; in-place, não estável.
*/

void algoritmo_id1_quicksort(int *array, int n);
void algoritmo_id2_mergesort(int *array, int n);
void algoritmo_id3_heapsort(int *array, int n);

// Troca simples de dois inteiros por referência
static void swap_int(int *a, int *b) {
    int t = *a; 
    *a = *b; 
    *b = t;
}

// Insertion Sort in-place em [lo, hi]
// Bom desempenho para subarranjos pequenos (usado como cutoff no Quicksort)
static void insertion_sort(int *a, int lo, int hi) {
    // Complexidade O(m^2) no pior caso (m = hi - lo + 1); porém muito eficiente para subarranjos pequenos.
    // Útil como "cutoff" para reduzir overhead de recursão/merges em algoritmos híbridos.
    for (int i = lo + 1; i <= hi; ++i) {
        int key = a[i], j = i - 1;
        
        // Desloca elementos maiores que key uma posição à direita
        // Invariante: a[lo..j] já está ordenado e todos > key serão movidos
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
    // Seleciona a mediana entre a[lo], a[mid], a[hi] e a move para a[lo] como pivô.
    // Ajuda a evitar piores casos em entradas já (quase) ordenadas.
    int mid = lo + ((hi - lo) >> 1);
    if (a[mid] < a[lo]) 
        swap_int(&a[mid], &a[lo]);

    if (a[hi]  < a[mid]) 
        swap_int(&a[hi],  &a[mid]);

    if (a[mid] < a[lo]) 
        swap_int(&a[mid], &a[lo]);

    // Move o pivô (mediana) para a[lo]
    swap_int(&a[lo], &a[mid]);
    return a[lo];
}

// Particionamento de Hoare em [lo, hi]
// Retorna índice j tal que [lo, j] <= pivô e [j+1, hi] >= pivô
static int partition_hoare(int *a, int lo, int hi) {
    // Particionamento de Hoare:
    // - Mantém i avançando até a[i] >= pivô e j recuando até a[j] <= pivô; troca quando i < j.
    // - Retorna j tal que [lo..j] <= pivô e [j+1..hi] >= pivô (intervalos podem se sobrepor em valores iguais).
    int pivot = median_of_three(a, lo, hi), i = lo - 1, j = hi + 1;

    for (;;) {
        // Avança i até encontrar elemento >= pivô
        do { 
            ++i; 
        } while (a[i] < pivot);

        // Regride j até encontrar elemento <= pivô
        do { 
            --j; 
        } while (a[j] > pivot);

        if (i >= j) 
            return j; // região particionada

        swap_int(&a[i], &a[j]);
    }
}

// Quicksort com:
// - cutoff para insertion sort quando o subarray é pequeno
// - particionamento de Hoare
// - recursão na menor partição (tail recursion elimination)
static void quicksort_impl(int *a, int lo, int hi) {
    // Estratégias:
    // - Cutoff para insertion sort quando subarray é pequeno (reduz overhead e melhora localidade)
    // - Particionamento de Hoare (menos swaps em média)
    // - Recursão sempre na menor metade (eliminação de recursão de cauda), limitando profundidade para O(log n)
    while (lo < hi) {
        if (hi - lo + 1 <= 16) { // cutoff para pequenos subarrays (valor empírico)
            insertion_sort(a, lo, hi);
            break;
        }
        
        int p = partition_hoare(a, lo, hi);

        // Ordena recursivamente a menor metade para limitar profundidade
        if (p - lo < hi - (p + 1)) {
            quicksort_impl(a, lo, p);
            lo = p + 1; // itera na metade maior
        }
        
        else {
            quicksort_impl(a, p + 1, hi);
            hi = p; // itera na metade maior
        }
    }
}

// Interface do algoritmo Quicksort
void algoritmo_id1_quicksort(int *array, int n) {
    // Guarda de sanidade: nada a ordenar para n <= 1
    if (n > 1) 
        quicksort_impl(array, 0, n - 1);
}

// Helpers para detectar ordenação e reverter rapidamente casos degenerados
static int is_sorted_asc(const int *a, int n) {
    // Checagem linear O(n) usada como fast-path para evitar trabalho desnecessário
    for (int i = 1; i < n; ++i) {
        if (a[i - 1] > a[i]) {
            return 0;
        }
    }

    return 1;
}

static int is_sorted_desc(const int *a, int n) {
    // Varredura linear O(n) para detectar ordem estritamente decrescente
    for (int i = 1; i < n; ++i) {
        if (a[i - 1] < a[i]) 
            return 0;
    }
     
    return 1;
}

static void reverse_array(int *a, int n) {
    // Inverte in-place em O(n) quando detectado array decrescente
    for (int i = 0, j = n - 1; i < j; ++i, --j) 
        swap_int(&a[i], &a[j]);
}

// Mergesort iterativo (bottom-up) hibridizado com insertion sort para runs pequenas
// - Pré-ordena runs de tamanho RUN com insertion sort
// - Faz merge em "ping-pong" entre array e tmp, copiando ao final só se necessário
void algoritmo_id2_mergesort(int *array, int n) {
    // Versão iterativa (bottom-up) com runs pequenas otimizadas via insertion sort (melhora localidade e cache).
    if (n <= 1) 
        return;

    // Fast-paths: já ordenado crescente ou totalmente decrescente
    if (is_sorted_asc(array, n)) 
        return;

    if (is_sorted_desc(array, n)) { 
        reverse_array(array, n); 
        return; 
    }

    size_t N = (size_t)n;

    // Pré-ordena runs pequenas com insertion sort
    const size_t RUN = 32; // tamanho de run escolhido empiricamente; ajustável conforme arquitetura e dados

    for (size_t i = 0; i < N; i += RUN) {
        size_t lo = i;
        size_t hi = (i + RUN - 1 < N) ? (i + RUN - 1) : (N - 1);
        insertion_sort(array, (int)lo, (int)hi);
    }

    // Se já coube tudo em uma run, já está ordenado
    if (N <= RUN) return;

    int *tmp = (int *)malloc(N * sizeof(int));

    if (!tmp) {
        // Fallback robusto em caso de falta de memória para o buffer auxiliar do merge
        algoritmo_id3_heapsort(array, n);
        return;
    }

    int *src = array, *dst = tmp;
    size_t width = RUN;
    while (width < N) {
        // Pré-checagem: se todas as fronteiras de runs já estão em ordem, termina
        // Evita passes de merge desnecessários quando quase tudo já está mesclado
        int already_sorted = 1;

        for (size_t i = 0; i + width < N; i += (width << 1)) {
            size_t mid = i + width;
            if (src[mid - 1] > src[mid]) { already_sorted = 0; break; }
        }

        if (already_sorted) break;

        int any_merged = 0;
        for (size_t i = 0; i < N; i += (width << 1)) {
            size_t left = i, mid = i + width, right = i + (width << 1);
            if (mid > N)
                mid = N;

            if (right > N) 
                right = N;

            // Se bloco direito vazio ou já em ordem, só copia
            if (mid >= right || src[mid - 1] <= src[mid]) {
                memcpy(&dst[left], &src[left], (right - left) * sizeof(int));
                continue;
            }

            // Merge estável src -> dst
            // Invariante: [left..mid) e [mid..right) estão ordenados em src
            size_t p = left, q = mid, k = left;
            while (p < mid && q < right) {
                if (src[p] <= src[q]) 
                    dst[k++] = src[p++];

                else 
                dst[k++] = src[q++];
            }

            // Copia o restante (um dos lados sempre esgota primeiro)
            while (p < mid) 
                dst[k++] = src[p++];

            while (q < right) 
                dst[k++] = src[q++];

            any_merged = 1;
        }

        if (!any_merged) break; // nada a mesclar neste passe

        // Próximo passe: troca src/dst (ping-pong). Evita cópias desnecessárias a cada nível.
        int *swap = src; src = dst; dst = swap;

        if (width > (N >> 1)) break;     // evita overflow ao dobrar
        width <<= 1;
    }

    // Se o resultado final ficou em tmp (src != array), copia de volta uma vez
    if (src != array)
        memcpy(array, src, N * sizeof(int));

    free(tmp);
}

// Restaura a propriedade de heap máximo a partir do índice i em um heap de tamanho n
static void sift_down(int *a, int n, int i) {
    // "Sift-down" clássico de heap máximo. Usa um cache do valor (v) para reduzir swaps.
    int v = a[i], half = n >> 1; // primeiros 'half' índices são nós internos

    while (i < half) {
        int l = (i << 1) + 1, r = l + 1;

        // escolhe o maior filho
        int j = (r < n && a[r] > a[l]) ? r : l;

        if (a[j] <= v) 
            break; // posição correta encontrada

        a[i] = a[j];
        i = j;
    }

    a[i] = v;
}

// Variante de sift-down que insere um valor 'v' começando na raiz (i)
// em um heap de tamanho n, evitando o swap no laço principal do sort.
static void sift_down_with_val(int *a, int n, int i, int v) {
    // Variante que evita a troca inicial entre raiz e último elemento.
    // 'v' é o valor que será afundado a partir da posição 'i'.
    int half = n >> 1;

    while (i < half) {
        int l = (i << 1) + 1, r = l + 1, j = (r < n && a[r] > a[l]) ? r : l;

        if (a[j] <= v) 
            break;
            
        a[i] = a[j];
        i = j;
    }

    a[i] = v;
}

// Heapsort hibridizado com insertion sort para tamanhos pequenos e cauda pequena
void algoritmo_id3_heapsort(int *array, int n) {
    if (n <= 1) return;
    
    // Fast-paths: já ordenado crescente ou totalmente decrescente
    if (is_sorted_asc(array, n))
        return;

    if (is_sorted_desc(array, n)) { 
        reverse_array(array, n); 
        return; 
    }

    // Cutoff para tamanhos pequenos: insertion sort é tipicamente melhor em cache
    if (n <= 32) {
        insertion_sort(array, 0, n - 1);
        return;
    }

    // Construção do heap (Floyd): O(n)
    for (int i = (n >> 1) - 1; i >= 0; --i) 
        sift_down(array, n, i);

    // Extração do máximo + restauração do heap: O(n log n)
    // Cutoff: termina com insertion sort quando a cauda for pequena
    for (int end = n - 1; end > 0; --end) {
        if (end < 32) { // ordena o prefixo restante de forma eficiente
            insertion_sort(array, 0, end);
            break;
        }

        int max = array[0], v = array[end];

        sift_down_with_val(array, end, 0, v);
        array[end] = max;
    }
}

int main() {
    int n, id_algoritmo;
    char tipo_sequencia[10];

    // Leitura: usa %9s para evitar overflow do buffer de 'tipo_sequencia'
    scanf("%d", &n);
    scanf("%9s", tipo_sequencia);
    scanf("%d", &id_algoritmo);

    // Evita overflow de size_t e acesso inválido no CRC quando n < 0
    if (n < 0) {
        fprintf(stderr, "Valor de n invalido.\n");
        return 1;
    }

    int *sequencia = (int *)malloc((size_t)n * sizeof(int));
    if (sequencia == NULL && n > 0) {
        fprintf(stderr, "Erro de alocacao de memoria.\n");
        return 1;
    }

    // Geração da sequência conforme requisitado
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

    // Seleção do algoritmo de ordenação
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

    // Hash de verificação (independente do algoritmo), útil para validar resultados em testes automatizados
    init_crc32(); // Inicializa a tabela de CRC32
    uint32_t saida = crc32(0, sequencia, (size_t)n * sizeof(int)); // Calcula o hash
    printf("%08X\n", saida); // Imprime a saída no formato hexadecimal

    // Libera recursos
    free(sequencia);

    return 0;
}