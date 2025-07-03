#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../include/Util.h"
#include "../include/Carro.h"
#include <sys/resource.h>

// Array global usado para armazenar chaves aleatórias de busca
int chaves_para_busca[NUM_BUSCAS_A_REALIZAR];

long contar_registros(const char *nome_arquivo) {
    FILE *f = fopen(nome_arquivo, "r");
    if (!f) return 0;
    long count = 0;
    int ch;
    while (EOF != (ch = fgetc(f))) {
        if (ch == '\n') {
            count++;
        }
    }
    // Adiciona 1 se o arquivo não terminar com \n mas tiver conteúdo
    if (ftell(f) > 0 && count == 0) count = 1;
    fclose(f);
    return count;
}


int carregar_registros(Carro *carros_out, long num_a_carregar) {
    FILE *f = fopen("docs/registros.txt", "r");
    if (!f) {
        perror("Erro: 'registros.txt' não encontrado. Execute o gerador primeiro");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    char formato_scanf[100];
    sprintf(formato_scanf, "%%d;%%%d[^;];%%d;%%%d[^\n]\n", MAX_MODELO_LEN - 1, MAX_COR_LEN - 1);
    
    while (i < num_a_carregar && fscanf(f, formato_scanf,
        &carros_out[i].renavam,
        carros_out[i].modelo,
        &carros_out[i].ano,
        &carros_out[i].cor) == 4) {
        i++;
    }
    fclose(f);
    return i;
}


void preparar_chaves_busca(const Carro *todos_os_carros, int num_registros) {
    srand(time(NULL));
    for (int i = 0; i < NUM_BUSCAS_A_REALIZAR; i++) {
        int random_index = rand() % num_registros;
        chaves_para_busca[i] = todos_os_carros[random_index].renavam;
    }
}