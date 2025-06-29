# Executáveis
GERADOR = gerador
ARVORE  = teste_arvore

# Fontes
SRC_GERADOR = gerador_registros.c
SRC_ARVORE  = arvore_b_mais.c

# Compilador e flags
CC = gcc
CFLAGS = -Wall -O2

# Compila tudo
all: $(GERADOR) $(ARVORE)

# Compila o gerador
$(GERADOR): $(SRC_GERADOR)
	$(CC) $(CFLAGS) -o $(GERADOR) $(SRC_GERADOR)

# Compila a árvore
$(ARVORE): $(SRC_ARVORE)
	$(CC) $(CFLAGS) -o $(ARVORE) $(SRC_ARVORE)

# Executa o gerador e depois testa a árvore
run: $(GERADOR) $(ARVORE)
	@echo "[1/2] Gerando registros..."
	./$(GERADOR)
	@echo "[2/2] Executando testes com árvore B+..."
	./$(ARVORE)

# Limpa tudo
clean:
	rm -f $(GERADOR) $(ARVORE) registros.txt
