#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estruturas e funções para implementação de tabela hash

// Nó da lista encadeada para tratamento de colisões
typedef struct Node {
    long date_key;
    struct Node *next;
} Node;

// Estrutura da tabela hash
typedef struct HashTable {
    int size;
    Node **table;
    // pool para evitar malloc por inserção
    Node *pool;
    int pool_index;
    int pool_capacity;
} HashTable;

// Função hash usando método da divisão
int hash_function(long key, int size) {
    // Garante resultado positivo
    return (int)(key % size + size) % size;
}

// Cria e inicializa a tabela hash (com pool para N elementos)
HashTable *hash_create(int size, int capacity) {
    HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
    ht->size = size;
    ht->table = (Node **)calloc(size, sizeof(Node *));
    ht->pool_capacity = capacity > 0 ? capacity : 1;
    ht->pool = (Node *)malloc(sizeof(Node) * ht->pool_capacity);
    ht->pool_index = 0;
    return ht;
}

// Insere uma chave na tabela hash (usa pool ao invés de malloc por nó)
void hash_insert(HashTable *ht, long key) {
    int index = hash_function(key, ht->size);
    Node *newNode = &ht->pool[ht->pool_index++]; // assume capacidade suficiente
    newNode->date_key = key;
    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

// Busca uma chave na tabela hash
int hash_search(HashTable *ht, long key) {
    int index = hash_function(key, ht->size);
    
    Node *current = ht->table[index];
    while (current != NULL) {
        if (current->date_key == key) {
            return 1; // encontrada
        }
        current = current->next;
    }
    return 0; // não encontrada
}

// Libera toda a memória da tabela hash
void hash_free(HashTable *ht) {
    if (ht == NULL) return;
    // nós pertencem ao pool; não liberar individualmente
    free(ht->table);
    free(ht->pool);
    free(ht);
}

// Busca sequencial no vetor
int sequential_search(long *date_vector, int N, long key) {
    for (int i = 0; i < N; i++) {
        if (date_vector[i] == key) {
            return 1; // encontrada
        }
    }
    return 0; // não encontrada
}

// Busca binária (requer vetor ordenado)
int binary_search(long *date_vector, int N, long key) {
    int left = 0;
    int right = N - 1;
    
    while (left <= right) {
        int mid = left + (right - left) / 2;
        
        if (date_vector[mid] == key) {
            return 1; // encontrada
        }
        
        if (date_vector[mid] < key) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return 0; // não encontrada
}

// Função de comparação para qsort
int compare_longs(const void *a, const void *b) {
    long val1 = *(const long *)a;
    long val2 = *(const long *)b;
    if (val1 < val2) return -1;
    if (val1 > val2) return 1;
    return 0;
}

// Converte data "dd-mm-yyyy" para formato long "yyyymmdd"
long convert_date_to_long(char *date_str) {
    int d, m, y;
    sscanf(date_str, "%d-%d-%d", &d, &m, &y);
    return (long)y * 10000L + (long)m * 100L + (long)d;
}

// Funções auxiliares para escolher tamanho primo da tabela
static int is_prime(int x) {
    if (x <= 1) return 0;
    if (x <= 3) return 1;
    if (x % 2 == 0 || x % 3 == 0) return 0;
    for (int i = 5; 1LL * i * i <= x; i += 6) {
        if (x % i == 0 || x % (i + 2) == 0) return 0;
    }
    return 1;
}
static int next_prime(int x) {
    if (x <= 2) return 2;
    if (x % 2 == 0) x++;
    while (!is_prime(x)) x += 2;
    return x;
}

int main() {
    int N;
    scanf("%d", &N);
    
    // Aloca vetor para as datas
    long *date_vector = (long *)malloc(N * sizeof(long));
    char date_buffer[12];

    // Lê as N datas
    for (int i = 0; i < N; i++) {
        scanf("%s", date_buffer);
        date_vector[i] = convert_date_to_long(date_buffer);
    }
    
    int Q;
    scanf("%d", &Q);
    
    // Aloca vetor para as consultas
    long *search_vector = (long *)malloc(Q * sizeof(long));
    
    // Lê as datas de busca
    for (int i = 0; i < Q; i++) {
        scanf("%s", date_buffer);
        search_vector[i] = convert_date_to_long(date_buffer);
    }
    
    int algorithm_choice;
    scanf("%d", &algorithm_choice);

    int found;

    switch (algorithm_choice) {
        case 1: // Busca binária
            qsort(date_vector, N, sizeof(long), compare_longs);
            for (int i = 0; i < Q; i++) {
                found = binary_search(date_vector, N, search_vector[i]);
                puts(found ? "ENCONTRADA" : "NAO_ENCONTRADA");
            }
            break;

        case 2: { // Busca com hash
            int base = (N > 0) ? (2 * N) : 1;
            int table_size = next_prime(base);
            HashTable *ht = hash_create(table_size, N > 0 ? N : 1);

            // Insere todas as datas na tabela hash
            for (int i = 0; i < N; i++) {
                hash_insert(ht, date_vector[i]);
            }

            // Realiza as buscas
            for (int i = 0; i < Q; i++) {
                found = hash_search(ht, search_vector[i]);
                puts(found ? "ENCONTRADA" : "NAO_ENCONTRADA");
            }

            hash_free(ht);
            break;
        }

        case 3: // Busca sequencial
            for (int i = 0; i < Q; i++) {
                found = sequential_search(date_vector, N, search_vector[i]);
                puts(found ? "ENCONTRADA" : "NAO_ENCONTRADA");
            }
            break;

        default:
            fprintf(stderr, "Erro: Algoritmo de busca desconhecido.\n");
            break;
    }

    free(date_vector);
    free(search_vector);
    
    return 0;
}