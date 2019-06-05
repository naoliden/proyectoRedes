// En este archivo .c están las definiciones de las funciones del modulo
#include "damas.h"
/** Módulo estándar de números enteros */
#include <stdint.h>
/** Módulo estándar de Input y Output */
#include <stdio.h>
// Libreria estandar de c
#include <stdlib.h>
// Libreria para bool
#include <stdbool.h>

int main()
{
  Game* mi_juego = init_game();
  int row1, row2, col1, col2;
  //int continuar = 0;
  board_print(mi_juego);
  while (!mi_juego->termino){
    //continuar = 0;
    row1 = 8;
    row2 = 8;
    col1 = 8;
    col2 = 8;
    printf("ingrese fila, columna de la pieza (por ejemplo >> 2,3)\n");
    scanf("%d,%d",&row1, &col1);
    printf("ingrese fila de donde quiere llegar\n");
    scanf("%d,%d",&row2, &col2);
    do_move(mi_juego, row1,col1,row2,col2);
    //printf("detener? 1 para detener\n");
    //scanf("%d",&continuar);
  }
  printf("///////////////////////////////\n");
  printf("/////   termino el juego  /////\n");
  printf("///////////////////////////////\n\n\n");
  if (mi_juego->winner == 3) printf(" HAY UN EMPATE\n");
  else printf("EL GANADOR ES EL JUGADOR %d\n", mi_juego->winner);

  free(mi_juego);
}
