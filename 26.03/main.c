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

//Cria um novo nó e retorna o ponteiro para ele
no_t *criar_no(int chave) {
    no_t *novo = (no_t *)malloc(sizeof(no_t));
    if (novo) {
        novo->chave = chave;
        novo->esq = novo->dir = NULL;
    }
    return novo;
}

//Insere na árvore já em ordem, sem se importar com o balanceamento, retorna o ponteiro para o nó inserido
no_t *inserir(no_t *raiz, int chave) {
    if (raiz == NULL)
        return criar_no(chave);
    
    if (chave < raiz->chave)
        raiz->esq = inserir(raiz->esq, chave);
    else if (chave > raiz->chave)
        raiz->dir = inserir(raiz->dir, chave);
    
    return raiz;
}
//libera a memória alocada para a árvore e seus nós
void liberar_arvore(no_t *raiz) {
    if (raiz) {
        liberar_arvore(raiz->esq);
        liberar_arvore(raiz->dir);
        free(raiz);
    }
}

//Imprime os elementos em ordem
void em_ordem(no_t *raiz) {
    if (raiz) {
        em_ordem(raiz->esq);
        printf("%d ", raiz->chave);
        em_ordem(raiz->dir);
    }
}

//Busca um elemento passado nos paramêtros e retorna o ponteiro para ele, utilizando recursão
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

//Busca um elemento na árvore sem utilizar recursão
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

//retorna a soma de todas as chaves da árvore
int somaChave(struct no *atual) {
    if (atual == NULL) {
        return 0;
    }
    return atual->chave + somaChave(atual->esq) + somaChave(atual->dir);
}

//Busca o menor valor de chave na árvore
struct no *busca_minimo(struct no *atual) {
    if (atual == NULL) {
        return NULL;
    }

    struct no *min_esq = busca_minimo(atual->esq);
    struct no *min_dir = busca_minimo(atual->dir);

    struct no *menor = atual;
    if (min_esq != NULL && min_esq->chave < menor->chave) {
        menor = min_esq;
    }
    if (min_dir != NULL && min_dir->chave < menor->chave) {
        menor = min_dir;
    }

    return menor;
}

//Rearranja a árvore para que os pais sempre sejam maiores que os filhos
void paiMaior(struct no *atual) {
    if (atual == NULL) {
        return;
    }

    paiMaior(atual->esq);
    paiMaior(atual->dir);

    int maior = atual->chave;
    if (atual->esq != NULL && atual->esq->chave > maior) {
        maior = atual->esq->chave;
    }
    if (atual->dir != NULL && atual->dir->chave > maior) {
        maior = atual->dir->chave;
    }

    if (atual->esq != NULL || atual->dir != NULL) {
        atual->chave = maior;
    }
}

//Dobra o tamanho da árvore utilizando os parametros especificados para pares e ímpares
struct no *dobraArvore(struct no *atual) {
    if (atual == NULL) {
        return NULL;
    }
    // Cria um novo nó pai para o nó atual
    struct no *novoPai = (struct no*)malloc(sizeof(struct no));
    if (novoPai == NULL) {
        return NULL; 
    }

    if(atual->chave % 2 == 0){
      novoPai->chave = atual->chave + 1;
    }else{
      novoPai->chave = atual->chave - 1;
    }

    // O novo nó pai assume o lugar do nó atual na árvore
    if (atual->chave % 2 == 0) {
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
//Coloca o maior elemento da árvore na raiz
void maiorNaRaiz(struct no *atual) {
    if (atual == NULL) {
        return;
    }

    paiMaior(atual);


    struct no *maior = atual;
    struct no *esq = atual->esq;
    struct no *dir = atual->dir;

    if (esq != NULL && esq->chave > maior->chave) {
        maior = esq;
    }
    if (dir != NULL && dir->chave > maior->chave) {
        maior = dir;
    }

    if (maior != atual) {
        int temp = atual->chave;
        atual->chave = maior->chave;
        maior->chave = temp;
        maiorNaRaiz(maior);
    }
}

//Rearranja os elementos para que estejam em ordem
void ordenaPeloMaior(struct no *atual) {
    if (atual == NULL) {
        return;
    }

    maiorNaRaiz(atual);

    ordenaPeloMaior(atual->esq);
    ordenaPeloMaior(atual->dir);
}


struct no *buscaBinaria(struct no *atual, int elemento){
    if(atual == NULL){
        return NULL;
    }

    if(atual->chave == elemento){
        return atual;
    }
    if(atual->chave > elemento){
        return buscaBinaria(atual->esq, elemento);
    }
    if(atual->chave < elemento){
        return buscaBinaria(atual->dir, elemento);
    }
    
}


int main() {
    no_t *arvore = NULL;
    int i, n = 10; // Número de elementos na árvores
    
    srand(time(NULL));
    
    // Gerar árvore aleatória com 10 elementos
    printf("Inserindo elementos na árvore:\n");
    for (i = 0; i < n; i++) {
        int chave = aleat(1, 100);
        printf("%d ", chave);
        arvore = inserir(arvore, chave);
    }
    printf("\n\nÁrvore original (em ordem):\n");
    em_ordem(arvore);
    printf("\n");
    
    // Testar busca em profundidade recursiva e iterativa
    int busca_valor = aleat(1, 100);
    printf("\nBuscando o valor %d na árvore (recursivo): %s\n", busca_valor, busca_prof(arvore, busca_valor) ? "Encontrado" : "Não encontrado"); //usei ternário por preguiça de fazer if
    printf("Buscando o valor %d na árvore (iterativo): %s\n", busca_valor, busca_profSR(arvore, busca_valor) ? "Encontrado" : "Não encontrado");
    printf("Buscando o valor %d na árvore (Binária): %s\n", busca_valor, buscaBinaria(arvore, busca_valor) ? "Encontrado" : "Não encontrado");
    
    // Testar soma das chaves
    printf("\nSoma de todas as chaves: %d\n", somaChave(arvore));
    
    // Testar busca do menor elemento
    no_t *menor = busca_minimo(arvore);
    printf("Menor elemento na árvore: %d\n", menor ? menor->chave : -1);
    
    // Aplicar e testar paiMaior
    paiMaior(arvore);
    printf("\nÁrvore após paiMaior (em ordem):\n");
    em_ordem(arvore);
    printf("\n");
    
    // Aplicar e testar dobraArvore
    arvore = dobraArvore(arvore);
    printf("\nÁrvore após dobraArvore (em ordem):\n");
    em_ordem(arvore);
    printf("\n");
    
    // Aplicar e testar maiorNaRaiz
    maiorNaRaiz(arvore);
    printf("\nÁrvore após maiorNaRaiz (em ordem):\n");
    em_ordem(arvore);
    printf("\n");
    
    // Aplicar e testar ordenaPeloMaior
    ordenaPeloMaior(arvore);
    printf("\nÁrvore após ordenaPeloMaior (em ordem):\n");
    em_ordem(arvore);
    printf("\n");
    
    // Liberar memória
    liberar_arvore(arvore);
    return 0;
}
