#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "util.h"

// Helper para imprimir long long sem printf %lld
static void print_ll(long long x) {
    // Trata números negativos
    if (x < 0) { 
        putchar('-'); 
        x = -x; 
    }

    char buf[32]; // Buffer suficiente para armazenar o número

    // Preenche o buffer com os dígitos em ordem reversa
    int i = 0;
    do {
        buf[i++] = (char)('0' + (int)(x % 10));
        x /= 10;
    } while (x);

    // Imprime os dígitos na ordem correta
    while (i--)
        putchar(buf[i]);
}

// Verifica se o vetor está ordenado em ordem crescente
bool is_sorted(int *v, int n) {
    for (int i = 1; i < n; i++)
        if (v[i - 1] > v[i]) 
            return false;

    return true;
}

// Versão do PDF (necessária para passar no runcodes)
void shuffle(int *v, int n, int *seed) {
    for (int i = n - 1; i >= 1; i--) {
        int j = get_random(seed, i) - 1, temp = v[i]; 
        v[i] = v[j];
        v[j] = temp;
    }
}

int main() {
    int n, seed = 12345;
    scanf("%d", &n);
    int *v = (int *)malloc(n * sizeof(int));

    // Preenche o vetor com números aleatórios entre 1 e n
    for (int i = 0; i < n; i++) 
        v[i] = get_random(&seed, n);

    long long count = 0;

    // Aplica o bogosort (ordenação por permutação aleatória)
    while (!is_sorted(v, n)) {
        shuffle(v, n, &seed);
        count++;
    }

    // Imprime o número de permutações realizadas
    print_ll(count);
    putchar('\n');

    // Imprime o vetor ordenado
    for (int i = 0; i < n; i++)
        printf("%d%s", v[i], (i == n - 1) ? "" : " ");
    printf("\n");

    free(v);
    return 0;
}