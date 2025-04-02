/*---------------------------------------------------------
PROGRAMA: cliente TAD ArvBin
-----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

typedef int ItemArv;
#include "itemInt.h"
#include "arvBin.h"


struct Nodo *busca_prof (Nodo *atual, int elemento) {
    if(atual == NULL){
        return NULL;
    }
    
    if(atual->item == elemento){
        return atual;
    }

    struct no *result;
    result = busca_prof(atual->esq, elemento);
    if(result){
        return result;
    }
    result = busca_prof(atual->dir, elemento);
    return result;
}

int somaChave(Nodo *atual) {
    if (atual == NULL) {
        return 0;
    }
    return atual->item + somaChave(atual->esq) + somaChave(atual->dir);
}

Nodo *busca_minimo(Nodo *atual) {
    if (atual == NULL) {
        return NULL;
    }

    Nodo *min_esq = busca_minimo(atual->esq);
    Nodo *min_dir = busca_minimo(atual->dir);

    Nodo *menor = atual;
    if (min_esq != NULL && min_esq->item < menor->item) {
        menor = min_esq;
    }
    if (min_dir != NULL && min_dir->item < menor->item) {
        menor = min_dir;
    }

    return menor;
}

void paiMaior(Nodo *atual) {
    if (atual == NULL) {
        return;
    }

    paiMaior(atual->esq);
    paiMaior(atual->dir);

    int maior = atual->item;
    if (atual->esq != NULL && atual->esq->item > maior) {
        maior = atual->esq->item;
    }
    if (atual->dir != NULL && atual->dir->item > maior) {
        maior = atual->dir->item;
    }

    if (atual->esq != NULL || atual->dir != NULL) {
        atual->item = maior;
    }
}

Nodo *dobraArvore(Nodo *atual) {
    if (atual == NULL) {
        return NULL;
    }
    // Cria um novo nó pai para o nó atual
    Nodo *novoPai = (Nodo *)malloc(sizeof(Nodo));
    if (novoPai == NULL) {
        return NULL; 
    }

    if(atual->item % 2 == 0){
      novoPai->item = atual->item + 1;
    }else{
      novoPai->item = atual->item - 1;
    }

    // O novo nó pai assume o lugar do nó atual na árvore
    if (atual->item % 2 == 0) {
        novoPai->esq = atual; // Nó atual se torna filho esquerdo
        novoPai->dir = NULL;
    } else {
        novoPai->dir = atual; // Nó atual se torna filho direito
        novoPai->esq = NULL;
    }

    atual->esq = dobraArvore(atual->esq);
    atual->dir = dobraArvore(atual->dir);

    return novoPai;
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
int main(int argc, char *argv[]){
  ArvBin arv;
  ItemArv v;
  
  criaArv( &arv );
  read( v );
  while( !eq( v, FIM )){
    arv= insereArv( v, arv );
    read( v );
  }
  printf( "\nArvore\n" );
  escreveArv( arv );

  freeArv( arv );
  return 0;
}
