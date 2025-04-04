#ifndef DICIONARIO_H
#define DICIONARIO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAM_ALFABETO 26
#define MAX_RESULTADOS 20
#define MAX_DISTANCIA 3

// Estrutura do nó da TRIE
typedef struct TrieNode {
    struct TrieNode *filhos[TAM_ALFABETO]; // Ponteiros para os filhos (próximas letras)
    int eh_fim; // Indica se é o fim de uma palavra
} TrieNode;

// Estrutura da TRIE
typedef struct {
    TrieNode *raiz; // Ponteiro para a raiz da TRIE
} Trie;

// Cria um novo nó da TRIE
TrieNode *criar_no() {
    TrieNode *no = (TrieNode *)malloc(sizeof(TrieNode));
    if (no) {
        no->eh_fim = 0;
        for (int i = 0; i < TAM_ALFABETO; i++)
            no->filhos[i] = NULL;
    }
    return no;
}

// Cria uma nova TRIE e retorna um ponteiro para ela
Trie *criar_trie() {
    Trie *trie = (Trie *)malloc(sizeof(Trie));
    if (trie)
        trie->raiz = criar_no();
    return trie;
}

// Insere uma palavra na TRIE
// Percorre a árvore e cria os nós necessários para armazenar a palavra
void inserir_palavra(Trie *trie, const char *palavra) {
    TrieNode *atual = trie->raiz;
    while (*palavra) {
        char c = tolower(*palavra);
        if (c >= 'a' && c <= 'z') {
            int indice = c - 'a';
            if (!atual->filhos[indice])
                atual->filhos[indice] = criar_no();
            atual = atual->filhos[indice];
        }
        palavra++;
    }
    atual->eh_fim = 1;
}

// Busca palavras na TRIE dentro de um limite de distância de edição
// Utiliza recursão para comparar a palavra de entrada com palavras armazenadas na TRIE
// Armazena os resultados em um array de strings
int busca_edicao(TrieNode *no, const char *palavra, int limite_distancia, char *buffer, int profundidade, char resultados[MAX_RESULTADOS][100], int *contador);

// Carrega palavras de um arquivo de texto e as insere na TRIE
void carregar_dicionario(Trie *trie, const char *arquivo);

// Processa as consultas do arquivo e busca palavras na TRIE dentro da distância especificada
void processar_consultas(Trie *trie, const char *arquivo);

// Libera toda a memória alocada pela TRIE de forma recursiva
void liberar_trie(TrieNode *no);

#endif // DICIONARIO_H
