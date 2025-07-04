#ifndef UTIL_H
#define UTIL_H

#include "BPlusTree.h"
#include "Carro.h"

#define NUM_BUSCAS_A_REALIZAR 100

extern int chaves_para_busca[NUM_BUSCAS_A_REALIZAR];
/**
 * @brief Conta o número de linhas (registros) em um arquivo.
 * @param nome_arquivo O caminho para o arquivo.
 * @return O número de linhas contadas.
 */
long contar_registros(const char *nome_arquivo);

/**
 * @brief Carrega registros de um arquivo para um array de Carros já alocado.
 * @param carros_out Ponteiro para o array de Carro onde os dados serão armazenados.
 * @param num_a_carregar O número máximo de registros a serem lidos.
 * @return O número de registros efetivamente carregados.
 */
int carregar_registros(Carro *carros_out, long num_a_carregar);

/**
 * @brief Seleciona chaves aleatórias dos registros carregados para usar nos testes.
 * @param todos_os_carros Array com todos os carros.
 * @param num_registros O total de registros disponíveis para selecionar.
 */
void preparar_chaves_busca(const Carro *todos_os_carros, int num_registros);

/**
 * @brief Obtém o tamanho do bloco de disco do sistema de arquivos onde o caminho especificado está localizado.
 * @param path Caminho para o diretório ou arquivo cujo sistema de arquivos será consultado.
 * @return O tamanho do bloco em bytes, ou -1 em caso de erro.
 */
long get_block_size(const char *path);

#endif
