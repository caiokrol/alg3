#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// --- DEFINIÇÕES DE CONSTANTES GLOBAIS ---

// Configurações da Árvore B+
#define MAX_ORDER 21        // Ordem máxima da árvore que a estrutura suporta.

// Configurações dos Registros de Carros
#define MAX_RECORDS 20000000 // Suporte para até 1 milhão de registros em memória.
#define MAX_MODELO_LEN 50   // Tamanho máximo para a string do modelo do carro.
#define MAX_COR_LEN 30      // Tamanho máximo para a string da cor do carro.

// Configurações de Teste de Desempenho
#define NUM_BUSCAS_A_REALIZAR 500 // Quantidade de buscas aleatórias para teste de desempenho.

// --- ESTRUTURAS DE DADOS ---
typedef struct {
    int renavam;
    char modelo[MAX_MODELO_LEN];
    int ano;
    char cor[MAX_COR_LEN];
} Carro;

typedef struct No {
    bool folha;
    int num_chaves;
    int chaves[MAX_ORDER + 1];
    void *ponteiros[MAX_ORDER + 1];
    struct No *prox_folha;
    struct No *pai;
} No;

// --- VARIÁVEIS GLOBAIS ---
No *raiz = NULL;
int ordem_arvore = 0; // Ordem real da árvore, definida dinamicamente nos testes.

// --- PROTÓTIPOS DE FUNÇÕES ---
void dividir_no(No *no);

// --- FUNÇÕES DA ÁRVORE B+ ---

/**
 * @brief Aloca memória para um novo nó da árvore.
 * @return Ponteiro para o novo nó criado.
 */
No* criar_no() {
    No *novo_no = (No*)calloc(1, sizeof(No));
    if (!novo_no) {
        perror("Falha ao alocar memória para o nó");
        exit(EXIT_FAILURE);
    }
    novo_no->folha = true;
    return novo_no;
}

/**
 * @brief Libera recursivamente toda a memória utilizada pela árvore.
 * @param no O nó raiz da árvore (ou sub-árvore) a ser destruída.
 */
void destruir_arvore(No *no) {
    if (no == NULL) return;
    if (!no->folha) {
        for (int i = 0; i <= no->num_chaves; i++) {
            destruir_arvore(no->ponteiros[i]);
        }
    }
    free(no);
}

/**
 * @brief Busca por uma chave na árvore.
 * @param no O nó a partir do qual a busca se inicia.
 * @param chave A chave (renavam) a ser encontrada.
 * @return Ponteiro para a estrutura `Carro` se encontrada, ou `NULL` caso contrário.
 */
Carro* buscar(No *no, int chave) {
    if (no == NULL) return NULL;
    if (no->folha) {
        for (int i = 0; i < no->num_chaves; i++) {
            if (no->chaves[i] == chave) {
                return (Carro*)no->ponteiros[i];
            }
        }
        return NULL;
    }
    int i = 0;
    while (i < no->num_chaves && chave >= no->chaves[i]) {
        i++;
    }
    return buscar(no->ponteiros[i], chave);
}

/**
 * @brief Insere uma chave e um ponteiro em um nó pai após uma divisão (split).
 * @param no_esquerdo O nó original que foi dividido.
 * @param chave A chave que será "promovida" ao pai.
 * @param no_direito O novo nó criado na divisão.
 */
void inserir_no_pai(No *no_esquerdo, int chave, No *no_direito) {
    No *pai = no_esquerdo->pai;
    if (pai == NULL) {
        // Se não há pai, cria uma nova raiz.
        raiz = criar_no();
        raiz->folha = false;
        raiz->chaves[0] = chave;
        raiz->ponteiros[0] = no_esquerdo;
        raiz->ponteiros[1] = no_direito;
        raiz->num_chaves = 1;
        no_esquerdo->pai = raiz;
        no_direito->pai = raiz;
        return;
    }

    // Encontra a posição correta para a nova chave/ponteiro no pai.
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

    // Se o pai ficou cheio, divide-o também.
    if (pai->num_chaves == ordem_arvore) {
        dividir_no(pai);
    }
}

/**
 * @brief Divide um nó que atingiu sua capacidade máxima de chaves.
 * @param no O nó a ser dividido.
 */
void dividir_no(No *no) {
    int meio_idx = ordem_arvore / 2;
    No *irmao_direito = criar_no();
    irmao_direito->folha = no->folha;
    int chave_promovida;

    if (!no->folha) { // Divisão de nó interno
        chave_promovida = no->chaves[meio_idx];
        for (int i = meio_idx + 1; i < ordem_arvore; i++) {
            irmao_direito->chaves[i - (meio_idx + 1)] = no->chaves[i];
            irmao_direito->ponteiros[i - (meio_idx + 1)] = no->ponteiros[i];
        }
        irmao_direito->ponteiros[ordem_arvore - (meio_idx + 1)] = no->ponteiros[ordem_arvore];
        irmao_direito->num_chaves = ordem_arvore - meio_idx - 1;
        no->num_chaves = meio_idx;
        for (int i = 0; i <= irmao_direito->num_chaves; i++) {
            No *filho = (No*)irmao_direito->ponteiros[i];
            if (filho) filho->pai = irmao_direito;
        }
    } else { // Divisão de nó folha
        for (int i = meio_idx; i < ordem_arvore; i++) {
            irmao_direito->chaves[i - meio_idx] = no->chaves[i];
            irmao_direito->ponteiros[i - meio_idx] = no->ponteiros[i];
        }
        irmao_direito->num_chaves = ordem_arvore - meio_idx;
        no->num_chaves = meio_idx;
        chave_promovida = irmao_direito->chaves[0];
        irmao_direito->prox_folha = no->prox_folha;
        no->prox_folha = irmao_direito;
    }

    inserir_no_pai(no, chave_promovida, irmao_direito);
}

/**
 * @brief Insere um novo par (chave, valor) na árvore B+.
 * @param chave O renavam do carro.
 * @param carro Um ponteiro para a estrutura `Carro` a ser inserida.
 */
void inserir(int chave, Carro *carro) {
    if (raiz == NULL) {
        raiz = criar_no();
        raiz->chaves[0] = chave;
        raiz->ponteiros[0] = carro;
        raiz->num_chaves = 1;
        return;
    }

    No *no_atual = raiz;
    while (!no_atual->folha) {
        int i = 0;
        while (i < no_atual->num_chaves && chave >= no_atual->chaves[i]) {
            i++;
        }
        no_atual = (No*)no_atual->ponteiros[i];
    }

    int i = no_atual->num_chaves;
    while (i > 0 && no_atual->chaves[i - 1] > chave) {
        no_atual->chaves[i] = no_atual->chaves[i - 1];
        no_atual->ponteiros[i] = no_atual->ponteiros[i - 1];
        i--;
    }
    no_atual->chaves[i] = chave;
    no_atual->ponteiros[i] = carro;
    no_atual->num_chaves++;

    if (no_atual->num_chaves == ordem_arvore) {
        dividir_no(no_atual);
    }
}

// --- FUNÇÕES PRINCIPAL E DE TESTES ---
Carro todos_os_carros[MAX_RECORDS];
int chaves_para_busca[NUM_BUSCAS_A_REALIZAR];

/**
 * @brief Carrega registros de um arquivo de texto para o array `todos_os_carros`.
 * @param num_a_carregar O número máximo de registros a serem lidos.
 * @return O número de registros efetivamente carregados.
 */
int carregar_registros(int num_a_carregar) {
    FILE *f = fopen("registros.txt", "r");
    if (!f) {
        perror("Erro: 'registros.txt' não encontrado. Execute o gerador primeiro");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    // Monta a string de formato para o sscanf de forma segura e adaptável
    char formato_scanf[100];
    sprintf(formato_scanf, "%%d;%%%d[^;];%%d;%%%d[^\n]\n", MAX_MODELO_LEN - 1, MAX_COR_LEN - 1);
    
    while (i < num_a_carregar && fscanf(f, formato_scanf,
        &todos_os_carros[i].renavam,
        todos_os_carros[i].modelo,
        &todos_os_carros[i].ano,
        &todos_os_carros[i].cor) == 4) {
        i++;
    }
    fclose(f);
    return i;
}

/**
 * @brief Seleciona chaves aleatórias dos registros carregados para usar nos testes.
 * @param num_registros O total de registros disponíveis para selecionar.
 */
void preparar_chaves_busca(int num_registros) {
    srand(time(NULL));
    for (int i = 0; i < NUM_BUSCAS_A_REALIZAR; i++) {
        int random_index = rand() % num_registros;
        chaves_para_busca[i] = todos_os_carros[random_index].renavam;
    }
}

int main() {
    printf("Carregando registros do arquivo...\n");
    int total_registros = carregar_registros(MAX_RECORDS);

    // Cenários de teste
    int ordens_para_testar[] = {3, 5, 10, 15, 20};
    int num_ordens = sizeof(ordens_para_testar) / sizeof(int);

    int tamanhos_para_testar[] = {100, 1000, 10000, 100000, 1000000, 10000000, 20000000};
    int num_tamanhos = sizeof(tamanhos_para_testar) / sizeof(int);

    printf("\nINICIANDO TESTES DE DESEMPENHO\n");
    printf("=================================\n");

    for (int i = 0; i < num_tamanhos; i++) {
        int tamanho_atual = tamanhos_para_testar[i];
        if (tamanho_atual > total_registros) {
            printf("\nPulando teste com %d registros: arquivo não possui registros suficientes.\n", tamanho_atual);
            continue;
        }

        printf("\n--- Testando com %d Registros ---\n", tamanho_atual);
        preparar_chaves_busca(tamanho_atual);

        for (int j = 0; j < num_ordens; j++) {
            ordem_arvore = ordens_para_testar[j];
            destruir_arvore(raiz);
            raiz = NULL;

            // Fase de Inserção (não cronometrada)
            for (int k = 0; k < tamanho_atual; k++) {
                inserir(todos_os_carros[k].renavam, &todos_os_carros[k]);
            }

            // Fase de Busca (cronometrada)
            clock_t inicio = clock();
            int buscas_encontradas = 0;
            for (int k = 0; k < NUM_BUSCAS_A_REALIZAR; k++) {
                Carro* resultado = buscar(raiz, chaves_para_busca[k]);
                if (resultado != NULL) buscas_encontradas++;
            }
            clock_t fim = clock();

            double tempo_total_cpu = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
            double tempo_medio_ms = (tempo_total_cpu * 1000.0) / (double)NUM_BUSCAS_A_REALIZAR;

            printf("Ordem %2d: Tempo médio por busca: %.6f ms (%d/%d encontradas)\n",
                   ordem_arvore, tempo_medio_ms, buscas_encontradas, NUM_BUSCAS_A_REALIZAR);
            printf("\t   Tempo total para %d buscas: %.6f segundos (%d encontradas)\n",
                    NUM_BUSCAS_A_REALIZAR, tempo_total_cpu, buscas_encontradas);
        }
    }

    destruir_arvore(raiz);
    printf("\n=================================\n");
    printf("Testes finalizados.\n");
    return 0;
}