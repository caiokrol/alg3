/*---------------------------------------------------------
Interface: TAD Pilha
-----------------------------------------------------------*/
#define MAX 100

struct NoPilha {
  ItemPilha item[MAX];
  int topo;
};

typedef struct NoPilha Pilha;

void criaPilha(Pilha*);
int pilhaVazia( Pilha );
void empilha( ItemPilha, Pilha*);
void desempilha( Pilha*, ItemPilha* );
