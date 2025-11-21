/* 
Problema: verificar para cada k se existe par distinto cuja soma = k.
Abordagem: ordenar vetor e para cada Ai fazer busca binaria de need = k - Ai em subintervalo (i+1..N-1).
Otimizacao: break se need < Ai para evitar pares repetidos e manter indices distintos (como dois ponteiros implicitos).
Complexidade:
- Ordenacao: O(N log N).
- Cada consulta: O(N log N) (N buscas binarias). Total M * N log N.
Memoria: O(N).
Entrada: N, N inteiros, M consultas k.
Saida: S ou N por linha.
Cuidados:
- Usa long long para k e need para evitar overflow na subtracao.
- Garante distinct indices: busca inicia em i+1 e condicao need < Ai habilita corte precoce.
*/
#include <stdio.h>
#include <stdlib.h>

// Comparador para qsort: retorna negativo, zero ou positivo conforme ordenacao
static int cmpInt(const void *a,const void *b){
    int A = *(const int*)a, B = *(const int*)b;
    return (A>B)-(A<B);         // forma sucinta: 1,0,-1
}

int main(){
    int N;
    if (scanf("%d",&N)!=1) 
        return 0; // leitura defensiva

    int *A = (int*)malloc(sizeof(int)*N);  // aloca vetor
    for (int i=0;i<N;i++) 
        scanf("%d",&A[i]); // lê elementos
    qsort(A,N,sizeof(int),cmpInt); // ordena para permitir busca binaria

    int M; scanf("%d",&M); // numero de consultas
    for (int q=0;q<M;q++){
        long long k; 
        scanf("%lld",&k); // alvo da soma

        int found = 0; // flag resultado
        for (int i=0;i<N && !found;i++){ // percorre cada Ai
            long long need = k - A[i]; // complementar
            if (need < A[i]) break; // corte precoce: pares duplicados reversos

            // busca binaria de need em intervalo (i+1..N-1) garantindo indices distintos
            int l=i+1,r=N-1;
            while(l<=r){
                int m=(l+r)/2;
                if (A[m]==need){ found=1; break; } // achou
                if (A[m]<need) l=m+1; else r=m-1; // ajusta limites
            }
        }

        putchar(found?'S':'N'); // imprime resultado consulta
        putchar('\n');
    }
    
    free(A); // libera vetor
    return 0;
}
