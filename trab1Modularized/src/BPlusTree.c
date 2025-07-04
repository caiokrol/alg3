#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../include/BPlusTree.h"
#include "../include/BPlusTree.h"

//---------------------------------- Protótipos funções internas----------------------------------
/**
 * @brief Aloca memória para um novo nó da árvore.
 * @return Ponteiro para o novo nó criado.
 */
static No* criar_no();

/**
 * @brief Divide um nó que atingiu sua capacidade máxima de chaves.
 * @param arvore A árvore sendo modificada.
 * @param no O nó a ser dividido.
 */
static void dividir_no(BPlusTree *arvore, No *no);

/**
 * @brief Insere uma chave e um ponteiro em um nó pai após uma divisão (split).
 * @param arvore A árvore sendo modificada.
 * @param no_esquerdo O nó original que foi dividido.
 * @param chave A chave que será "promovida" ao pai.
 * @param no_direito O novo nó criado na divisão.
 */
static void inserir_no_pai(BPlusTree *arvore, No *no_esquerdo, int chave, No *no_direito);

/**
 * @brief Libera recursivamente os nós da árvore.
 * @param no O nó raiz da árvore (ou sub-árvore) a ser destruída.
 */
static void destruir_nos_recursivo(No *no);


//----------------------------------Funções definidas no .h----------------------------------
BPlusTree* criar_arvore_bplus(int ordem) {
    if (ordem > MAX_ORDER) {
        fprintf(stderr, "Erro: Ordem %d é maior que a máxima suportada (%d).\n", ordem, MAX_ORDER);
        return NULL;
    }
    BPlusTree *arvore = (BPlusTree*)malloc(sizeof(BPlusTree));
    if (!arvore) {
        perror("Falha ao alocar memória para a Árvore B+");
        exit(EXIT_FAILURE);
    }
    arvore->raiz = NULL;
    arvore->ordem = ordem;
    return arvore;
}



void destruir_arvore(BPlusTree *arvore) {
    if (arvore == NULL) return;
    destruir_nos_recursivo(arvore->raiz);
    free(arvore);
}


void inserir(BPlusTree *arvore, int chave, Carro *carro) {
    // Caso 1: A árvore está vazia.
    if (arvore->raiz == NULL) {
        arvore->raiz = criar_no();
        arvore->raiz->folha = true;
        arvore->raiz->chaves[0] = chave;
        arvore->raiz->ponteiros[0] = carro;
        arvore->raiz->num_chaves = 1;
        return;
    }

    // Caso 2: Encontra o nó folha correto para a inserção.
    No *no_atual = arvore->raiz;
    while (!no_atual->folha) {
        int i = 0;
        while (i < no_atual->num_chaves && chave >= no_atual->chaves[i]) {
            i++;
        }
        no_atual = (No*)no_atual->ponteiros[i];
    }

    // Agora `no_atual` é o nó folha onde a chave deve ser inserida.
    
    // Desloca as chaves e ponteiros existentes para abrir espaço para o novo par.
    int i = no_atual->num_chaves;
    while (i > 0 && no_atual->chaves[i - 1] > chave) {
        no_atual->chaves[i] = no_atual->chaves[i - 1];
        no_atual->ponteiros[i] = no_atual->ponteiros[i - 1];
        i--;
    }
    
    // Insere a nova chave e o ponteiro na posição correta.
    no_atual->chaves[i] = chave;
    no_atual->ponteiros[i] = carro;
    no_atual->num_chaves++;

    // Caso 3: Se o nó ficou superlotado, divide-o.
    // A ordem define o número MÁXIMO de ponteiros. O número máximo de chaves é ordem-1.
    // Se num_chaves == ordem, significa que estourou o limite.
    if (no_atual->num_chaves == arvore->ordem) {
        dividir_no(arvore, no_atual);
    }
}



Carro* buscar(BPlusTree *arvore, int chave) {
    if (arvore == NULL || arvore->raiz == NULL) return NULL;

    No *no_atual = arvore->raiz;
    while (!no_atual->folha) {
        int i = 0;
        while (i < no_atual->num_chaves && chave >= no_atual->chaves[i]) {
            i++;
        }
        no_atual = (No*)no_atual->ponteiros[i];
    }

    // Busca linear na folha
    for (int i = 0; i < no_atual->num_chaves; i++) {
        if (no_atual->chaves[i] == chave) {
            return (Carro*)no_atual->ponteiros[i];
        }
    }
    return NULL;
}


size_t tamanho_no_bplustree(BPlusTree* arvore, int ordem) {
    size_t sub_chaves_vazias = sizeof(int) * ((MAX_ORDER + 1) - ordem);
    size_t sub_ponteiros_vazios = sizeof(void *) * ((MAX_ORDER + 1) - ordem);
    return (sizeof(No) * ordem) - sub_chaves_vazias - sub_ponteiros_vazios;
}

//----------------------------------Funções internas (implementações)----------------------------------
No* criar_no() {
    No *novo_no = (No*)calloc(1, sizeof(No));
    if (!novo_no) {
        perror("Falha ao alocar memória para o nó");
        exit(EXIT_FAILURE);
    }
    return novo_no;
}


void dividir_no(BPlusTree *arvore, No *no) {
    int meio_idx = arvore->ordem / 2;
    No *irmao_direito = criar_no();
    irmao_direito->folha = no->folha;
    irmao_direito->pai = no->pai;
    int chave_promovida;

    if (!no->folha) { // Divisão de nó interno
        chave_promovida = no->chaves[meio_idx];

        // Copia a segunda metade das chaves e ponteiros para o irmão
        for (int i = meio_idx + 1; i < arvore->ordem; i++) {
            irmao_direito->chaves[i - (meio_idx + 1)] = no->chaves[i];
            irmao_direito->ponteiros[i - (meio_idx + 1)] = no->ponteiros[i];
        }
        irmao_direito->ponteiros[arvore->ordem - 1 - meio_idx] = no->ponteiros[arvore->ordem];
        irmao_direito->num_chaves = arvore->ordem - 1 - meio_idx;
        no->num_chaves = meio_idx;

        // Atualiza o ponteiro do pai para os filhos movidos
        for (int i = 0; i <= irmao_direito->num_chaves; i++) {
            No *filho = (No*)irmao_direito->ponteiros[i];
            if (filho) filho->pai = irmao_direito;
        }

    } else { // Divisão de nó folha
        // Copia a segunda metade das chaves e ponteiros de dados para o irmão
        for (int i = meio_idx; i < arvore->ordem; i++) {
            irmao_direito->chaves[i - meio_idx] = no->chaves[i];
            irmao_direito->ponteiros[i - meio_idx] = no->ponteiros[i];
        }
        irmao_direito->num_chaves = arvore->ordem - meio_idx;
        no->num_chaves = meio_idx;
        
        chave_promovida = irmao_direito->chaves[0];

        // Encadeia as folhas
        irmao_direito->prox_folha = no->prox_folha;
        no->prox_folha = irmao_direito;
    }

    inserir_no_pai(arvore, no, chave_promovida, irmao_direito);
}


void inserir_no_pai(BPlusTree *arvore, No *no_esquerdo, int chave, No *no_direito) {
    No *pai = no_esquerdo->pai;
    if (pai == NULL) {
        // Se não há pai, cria uma nova raiz.
        No *nova_raiz = criar_no();
        nova_raiz->folha = false;
        nova_raiz->chaves[0] = chave;
        nova_raiz->ponteiros[0] = no_esquerdo;
        nova_raiz->ponteiros[1] = no_direito;
        nova_raiz->num_chaves = 1;
        
        arvore->raiz = nova_raiz;
        no_esquerdo->pai = nova_raiz;
        no_direito->pai = nova_raiz;
        return;
    }

    // Se o pai tem espaço, apenas insere
    if (pai->num_chaves < arvore->ordem - 1) {
        int i = pai->num_chaves;
        while (i > 0 && pai->chaves[i - 1] > chave) {
            pai->chaves[i] = pai->chaves[i - 1];
            pai->ponteiros[i + 1] = pai->ponteiros[i];
            i--;
        }
        pai->chaves[i] = chave;
        pai->ponteiros[i + 1] = no_direito;
        pai->num_chaves++;
        no_direito->pai = pai;
    } else { // O pai está cheio, precisa dividir
        // Copia chaves e ponteiros para um array temporário maior
        
        int i = pai->num_chaves;
        while (i > 0 && pai->chaves[i - 1] > chave) {
            pai->chaves[i] = pai->chaves[i - 1];
            pai->ponteiros[i + 1] = pai->ponteiros[i];
            i--;
        }
        pai->chaves[i] = chave;
        pai->ponteiros[i+1] = no_direito;
        pai->num_chaves++;

        dividir_no(arvore, pai);
    }
}


void destruir_nos_recursivo(No *no) {
    if (no == NULL) return;
    if (!no->folha) {
        for (int i = 0; i <= no->num_chaves; i++) {
            destruir_nos_recursivo(no->ponteiros[i]);
        }
    }
    free(no);
}
