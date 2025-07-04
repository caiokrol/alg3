#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include "../include/Carro.h"
#include "../include/BPlusTree.h"
#include "../include/Util.h"
#include <sys/resource.h>


// Configurações dos Registros de Carros
#define MAX_MODELO_LEN 50   // Tamanho máximo para a string do modelo do carro.
#define MAX_COR_LEN 30      // Tamanho máximo para a string da cor do carro.


int main() {
    printf("Contando registros no arquivo...\n");
    long total_registros_no_arquivo = contar_registros("docs/registros.txt");
    if (total_registros_no_arquivo == 0) {
        printf("Nenhum registro encontrado em 'registros.txt'. Abortando.\n");
        return 1;
    }
    printf("%ld registros encontrados.\n", total_registros_no_arquivo);
    
    printf("Alocando memória para os registros...\n");
    Carro *todos_os_carros = malloc(total_registros_no_arquivo * sizeof(Carro));
    if (!todos_os_carros) {
        perror("Falha ao alocar memória principal para os registros");
        return EXIT_FAILURE;
    }

    printf("Carregando registros para a memória...\n");
    int total_carregado = carregar_registros(todos_os_carros, total_registros_no_arquivo);

    // Cenários de teste
    int ordens_para_testar[] = {5, 20 ,50, 150, 220, 300, 400, 800};
    int num_ordens = sizeof(ordens_para_testar) / sizeof(int);

    int tamanhos_para_testar[] = {100, 1000, 10000, 100000, 1000000, 10000000, 20000000};
    int num_tamanhos = sizeof(tamanhos_para_testar) / sizeof(int);

    printf("\nINICIANDO TESTES DE DESEMPENHO\n");
    printf("=================================\n");

    for (int i = 0; i < num_tamanhos; i++) {
        int tamanho_atual = tamanhos_para_testar[i];
        if (tamanho_atual > total_carregado) {
            printf("\nPulando teste com %d registros: arquivo não possui registros suficientes.", tamanho_atual);
            continue;
        }

        printf("\n================= Testando com %d Registros ================= \n", tamanho_atual);
        preparar_chaves_busca(todos_os_carros, tamanho_atual);

        for (int j = 0; j < num_ordens; j++) {
            int ordem_atual = ordens_para_testar[j];
            
            // Cria uma nova árvore para cada teste
            BPlusTree* arvore = criar_arvore_bplus(ordem_atual);
            if (!arvore) continue;


            clock_t inicio_insercao = clock();
            // Fase de Inserção (cronometrada)
            for (int k = 0; k < tamanho_atual; k++) {
                inserir(arvore, todos_os_carros[k].renavam, &todos_os_carros[k]);
            }
            clock_t fim_insercao = clock();

            // Fase de Busca (cronometrada e com contagem de acessos)
            clock_t inicio = clock();
            int buscas_encontradas = 0;
            // O contador de acessos é zerado na criação da árvore e incrementado na busca
            for (int k = 0; k < NUM_BUSCAS_A_REALIZAR; k++) {
                Carro* resultado = buscar(arvore, chaves_para_busca[k]);
                if (resultado != NULL) buscas_encontradas++;
            }
            clock_t fim = clock();

            double tempo_total_cpu = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
            double tempo_medio_ms = (tempo_total_cpu * 1000.0) / (double)NUM_BUSCAS_A_REALIZAR;
            double tempo_insercao_ms = ((double)(fim_insercao - inicio_insercao) * 1000.0) / CLOCKS_PER_SEC;
            size_t tamanho_no = tamanho_no_bplustree(arvore, ordem_atual);
            long tamanho_bloco = get_block_size("docs/registros.txt");
            
            // Calcula a nova métrica de acessos médios por busca
            double media_acessos_por_busca = (double)arvore->acessos_de_disco_simulados / (double)NUM_BUSCAS_A_REALIZAR;
            // Calcula quantos blocos de disco são necessários para ler um único nó.
            double blocos_por_no = ceil((double)tamanho_no / (double)tamanho_bloco);

            // Calcula a métrica final multiplicando os acessos pelo custo de cada acesso.
            double custo_real_simulado = media_acessos_por_busca * blocos_por_no;


            printf("Ordem %2d:\n", ordem_atual);

            // Espaço de Memória
            printf("  \t[Espaço de Memória]\n");
            printf("    \t Tamanho do nó.........................: %zu bytes (%.2f KB)\n", 
                tamanho_no, (double)tamanho_no / 1024);
            printf("    \t Tamanho do bloco do disco.............: %ld bytes (%.2f KB)\n", 
                tamanho_bloco, (double)tamanho_bloco / 1024);

            // Tempo de Execução
            printf("  \t[Tempo de Execução]\n");
            printf("    \t Tempo total de inserção..............: %.6f ms\n", tempo_insercao_ms);
            printf("    \t Tempo total de busca (CPU)...........: %.6f ms\n", tempo_total_cpu * 1000);
            printf("    \t Tempo médio por busca (em CPU).......: %.6f ms (%d/%d encontradas)\n",
                tempo_medio_ms, buscas_encontradas, NUM_BUSCAS_A_REALIZAR);

            // Custo Simulado de Acesso ao Disco
            printf("  \t[Simulação de Acesso a Disco]\n");
            printf("    \t Média de acessos por busca...........: %.2f\n", media_acessos_por_busca);
            printf("    \t Custo total simulado.................: %.2f\n", custo_real_simulado);
            printf("\n==================================================== \n");
            destruir_arvore(arvore); // Libera a memória da árvore para o próximo teste
        }
    }

    // Libera a memória principal alocada para os carros
    free(todos_os_carros);
    
    printf("\n=================================\n");
    printf("Testes finalizados.\n");
    return 0;
}