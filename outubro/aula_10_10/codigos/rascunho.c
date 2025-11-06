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
    - Mergesort (bottom-up iterativo com vetor auxiliar)
      O(n log n) no pior/médio/melhor, estável.
    - Heapsort (construção de Floyd + extrações sucessivas)
      O(n log n) no pior/médio/melhor; in-place, não estável.
*/

void algoritmo_id1_quicksort(int *array, int n);
void algoritmo_id2_mergesort(int *array, int n);
void algoritmo_id3_heapsort(int *array, int n);

// Troca simples de dois inteiros por referencia
static void swap_int(int *a, int *b) {
    int t = *a; 
    *a = *b; 
    *b = t;
}

// Insertion Sort in-place em [lo, hi]
// Bom desempenho para subarranjos pequenos (usado como cutoff no Quicksort)
static void insertion_sort(int *a, int lo, int hi) {
    // Complexidade O(m^2) no pior caso (m = hi - lo + 1); porem muito eficiente para subarranjos pequenos.
    // Util como "cutoff" para reduzir overhead de recursao/merges em algoritmos hibridos.
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

// Escolha de pivo por mediana de tres: a[lo], a[mid], a[hi]
// Reduz a chance de piores casos em entradas quase ordenadas
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
        if (hi - lo + 1 <= 32) { // cutoff reduz chamadas recursivas e melhora cache
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

// Interface do algoritmo Quicksort
void algoritmo_id1_quicksort(int *array, int n) {
    // Guarda de sanidade: nada a ordenar para n <= 1
    if (n > 1) 
        quicksort_impl(array, 0, n - 1);
}

// Funções auxiliares in-place para mescla estável sem vetor auxiliar (O(1) de espaço extra)

// Inverte o intervalo [first, last) in-place
static void reverse_range(int *a, size_t first, size_t last) {
    while (first < last) {
        --last;
        if (first >= last) break;
        swap_int(&a[first], &a[last]);
        ++first;
    }
}

// Rotaciona [first, middle) com [middle, last):
// elementos em [middle, last) vão para a frente, preservando a ordem relativa via três reversões
static void rotate_range(int *a, size_t first, size_t middle, size_t last) {
    if (first == middle || middle == last) return;
    reverse_range(a, first, middle);
    reverse_range(a, middle, last);
    reverse_range(a, first, last);
}

// lower_bound: primeiro índice em [first, last) tal que a[idx] >= valor
static size_t lower_bound_idx(const int *a, size_t first, size_t last, int value) {
    size_t count = last - first;
    while (count) {
        size_t step = count >> 1;
        size_t it = first + step;
        if (a[it] < value) {
            first = it + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

// upper_bound: primeiro índice em [first, last) tal que a[idx] > valor
static size_t upper_bound_idx(const int *a, size_t first, size_t last, int value) {
    size_t count = last - first;
    while (count) {
        size_t step = count >> 1;
        size_t it = first + step;
        if (a[it] <= value) {
            first = it + 1;
            count -= step + 1;
        } else {
            count = step;
        }
    }
    return first;
}

// Mescla estável in-place de dois subintervalos ordenados adjacentes [first, mid) e [mid, last)
static void inplace_merge_sym(int *a, size_t first, size_t mid, size_t last) {
    if (first >= mid || mid >= last) return;
    // Já está ordenado na fronteira entre os blocos
    if (a[mid - 1] <= a[mid]) return;

    if ((mid - first) <= (last - mid)) {
        // Lado esquerdo menor: divide à esquerda, encontra posição na direita, rotaciona e recursa
        size_t mid1 = first + ((mid - first) >> 1);
        int val = a[mid1];
        size_t mid2 = lower_bound_idx(a, mid, last, val);
        size_t new_mid = mid1 + (mid2 - mid);
        rotate_range(a, mid1, mid, mid2);
        inplace_merge_sym(a, first, mid1, new_mid);
        inplace_merge_sym(a, new_mid, mid2, last);
    } else {
        // Lado direito menor: divide à direita, encontra posição na esquerda, rotaciona e recursa
        size_t mid2 = mid + ((last - mid) >> 1);
        int val = a[mid2];
        size_t mid1 = upper_bound_idx(a, first, mid, val);
        size_t new_mid = mid1 + (mid2 - mid);
        rotate_range(a, mid1, mid, mid2);
        inplace_merge_sym(a, first, mid1, new_mid);
        inplace_merge_sym(a, new_mid, mid2, last);
    }
}

// Mergesort iterativo (bottom-up) com vetor auxiliar para O(n log n) garantido
void algoritmo_id2_mergesort(int *array, int n) {
    if (n <= 1) return;

    size_t N = (size_t)n;
    
    // Aloca vetor auxiliar
    int *tmp = (int *)malloc(N * sizeof(int));
    if (tmp == NULL) {
        fprintf(stderr, "Erro ao alocar memoria auxiliar para mergesort.\n");
        exit(1);
    }

    // Mergesort bottom-up com merge tradicional usando vetor auxiliar
    for (size_t width = 1; width < N; width <<= 1) {
        for (size_t i = 0; i < N; i += (width << 1)) {
            size_t left  = i;
            size_t mid   = i + width;        if (mid > N)   mid = N;
            size_t right = i + (width << 1); if (right > N) right = N;

            if (mid < right) {
                // Merge usando vetor auxiliar
                size_t l = left, r = mid, k = left;
                
                // Copia para auxiliar
                for (size_t j = left; j < right; j++)
                    tmp[j] = array[j];
                
                // Merge de volta para array
                while (l < mid && r < right) {
                    if (tmp[l] <= tmp[r])
                        array[k++] = tmp[l++];
                    else
                        array[k++] = tmp[r++];
                }
                
                while (l < mid)
                    array[k++] = tmp[l++];
                
                while (r < right)
                    array[k++] = tmp[r++];
            }
        }
    }
    
    free(tmp);
}

// Restaura a propriedade de heap máximo a partir do índice i em um heap de tamanho n
// Otimizações: restrict + menos acessos ao array ao escolher o maior filho
static void sift_down(int *restrict a, int n, int i) {
    int v = a[i];
    int half = n >> 1; // nós internos estão em [0, half)

    while (i < half) {
        int l = (i << 1) + 1;
        int r = l + 1;

        // Seleciona o maior filho com leituras minimizadas
        int j = l;
        int child = a[l];
        if (r < n) {
            int right_val = a[r];
            if (right_val > child) {
                j = r;
                child = right_val;
            }
        }

        if (child <= v) break;

        a[i] = child;
        i = j;
    }

    a[i] = v;
}

// Variante que "afunda" o valor v a partir da raiz i sem swap inicial
// Otimizações: restrict + seleção de filho com menos leituras
static void sift_down_with_val(int *restrict a, int n, int i, int v) {
    int half = n >> 1;

    while (i < half) {
        int l = (i << 1) + 1;
        int r = l + 1;

        int j = l;
        int child = a[l];
        if (r < n) {
            int right_val = a[r];
            if (right_val > child) {
                j = r;
                child = right_val;
            }
        }

        if (child <= v) break;

        a[i] = child;
        i = j;
    }

    a[i] = v;
}

// Heapsort puro (in-place) com rotinas sift-down otimizadas
void algoritmo_id3_heapsort(int *restrict array, int n) {
    if (n <= 1) return;

    for (int i = (n >> 1) - 1; i >= 0; --i)
        sift_down(array, n, i);

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

    // Geracao da sequencia conforme requisitada
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
            algoritmo_id2_mergesort(sequencia, n);
            break;

        case 3:
            algoritmo_id3_heapsort(sequencia, n);
            break;

        default:
            printf("Algoritmo com ID %d nao implementado.\n", id_algoritmo);
            free(sequencia);
            return 1;
    }

    // Hash de verificacao (independente do algoritmo), util para validar resultados em testes automatizados
    init_crc32(); // Inicializa a tabela de CRC32
    uint32_t saida = crc32(0, sequencia, (size_t)n * sizeof(int)); // Calcula o hash
    printf("%08X\n", saida); // Imprime a saida no formato hexadecimal

    // Libera recursos
    free(sequencia);

    return 0;
}