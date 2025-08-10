#include "contador.h"

    void contador(Contador *c) {
        c->valor = 0;
    }
    
    void incrementar(Contador *c) {
        c->valor++;
    }
    
    int valorAtual(const Contador *c) {
        return c->valor;
    }