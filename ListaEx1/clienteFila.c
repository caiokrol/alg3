/*---------------------------------------------------------
PROGRAMA: cliente TAD Fila
-----------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>

typedef int ItemFila;
#include "itemInt.h"
#include "fila.h"

/* ----------------------------------------------------- 
  Programa: cria uma fila e escreve os seus valores
*/
int main(int argc, char *argv[]){
  Fila fila;
  ItemFila v;
  
  criaFila( &fila );
  read( v );
  while( !eq( v, FIM )){
    insereFila( v, &fila );
    read( v );
  }
  printf( "\nFila\n" );
  while( !filaVazia( fila )){
    removeFila( &fila, &v );
    write( v );
    printf("\n");
  }
  return 0;
}

