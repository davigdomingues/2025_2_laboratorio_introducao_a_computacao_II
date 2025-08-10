#ifndef CONTADOR_H
#define CONTADOR_H
    
    typedef struct {
        int valor;
    } Contador;
    
    void contador(Contador *c);
    void incrementar(Contador *c);
    int valorAtual(const Contador *c);
    
#endif