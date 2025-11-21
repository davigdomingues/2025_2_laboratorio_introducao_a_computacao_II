/* 
Problema: dado conjunto A de N inteiros distintos e M consultas, cada consulta pergunta se existe par (Ai, Aj) com i!=j tal que Ai + Aj = k.
Estrutura: arvore binaria de busca (BST) para verificacao de existencia em O(log N) medio.
Fluxo:
1. Le N e insere cada valor na BST.
2. Para cada consulta k:
   - Percorre vetor A (N tentativas no pior caso).
   - Para cada Ai calcula need = k - Ai.
   - Garante que need cabe em int e que nao usa mesmo elemento (need != Ai).
   - Faz busca na BST.
Complexidade:
- Construir BST: O(N log N) medio, O(N^2) pior caso se ordenado.
- Cada consulta: O(N log N) medio, O(N^2) pior no degenerado.
Memoria: O(N) para vetor e O(N) para nós da arvore.
Vantagem: simples; Desvantagem: pode degradar sem balanceamento.
Entrada: N, N inteiros, M, M valores k (long long).
Saida: Para cada k imprime S se existe par, senao N.
Cuidados:
- Conversao segura de need para int somente dentro dos limites de int.
- Usa long long para evitar overflow na soma alvo.
*/
#include <stdio.h>
#include <stdlib.h>

// Estrutura de nó da BST: guarda chave e ponteiros para filhos esquerdo/direito
typedef struct Node {
    int key; // valor inteiro armazenado
    struct Node *l,*r; // l = filho esquerdo (menores), r = filho direito (maiores)
} Node;

// Insere chave na BST preservando a propriedade de ordenacao
Node* insert(Node*root,int key){
    if(!root){ // caso base: achou posicao nula
        root=(Node*)malloc(sizeof(Node)); // aloca novo nó
        root->key=key; // atribui valor
        root->l=root->r=NULL; // filhos inicialmente nulos
        return root; // retorna novo nó para encadear
    }

    if(key < root->key) // se menor, desce à esquerda
        root->l=insert(root->l,key);

    else if(key > root->key) // se maior, desce à direita
        root->r=insert(root->r,key);

    // valores iguais ignorados (distintos garantidos pela premissa do problema)
    return root; // retorna raiz (inalterada ou com subárvore ajustada)
}

// Busca iterativa: percorre descendo à esquerda/direita conforme comparação
int search(Node*root,int key){
    while(root){
        if(key==root->key) return 1; // achou
        root = key < root->key ? root->l // decide proximo nó
                               : root->r;
    }

    return 0; // nao encontrado
}

// Libera memoria da árvore via pos-ordem
void freeTree(Node*root){
    if(!root) 
        return;
        
    freeTree(root->l); // libera subárvore esquerda
    freeTree(root->r); // libera subárvore direita
    free(root); // libera nó atual
}

int main(){
    int N;
    if (scanf("%d",&N)!=1) 
        return 0; // leitura defensiva do tamanho
        
    int *A = (int*)malloc(sizeof(int)*N);   // vetor para iterar pares
    Node *root=NULL;                        // raiz inicial da BST

    for (int i=0;i<N;i++){
        scanf("%d",&A[i]);                  // lê elemento
        root=insert(root,A[i]);             // insere na BST
    }
    
    int M; 
    if (scanf("%d",&M)!=1) 
        return 0; // leitura defensiva do numero de consultas
    for (int q=0;q<M;q++){
        long long k; 
        scanf("%lld",&k); // alvo da soma (usa long long para evitar overflow)
        int ok=0; // flag de existencia de par

        for (int i=0;i<N && !ok;i++){ // percorre cada Ai enquanto não achou
            long long need = k - A[i]; // valor complementar desejado
            // garante indices distintos (need != Ai) e que cabe em int para busca
            if (need != A[i] && need >= -2147483648LL && need <= 2147483647LL)
                if (search(root,(int)need)) ok=1; // verifica existencia na BST
        }

        putchar(ok?'S':'N'); // imprime resultado da consulta
        putchar('\n');
    }

    free(A); // libera vetor
    freeTree(root); // libera BST
    return 0; // fim
}
