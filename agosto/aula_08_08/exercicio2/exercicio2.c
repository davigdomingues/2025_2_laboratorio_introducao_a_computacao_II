#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char palavra[100];
    int deslocamento = 1;

    fgets(palavra, sizeof(palavra), stdin);
    palavra[strcspn(palavra, "\n")] = '\0';

    for (int i = 0; palavra[i] != '\0'; i++) {
        unsigned char caractere = palavra[i];

        if (caractere >= 32 && caractere <= 126)
            caractere = (caractere - 32 + deslocamento)%95 + 32;

        palavra[i] = caractere;
    }

    printf("%s", palavra);

    return 0;
}