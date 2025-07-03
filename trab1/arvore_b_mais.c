#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

// --- DEFINIÇÕES DE CONSTANTES GLOBAIS ---

// Configurações da Árvore B+
#define MAX_ORDER 41       // Ordem máxima que a estrutura suporta.

// Configurações dos Registros de Carros
#define MAX_MODELO_LEN 50   // Tamanho máximo para a string do modelo do carro.
#define MAX_COR_LEN 30      // Tamanho máximo para a string da cor do carro.

// Configurações de Teste de Desempenho
#define NUM_BUSCAS_A_REALIZAR 500 // Quantidade de buscas aleatórias para teste.

// --- ESTRUTURAS DE DADOS ---

// Estrutura para os registros de carros
typedef struct {
    int renavam;
    char modelo[MAX_MODELO_LEN];
    int ano;
    char cor[MAX_COR_LEN];
} Carro;

// Estrutura para os nós da Árvore B+
typedef struct No {
    bool folha;
    int num_chaves;
    int chaves[MAX_ORDER + 1];
    void *ponteiros[MAX_ORDER + 1];
    struct No *prox_folha;
    struct No *pai;
} No;

// Estrutura que encapsula a Árvore B+
typedef struct {
    No *raiz;
    int ordem;
} BPlusTree;


// --- PROTÓTIPOS DE FUNÇÕES ---
// Note que as funções agora recebem um ponteiro para a árvore
void dividir_no(BPlusTree *arvore, No *no);
void inserir_no_pai(BPlusTree *arvore, No *no_esquerdo, int chave, No *no_direito);
No* criar_no();


// --- FUNÇÕES DA ÁRVORE B+ ---

/**
 * @brief Aloca memória e inicializa uma nova Árvore B+.
 * @param ordem A ordem da árvore a ser criada.
 * @return Ponteiro para a nova árvore.
 */
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
    return novo_no;
}

/**
 * @brief Libera recursivamente os nós da árvore.
 * @param no O nó raiz da árvore (ou sub-árvore) a ser destruída.
 */
void destruir_nos_recursivo(No *no) {
    if (no == NULL) return;
    if (!no->folha) {
        for (int i = 0; i <= no->num_chaves; i++) {
            destruir_nos_recursivo(no->ponteiros[i]);
        }
    }
    free(no);
}

/**
 * @brief Libera toda a memória utilizada por uma Árvore B+.
 * @param arvore A árvore a ser destruída.
 */
void destruir_arvore(BPlusTree *arvore) {
    if (arvore == NULL) return;
    destruir_nos_recursivo(arvore->raiz);
    free(arvore);
}

/**
 * @brief Busca por uma chave na árvore.
 * @param arvore A Árvore B+ onde a busca será realizada.
 * @param chave A chave (renavam) a ser encontrada.
 * @return Ponteiro para a estrutura `Carro` se encontrada, ou `NULL` caso contrário.
 */
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


/**
 * @brief Insere uma chave e um ponteiro em um nó pai após uma divisão (split).
 * @param arvore A árvore sendo modificada.
 * @param no_esquerdo O nó original que foi dividido.
 * @param chave A chave que será "promovida" ao pai.
 * @param no_direito O novo nó criado na divisão.
 */
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
        int temp_chaves[MAX_ORDER];
        void* temp_ponteiros[MAX_ORDER + 1];
        
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

/**
 * @brief Divide um nó que atingiu sua capacidade máxima de chaves.
 * @param arvore A árvore sendo modificada.
 * @param no O nó a ser dividido.
 */
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

/**
 * @brief Insere um novo par (chave, valor) na árvore B+.
 * @param arvore A árvore sendo modificada.
 * @param chave O renavam do carro.
 * @param carro Um ponteiro para a estrutura `Carro` a ser inserida.
 */
/**
 * @brief Insere um novo par (chave, valor) na árvore B+.
 * @param arvore A árvore sendo modificada.
 * @param chave O renavam do carro.
 * @param carro Um ponteiro para a estrutura `Carro` a ser inserida.
 */
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

// --- FUNÇÕES DE TESTE E GERENCIAMENTO DE DADOS ---
int chaves_para_busca[NUM_BUSCAS_A_REALIZAR];

/**
 * @brief Conta o número de linhas (registros) em um arquivo.
 * @param nome_arquivo O caminho para o arquivo.
 * @return O número de linhas contadas.
 */
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


/**
 * @brief Carrega registros de um arquivo para um array de Carros já alocado.
 * @param carros_out Ponteiro para o array de Carro onde os dados serão armazenados.
 * @param num_a_carregar O número máximo de registros a serem lidos.
 * @return O número de registros efetivamente carregados.
 */
int carregar_registros(Carro *carros_out, long num_a_carregar) {
    FILE *f = fopen("registros.txt", "r");
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

/**
 * @brief Seleciona chaves aleatórias dos registros carregados para usar nos testes.
 * @param todos_os_carros Array com todos os carros.
 * @param num_registros O total de registros disponíveis para selecionar.
 */
void preparar_chaves_busca(const Carro *todos_os_carros, int num_registros) {
    srand(time(NULL));
    for (int i = 0; i < NUM_BUSCAS_A_REALIZAR; i++) {
        int random_index = rand() % num_registros;
        chaves_para_busca[i] = todos_os_carros[random_index].renavam;
    }
}

int main() {
    printf("Contando registros no arquivo...\n");
    long total_registros_no_arquivo = contar_registros("registros.txt");
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
    int ordens_para_testar[] = {3, 5, 10, 15, 20, 40};
    int num_ordens = sizeof(ordens_para_testar) / sizeof(int);

    int tamanhos_para_testar[] = {100, 1000, 10000, 100000, 1000000, 10000000, 20000000};
    int num_tamanhos = sizeof(tamanhos_para_testar) / sizeof(int);

    printf("\nINICIANDO TESTES DE DESEMPENHO\n");
    printf("=================================\n");

    for (int i = 0; i < num_tamanhos; i++) {
        int tamanho_atual = tamanhos_para_testar[i];
        if (tamanho_atual > total_carregado) {
            printf("\nPulando teste com %d registros: arquivo não possui registros suficientes.\n", tamanho_atual);
            continue;
        }

        printf("\n--- Testando com %d Registros ---\n", tamanho_atual);
        preparar_chaves_busca(todos_os_carros, tamanho_atual);

        for (int j = 0; j < num_ordens; j++) {
            int ordem_atual = ordens_para_testar[j];
            
            // Cria uma nova árvore para cada teste
            BPlusTree* arvore = criar_arvore_bplus(ordem_atual);
            if (!arvore) continue;

            // Fase de Inserção (não cronometrada)
            for (int k = 0; k < tamanho_atual; k++) {
                inserir(arvore, todos_os_carros[k].renavam, &todos_os_carros[k]);
            }

            // Fase de Busca (cronometrada)
            clock_t inicio = clock();
            int buscas_encontradas = 0;
            for (int k = 0; k < NUM_BUSCAS_A_REALIZAR; k++) {
                Carro* resultado = buscar(arvore, chaves_para_busca[k]);
                if (resultado != NULL) buscas_encontradas++;
            }
            clock_t fim = clock();

            double tempo_total_cpu = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
            double tempo_medio_ms = (tempo_total_cpu * 1000.0) / (double)NUM_BUSCAS_A_REALIZAR;

            printf("Ordem %2d: Tempo médio por busca: %.6f ms (%d/%d encontradas)\n",
                   ordem_atual, tempo_medio_ms, buscas_encontradas, NUM_BUSCAS_A_REALIZAR);
            printf("\t   Tempo total para %d buscas: %.6f segundos (%d encontradas)\n",
                    NUM_BUSCAS_A_REALIZAR, tempo_total_cpu, buscas_encontradas);
            // Libera a memória da árvore usada neste teste
            destruir_arvore(arvore);
        }
    }

    // Libera a memória principal alocada para os carros
    free(todos_os_carros);
    
    printf("\n=================================\n");
    printf("Testes finalizados.\n");
    return 0;
}