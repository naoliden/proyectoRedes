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
  Game* mi_juego = init_game(); //inicializamos el tablero

  int row1, row2, col1, col2;

  board_print(mi_juego); //imprime el tablero

  while (!mi_juego->termino){
    //recoleccion de datos
    row1 = 8;
    row2 = 8;
    col1 = 8;
    col2 = 8;
    printf("ingrese fila, columna de la pieza (por ejemplo >> 2,3)\n");
    scanf("%d,%d",&row1, &col1);
    printf("ingrese fila, columna de donde quiere llegar\n");
    scanf("%d,%d",&row2, &col2);

    //aqui se juega
    if (moves(mi_juego)) do_move(mi_juego, row1,col1,row2,col2);

  }

  //print de que termino el juego
  printf("///////////////////////////////\n");
  printf("/////   termino el juego  /////\n");
  printf("///////////////////////////////\n\n\n");
  if (mi_juego->winner == 3) printf(" HAY UN EMPATE\n");
  else printf("EL GANADOR ES EL JUGADOR %d\n", mi_juego->winner);

  //libera la memria
  free(mi_juego);
}
