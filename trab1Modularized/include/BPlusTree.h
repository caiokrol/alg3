#ifndef BPLUSTREE_H
#define BPLUSTREE_H

#include <stdbool.h>
#include "Carro.h"

#define MAX_ORDER 41

typedef struct No {
    bool folha;
    int num_chaves;
    int chaves[MAX_ORDER + 1];
    void *ponteiros[MAX_ORDER + 1];
    struct No *prox_folha;
    struct No *pai;
} No;

typedef struct {
    No *raiz;
    int ordem;
} BPlusTree;

/**
 * @brief Aloca memória e inicializa uma nova Árvore B+.
 * @param ordem A ordem da árvore a ser criada.
 * @return Ponteiro para a nova árvore.
 */
BPlusTree* criar_arvore_bplus(int ordem);

/**
 * @brief Libera toda a memória utilizada por uma Árvore B+.
 * @param arvore A árvore a ser destruída.
 */
void destruir_arvore(BPlusTree *arvore);

/**
 * @brief Insere um novo par (chave, valor) na árvore B+.
 * @param arvore A árvore sendo modificada.
 * @param chave O renavam do carro.
 * @param carro Um ponteiro para a estrutura `Carro` a ser inserida.
 */
void inserir(BPlusTree *arvore, int chave, Carro *carro);

/**
 * @brief Busca por uma chave na árvore.
 * @param arvore A Árvore B+ onde a busca será realizada.
 * @param chave A chave (renavam) a ser encontrada.
 * @return Ponteiro para a estrutura `Carro` se encontrada, ou `NULL` caso contrário.
 */
Carro* buscar(BPlusTree *arvore, int chave);

/**
 * @brief Calcula o tamanho em bytes de um nó da árvore B+ para uma dada ordem.
 * @param arvore Ponteiro para a árvore B+ (pode ser usado para acessar configurações específicas da árvore, se necessário).
 * @param ordem A ordem da árvore B+ (quantidade máxima de chaves por nó).
 * @return O tamanho estimado, em bytes, de um único nó da árvore B+.
 */
size_t tamanho_no_bplustree(BPlusTree* arvore, int ordem);
#endif
