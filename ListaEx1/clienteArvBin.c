/*---------------------------------------------------------
PROGRAMA: cliente TAD ArvBin
-----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

typedef int ItemArv;
#include "itemInt.h"
#include "arvBin.h"


int somaChave(ArvBin arvore){
    

}

/* ----------------------------------------------------- */
/* Programa: cria uma árvore binária e imprime a árvore
*/
int main(int argc, char *argv[]){
  ArvBin arv;
  ItemArv v;
  
  criaArv( &arv );
  read( v );
  while( !eq( v, FIM )){
    arv= insereArv( v, arv );
    read( v );
  }
  printf( "\nArvore\n" );
  escreveArv( arv );

  freeArv( arv );
  return 0;
}
