#include <stdio.h>
#include "contador.h"

int main() {
    Contador c;
    int adicionador, i = 0;
    
    contador(&c);
    
    printf("Deseja adicionar quanto no contador? ");
    scanf("%d", &adicionador);
    
    while (i < adicionador) {
        incrementar(&c);
        i++;
    }
    
    printf("Valor atual do contador: %d", valorAtual(&c));
    
    return 0;
}