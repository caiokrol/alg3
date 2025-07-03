#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//#define NUM_REGISTROS 10000
#define MAX_MODELOS 1000
#define MAX_LINHA 256

typedef struct {
    char modelo[MAX_LINHA];
    int ano_inicio;
    int ano_fim;
} Automovel;

// Gera ano aleatório dentro do intervalo do modelo
int gerar_ano(int inicio, int fim) {
    if (inicio > fim) return inicio;
    return inicio + rand() % (fim - inicio + 1);
}

// Carrega modelos e seus anos do arquivo
int carregar_modelos(const char *nome_arquivo, Automovel modelos[]) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        perror("Erro ao abrir o arquivo de modelos");
        return -1;
    }

    int count = 0;
    char linha[MAX_LINHA];
    while (fgets(linha, sizeof(linha), arquivo) && count < MAX_MODELOS) {
        int ok = sscanf(linha, "%[^;];%d;%d", modelos[count].modelo, &modelos[count].ano_inicio, &modelos[count].ano_fim);
        if (ok == 3) count++;
        else fprintf(stderr, "Linha ignorada: %s", linha);
    }

    fclose(arquivo);
    return count;
}

// Embaralha vetor de RENAVAMs com algoritmo de Fisher-Yates
void embaralhar(unsigned long long *array, int tamanho) {
    for (int i = tamanho - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        unsigned long long tmp = array[i];
        array[i] = array[j];
        array[j] = tmp;
    }
}

int main() {
    FILE *arquivo_saida;
    Automovel automoveis[MAX_MODELOS];
    const char *cores[] = {"Preto", "Branco", "Prata", "Vermelho", "Azul", "Cinza", "Verde"};
    int num_cores = sizeof(cores) / sizeof(cores[0]);

        // --- ALTERAÇÃO PRINCIPAL AQUI ---
    int num_registros_desejados;
    printf("Digite o número de registros desejados: (Max 20.000.000)\n");
    // Valida se a entrada é um número inteiro positivo
    if (scanf("%d", &num_registros_desejados) != 1 || num_registros_desejados <= 0 || num_registros_desejados > 20000000) {
        fprintf(stderr, "Erro: Entrada inválida. Por favor, insira um número válido.\n");
        return 1;
    }
    // --- FIM DA ALTERAÇÃO ---

    srand(time(NULL));

    int num_modelos = carregar_modelos("modelos_anos.txt", automoveis);
    if (num_modelos <= 0) {
        fprintf(stderr, "Erro: nenhum modelo carregado.\n");
        return 1;
    }

    // Aloca e gera RENAVAMs únicos incrementais
    unsigned long long *renavams = malloc(sizeof(unsigned long long) * num_registros_desejados);
    if (!renavams) {
        perror("Erro alocando vetor de RENAVAMs");
        return 1;
    }

    unsigned long long base = 10000000000ULL;
    for (int i = 0; i < num_registros_desejados; i++) {
        renavams[i] = base + i;
    }

    // Embaralha os RENAVAMs para parecerem aleatórios
    embaralhar(renavams, num_registros_desejados);

    arquivo_saida = fopen("registros.txt", "w");
    if (!arquivo_saida) {
        perror("Erro ao abrir arquivo de saída");
        free(renavams);
        return 1;
    }

    printf("Gerando %d registros de automóveis com RENAVAMs únicos...\n", num_registros_desejados);

    for (int i = 0; i < num_registros_desejados; i++) {
        int modelo_idx = rand() % num_modelos;
        Automovel carro = automoveis[modelo_idx];
        int ano = gerar_ano(carro.ano_inicio, carro.ano_fim);
        const char *cor = cores[rand() % num_cores];

        fprintf(arquivo_saida, "%011llu;%s;%d;%s\n", renavams[i], carro.modelo, ano, cor);
    }

    fclose(arquivo_saida);
    free(renavams);

    printf("Arquivo 'registros.txt' gerado com sucesso!\n");
    return 0;
}
