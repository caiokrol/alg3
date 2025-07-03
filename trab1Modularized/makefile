# Diretórios
INCLUDE_DIR = include
SRC_DIR = src
BUILD_DIR = build
BIN_DIR = bin
DOCS_DIR = docs

# Executáveis
GERADOR = $(BIN_DIR)/gerador
ARVORE  = $(BIN_DIR)/teste_arvore

# Arquivos-fonte
SRC_GERADOR = gerador_registros.c
SRC_ARVORE = $(SRC_DIR)/main.c $(SRC_DIR)/BPlusTree.c $(SRC_DIR)/Util.c

# Arquivos-objeto (gerados a partir dos .c)
OBJ_ARVORE = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC_ARVORE))

# Compilador e flags
CC = gcc
CFLAGS = -Wall -O2 -I$(INCLUDE_DIR)

# Regra padrão
all: $(GERADOR) $(ARVORE)

# Criação de diretórios se não existirem
$(BUILD_DIR) $(BIN_DIR):
	mkdir -p $@

# Gerador direto (não modularizado)
$(GERADOR): $(SRC_GERADOR) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $<

# Compila os .o para a árvore
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Linka os objetos para formar o binário da árvore
$(ARVORE): $(OBJ_ARVORE) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Executa o gerador e o teste da árvore
run: $(GERADOR) $(ARVORE)
	@echo "[1/2] Gerando registros..."
	./$(GERADOR)
	@echo "[2/2] Executando testes com árvore B+..."
	./$(ARVORE)

# Limpa tudo
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR) $(DOCS_DIR)/registros.txt

