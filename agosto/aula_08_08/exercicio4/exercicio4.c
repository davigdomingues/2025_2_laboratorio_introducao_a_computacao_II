#include <stdio.h>
#include <stdbool.h>

enum Meses {
    janeiro = 1,
    fevereiro = 2,
    marco = 3,
    abril = 4,
    maio = 5,
    junho = 6,
    julho = 7,
    agosto = 8,
    setembro = 9,
    outubro = 10,
    novembro = 11,
    dezembro = 12
};

const char *nomesMeses[13] = { // meses codificados adequadamente para o UTF-8, via \x
    "", // posição 0 não usada
    "janeiro",
    "fevereiro",
    "mar\xC3\xA7o",
    "abril",
    "maio",
    "junho",
    "julho",
    "agosto",
    "setembro",
    "outubro",
    "novembro",
    "dezembro"
};

int totalDiasChecker(int mes, int ano, bool bissexto) {
    if (mes == abril || mes == junho || mes == setembro || mes == novembro)
        return 30;

    else if (mes == fevereiro)
        return bissexto ? 29 : 28;

    else
        return 31;
}

int main() {
    int dia, mes, ano;
    bool bissexto = false;

    scanf("%d %d %d", &dia, &mes, &ano);

    if (mes < 1 || mes > 12) {
        printf("M\xC3\xAAs inv\xC3\xA1lido.");
        return 1;
    }

    if ((ano % 400 == 0) || ((ano % 4 == 0) && (ano % 100 != 0))) {
        bissexto = true;
        printf("%d \xC3\xA9 bissexto.", ano);
    }

    else
        printf("%d n\xC3\xA3o \xC3\xA9 bissexto.", ano);

    int totalDiasMes = totalDiasChecker(mes, ano, bissexto);

    if (dia >= 1 && dia <= totalDiasMes)
        printf(" Existe o dia %d em %s.", dia, nomesMeses[mes]);

    else
        printf(" N\xC3\xA3o existe o dia %d em %s.", dia, nomesMeses[mes]);


    return 0;
}