/*---------------------------------------------------------
PROGRAMA: TAD Pilha
-----------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>

typedef char ItemPilha[50]; 
#include "itemString.h"
#include "pilhaVetor.h"

/* ----------------------------------------------------- */
/* Função de inicialização de uma pilha vazia 
*/
void criaPilha(Pilha *pilha){
  pilha->topo = -1;
}

/* ----------------------------------------------------- 
   Retorna 1 se a pilha estiver vazia e 0, caso contrario
*/
int pilhaVazia( Pilha p ){
    if( p.topo == -1 )
        return 1;
    return 0;
}

/* ----------------------------------------------------- 
   Empilha um novo Item 
*/
void empilha( ItemPilha v, Pilha *p ){
  
  p->topo++;
  cp( p->item[p->topo],  v);
}

/* ----------------------------------------------------- 
   Desempilha um Item 
*/
void desempilha( Pilha *p, ItemPilha *v){
  
  if( pilhaVazia( *p ))
    cp( *v, ERRO );
  cp( *v, p->item[p->topo] );
  p->topo--;
}



