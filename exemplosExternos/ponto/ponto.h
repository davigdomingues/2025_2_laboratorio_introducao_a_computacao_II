#ifndef PONTO_H
#define PONTO_H

    typedef struct {
        float abscissa;
        float ordenada;
    } Ponto;
    
    void criarPonto(Ponto *p, float abscissa, float ordenada);
    void alterarPonto(Ponto *p, float abscissa, float ordenada);
    void mostrarPonto(const Ponto *p);
    
#endif