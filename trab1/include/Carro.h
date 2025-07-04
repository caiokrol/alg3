#ifndef CARRO_H
#define CARRO_H

#define MAX_MODELO_LEN 50
#define MAX_COR_LEN 30

typedef struct {
    int renavam;
    char modelo[MAX_MODELO_LEN];
    int ano;
    char cor[MAX_COR_LEN];
} Carro;

#endif
