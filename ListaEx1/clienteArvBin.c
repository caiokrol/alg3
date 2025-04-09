/*---------------------------------------------------------
PROGRAMA: cliente TAD ArvBin
-----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

typedef int ItemArv;
#include "itemInt.h"
#include "arvBin.h"



// Soma todos os valores (itens) da árvore binária
int somaChave(Nodo *atual) {
    if (atual == NULL) {
        return 0; // Se o nó for nulo, retorna 0 (caso base da recursão)
    }
    // Soma o valor do nó atual com as somas das subárvores esquerda e direita
    return atual->item + somaChave(atual->esq) + somaChave(atual->dir);
}

// Retorna o ponteiro para o nó com o menor valor na árvore
Nodo *valorMinimo(Nodo *atual) {
    if (atual == NULL) {
        return NULL; // Caso base: árvore vazia
    }

    // Busca recursivamente o menor valor nas subárvores esquerda e direita
    Nodo *min_esq = valorMinimo(atual->esq);
    Nodo *min_dir = valorMinimo(atual->dir);

    Nodo *menor = atual;

    // Compara com o menor da esquerda
    if (min_esq != NULL && min_esq->item < menor->item) {
        menor = min_esq;
    }
    // Compara com o menor da direita
    if (min_dir != NULL && min_dir->item < menor->item) {
        menor = min_dir;
    }

    return menor; // Retorna o ponteiro para o menor valor encontrado
}

// Substitui cada nó não-folha pelo maior valor entre ele e seus filhos
void paiMaior(Nodo *atual) {
    if (atual == NULL) {
        return; // Caso base
    }

    // Processa recursivamente os filhos primeiro
    paiMaior(atual->esq);
    paiMaior(atual->dir);

    int maior = atual->item;

    // Compara com o filho da esquerda
    if (atual->esq != NULL && atual->esq->item > maior) {
        maior = atual->esq->item;
    }

    // Compara com o filho da direita
    if (atual->dir != NULL && atual->dir->item > maior) {
        maior = atual->dir->item;
    }

    // Se o nó não for folha, atualiza seu valor para o maior entre ele e os filhos
    if (atual->esq != NULL || atual->dir != NULL) {
        atual->item = maior;
    }
}

// Cria um novo nó pai para cada nó da árvore, com valor ajustado
Nodo *dobraArvore(Nodo *atual) {
    if (atual == NULL) {
        return NULL; // Caso base: árvore vazia
    }

    // Processa primeiro os filhos recursivamente
    atual->esq = dobraArvore(atual->esq);
    atual->dir = dobraArvore(atual->dir);

    // Cria um novo nó que será o pai do nó atual
    Nodo *novoPai = (Nodo *)malloc(sizeof(Nodo));
    if (novoPai == NULL) return NULL; // Falha na alocação

    // Define o valor do novo pai: se for par, soma 1; se for ímpar, subtrai 1
    if (atual->item % 2 == 0)
        novoPai->item = atual->item + 1;
    else
        novoPai->item = atual->item - 1;

    // Se o valor original for par, o nó atual vai para a esquerda do novo pai
    if (atual->item % 2 == 0) {
        novoPai->esq = atual;
        novoPai->dir = NULL;
    } else { // Se for ímpar, vai para a direita
        novoPai->dir = atual;
        novoPai->esq = NULL;
    }

    return novoPai; // Retorna o novo pai como novo nó raiz dessa subárvore
}



void maiorNaRaiz(Nodo *atual) {
    if (atual == NULL) {
        return;
    }

    // Primeiro, ajustamos a árvore para que cada pai tenha o maior valor dos filhos
    paiMaior(atual);

    // Agora, garantimos que a raiz tenha o maior valor, trocando valores se necessário
    Nodo *maior = atual;
    Nodo *esq = atual->esq;
    Nodo *dir = atual->dir;

    if (esq != NULL && esq->item > maior->item) {
        maior = esq;
    }
    if (dir != NULL && dir->item > maior->item) {
        maior = dir;
    }

    // Se o maior valor não estiver na raiz, troca os valores e chama recursivamente
    if (maior != atual) {
        int temp = atual->item;
        atual->item = maior->item;
        maior->item = temp;
        maiorNaRaiz(maior);
    }
}

void ordenaPeloMaior(Nodo *atual) {
    if (atual == NULL) {
        return;
    }

    // Primeiro, aplicamos `maiorNaRaiz` para garantir que o maior valor esteja na raiz
    maiorNaRaiz(atual);

    // Depois, aplicamos recursivamente nos filhos para propagar a ordenação na árvore
    ordenaPeloMaior(atual->esq);
    ordenaPeloMaior(atual->dir);
}


/* ----------------------------------------------------- */
/* Programa: cria uma árvore binária e imprime a árvore
*/
int main(int argc, char *argv[]) {
    ArvBin arv;
    ItemArv v;
    
    // Criar árvore vazia
    criaArv(&arv);
    
    // Ler e inserir valores até encontrar FIM
    read(v);
    while (!eq(v, FIM)) {
        arv = insereArv(v, arv);
        read(v);
    }
    
    // Exibir árvore original
    printf("\nÁrvore original:\n");
    escreveArv(arv);
    
    // Testar a função paiMaior
    paiMaior(arv);
    printf("\nÁrvore após paiMaior:\n");
    escreveArv(arv);

    // Testar a função dobraArvore
    arv = dobraArvore(arv);
    printf("\nÁrvore após dobraArvore:\n");
    escreveArv(arv);

    // Testar a função maiorNaRaiz
    maiorNaRaiz(arv);
    printf("\nÁrvore após maiorNaRaiz:\n");
    escreveArv(arv);

    // Testar a função ordenaPeloMaior
    ordenaPeloMaior(arv);
    printf("\nÁrvore após ordenaPeloMaior:\n");
    escreveArv(arv);

    // Liberar memória
    freeArv(arv);

    return 0;
}

