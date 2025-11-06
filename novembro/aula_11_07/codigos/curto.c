/*
 * Objetivo: Implementar algoritmos de busca binária, sequencial e por hashing
 * para localizar datas em um vetor.
 *
 * Otimização: A implementação de hashing utiliza endereçamento aberto
 * com sondagem linear em vez de encadeamento separado para evitar
 * múltiplas alocações de memória que degradam a performance.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Implementação de Tabela Hash com Endereçamento Aberto
 */

#define HASH_EMPTY 0L

typedef struct HashTable {
    int size;    // Tamanho da tabela
    int count;   // Número de elementos inseridos
    long *table; // Tabela de hash
} HashTable;

// Função de hash usando método da divisão
int hash_function(long key, int size) {
    return (int)(key % size + size) % size;
}

// Inicializa tabela hash
HashTable *hash_create(int size) {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    ht->size = size;
    ht->count = 0;
    ht->table = (long *)calloc(ht->size, sizeof(long));
    return ht;
}

// Insere elemento na tabela hash
void hash_insert(HashTable *ht, long key) {
    int index = hash_function(key, ht->size);
    
    // Sondagem linear para resolução de colisões
    while (ht->table[index] != HASH_EMPTY) {
        index = (index + 1) % ht->size;
    }
    
    ht->table[index] = key;
    ht->count++;
}

// Busca elemento na tabela hash
int hash_search(HashTable *ht, long key) {
    int index = hash_function(key, ht->size);
    int start_index = index;

    while (ht->table[index] != HASH_EMPTY) {
        if (ht->table[index] == key) {
            return 1;
        }
        
        index = (index + 1) % ht->size;
        
        if (index == start_index) {
            return 0; // Tabela percorrida completamente
        }
    }
    
    return 0; // Elemento não encontrado
}

// Libera memória da tabela hash
void hash_free(HashTable *ht) {
    if (ht == NULL) return;
    free(ht->table);
    free(ht);
}

/*
 * Algoritmo de Busca Sequencial
 * Complexidade: O(n)
 */
int sequential_search(long *date_vector, int N, long key) {
    for (int i = 0; i < N; i++) {
        if (date_vector[i] == key) {
            return 1;
        }
    }
    return 0;
}

/*
 * Algoritmo de Busca Binária
 * Complexidade: O(log n)
 * Pré-requisito: vetor ordenado
 */
int binary_search(long *date_vector, int N, long key) {
    int left = 0;
    int right = N - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;

        if (date_vector[mid] == key) {
            return 1;
        }

        if (date_vector[mid] < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return 0;
}

// Função de comparação para qsort
int compare_longs(const void *a, const void *b) {
    long val1 = *(const long *)a;
    long val2 = *(const long *)b;
    
    if (val1 < val2) return -1;
    if (val1 > val2) return 1;
    return 0;
}

// Converte data no formato "dd-mm-yyyy" para long "yyyymmdd"
long convert_date_to_long(char *date_str) {
    int d, m, y;
    sscanf(date_str, "%d-%d-%d", &d, &m, &y);
    return (long)y * 10000L + (long)m * 100L + (long)d;
}

int main() {
    int N;
    scanf("%d", &N);

    long *date_vector = (long *)malloc(N * sizeof(long));
    char date_buffer[12];

    // Leitura das datas do conjunto de dados
    for (int i = 0; i < N; i++) {
        scanf("%s", date_buffer);
        date_vector[i] = convert_date_to_long(date_buffer);
    }

    int Q;
    scanf("%d", &Q);

    long *search_vector = (long *)malloc(Q * sizeof(long));

    // Leitura das datas de consulta
    for (int i = 0; i < Q; i++) {
        scanf("%s", date_buffer);
        search_vector[i] = convert_date_to_long(date_buffer);
    }

    int algorithm_choice;
    scanf("%d", &algorithm_choice);

    int found;

    switch (algorithm_choice) {
        case 1: // Busca Binária
            qsort(date_vector, N, sizeof(long), compare_longs);
            
            for (int i = 0; i < Q; i++) {
                found = binary_search(date_vector, N, search_vector[i]);
                printf("%s\n", found ? "ENCONTRADA" : "NAO_ENCONTRADA");
            }
            break;

        case 2: // Busca com Hashing
            int table_size = (N > 1) ? (N * 2 + 1) : 3;
            HashTable *ht = hash_create(table_size);

            // Inserção dos elementos na tabela hash
            for (int i = 0; i < N; i++) {
                hash_insert(ht, date_vector[i]);
            }

            // Execução das consultas
            for (int i = 0; i < Q; i++) {
                found = hash_search(ht, search_vector[i]);
                printf("%s\n", found ? "ENCONTRADA" : "NAO_ENCONTRADA");
            }

            hash_free(ht);
            break;

        case 3: // Busca Sequencial
            for (int i = 0; i < Q; i++) {
                found = sequential_search(date_vector, N, search_vector[i]);
                printf("%s\n", found ? "ENCONTRADA" : "NAO_ENCONTRADA");
            }
            break;

        default:
            fprintf(stderr, "Erro: Algoritmo de busca inválido.\n");
            break;
    }

    free(date_vector);
    free(search_vector);

    return 0;
}