#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    int totalAlunos, totalAprovados = 0;
    scanf("%d", &totalAlunos);

    if (totalAlunos < 1)
        return 0;

    float notasAlunos[totalAlunos], maior = 0.0, soma = 0.0;

    for (int i = 0; i < totalAlunos; i++){
        scanf("%f", &notasAlunos[i]);

        if (notasAlunos[i] > 10 || notasAlunos[i] < 0) {
            printf("Nota fora do intervalo.");
            return 0;
        }

        if (notasAlunos[i] > maior)
            maior = notasAlunos[i];

        soma += notasAlunos[i];

        if (notasAlunos[i] >= 5.0)
            totalAprovados++;

    }

    printf("Foram %d aprovado(s). Maior nota = %.2f. Nota m\u00E9dia = %.2f.", totalAprovados, maior, (soma/(float)totalAlunos));

    return 0;
}
