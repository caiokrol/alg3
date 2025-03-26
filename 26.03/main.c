#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct no {
    int chave;
    struct no *esq;
    struct no *dir;
} no_t;

typedef struct pilha {
    no_t *no;
    struct pilha *prox;
} pilha_t;

int aleat(int min, int max) {
  return (rand() % (max - min + 1)) + min;
}

no_t *criar_no(int chave) {
    no_t *novo = (no_t *)malloc(sizeof(no_t));
    if (novo) {
        novo->chave = chave;
        novo->esq = novo->dir = NULL;
    }
    return novo;
}

no_t *inserir(no_t *raiz, int chave) {
    if (raiz == NULL)
        return criar_no(chave);
    
    if (chave < raiz->chave)
        raiz->esq = inserir(raiz->esq, chave);
    else if (chave > raiz->chave)
        raiz->dir = inserir(raiz->dir, chave);
    
    return raiz;
}

void liberar_arvore(no_t *raiz) {
    if (raiz) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        free(raiz);
    }
}

void em_ordem(no_t *raiz) {
    if (raiz) {
        em_ordem(raiz->esq);
        printf("%d ", raiz->chave);
        em_ordem(raiz->dir);
    }
}

struct no *busca_prof (no_t *atual, int elemento) {
    if(atual == NULL){
        return NULL;
    }
    
    if(atual->chave == elemento){
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

// Função para empilhar um nó
void empilhar(pilha_t **topo, no_t *no) {
    pilha_t *novo = (pilha_t *)malloc(sizeof(pilha_t));
    if (!novo) return;
    novo->no = no;
    novo->prox = *topo;
    *topo = novo;
}

// Função para desempilhar um nó
no_t *desempilhar(pilha_t **topo) {
    if (*topo == NULL) return NULL;
    pilha_t *temp = *topo;
    no_t *no = temp->no;
    *topo = temp->prox;
    free(temp);
    return no;
}

no_t *busca_profSR(no_t *atual, int elemento) {
    if (atual == NULL) return NULL;

    pilha_t *pilha = NULL;
    empilhar(&pilha, atual);

    while (pilha) {
        no_t *no = desempilhar(&pilha);

        if (no->chave == elemento) {
            return no; // Elemento encontrado
        }

        if (no->dir) empilhar(&pilha, no->dir); // Empilha primeiro a direita
        if (no->esq) empilhar(&pilha, no->esq); // Depois a esquerda
    }
    free(pilha);
    return NULL; // Elemento não encontrado
}



int main() {
    no_t *raiz = NULL;
    srand(time(NULL));

    int numDeItens = aleat(10,1000);
    for(int i = 0; i < numDeItens; i++){
        raiz = inserir(raiz, aleat(0, 1000));
    }
    
    int elemento;
    printf("Digite o elemento a ser buscado: ");
    scanf("%d", &elemento);
    if(busca_profSR(raiz, elemento) == NULL){
        printf("\nElemento %d não encontrado na arvore", elemento);
    } else {
        printf("\nElemento %d encontrado na lista", elemento);
    }

    printf("\nElementos em ordem: ");
    em_ordem(raiz);
    printf("\n");
    
    
    liberar_arvore(raiz);
    return 0;
}
