#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Cidade {
    char nomeCidade[30];
    int populacao;
};

int main() {
    int n;
    scanf("%d", &n);
    getchar();

    if (n > 10 || n < 2){
        printf("n fora do intervalo.");
        return 0;
    }

    struct Cidade cidade[n];

    for (int i = 0; i < n; i++) {
        scanf(" %[^\r]s", cidade[i].nomeCidade);
        scanf(" %d", &cidade[i].populacao);
        getchar();
    }

    int maior = cidade[0].populacao, indiceMaior;
    for (int i = 0; i < n; i++) {
        if (cidade[i].populacao >  maior){
            indiceMaior = i;
            maior = cidade[i].populacao;
        }
    }

    printf("%s %d", cidade[indiceMaior].nomeCidade, cidade[indiceMaior].populacao);

    return 0;
}