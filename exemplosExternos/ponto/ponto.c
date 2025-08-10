#include <stdio.h>
#include "ponto.h"

void criarPonto(Ponto *p, float x, float y) {
    p->abscissa = x;
    p->ordenada = y;
}

void alterarPonto(Ponto *p, float x, float y) {
    p->abscissa = x;
    p->ordenada = y;
}

void mostrarPonto(const Ponto *p) {
    printf("Coordenadas do ponto solicitado: (%.2f, %.2f)", p->abscissa, p->ordenada);
}