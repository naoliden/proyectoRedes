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

// Funcion que inicializa el juego
Game* init_game(){
  Game* juego = malloc(sizeof(Game));
  juego -> turn = 0;
  juego -> termino = false;
  juego -> winner = 0;
  juego -> c_piezaso = 12;
  juego -> c_piezasx = 12;
  for (int row=0; row<8; row++){
    for (int col=0; col<8; col++){
      int sum = row + col;
      if (sum%2 != 0) {
        if (row < 3) juego -> board[row][col] ='x';
        else if (row > 4) juego -> board[row][col] ='o';
        else juego -> board[row][col] ='b';
      }
      else {
        juego -> board[row][col] ='n';
      }
    }
  }
  return juego;
}

// Funcion que imprime el tablero
void board_print(Game* juego) {
  printf("Le toca al JUGADOR %d \n", juego->turn);
  printf("Hay %d piezas blancas en el tablero\n", juego->c_piezaso);
  printf("Hay %d piezas negras en el tablero\n", juego->c_piezasx);
  printf("\n");
  printf("   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |\n");
  printf("------------------------------------\n");
  for (int row=0; row<8; row++){
    printf(" %d |", row);
    for (int col=0; col<8; col++){
      printf(" %c |", juego -> board[row][col]);
    }
    printf("\n------------------------------------\n");
  }
}

// indica si el movimiento es valido
bool valid(Game* juego, int row1, int col1, int row2, int col2){
  // validar que las filas y columnas a solicitar esten dentro del tablero
  if (row1 > 7 || col1>7 || row2>7 || col2 > 7)  return false;
  // validar que si es el turno es de las blancas (o), la pieza a mover sea 'o' o 'O'
  if (juego -> turn == 0 && (juego -> board[row1][col1] != 'o' && juego -> board[row1][col1] != 'O')) return false;
  // validar que si es el turno es de las negras (x), la pieza a mover sea 'x' o 'X'
  if (juego -> turn == 1 && (juego -> board[row1][col1] != 'x' && juego -> board[row1][col1] != 'X')) return false;
  //validar que este disponible la casilla a la cual se quiere llegar
  if (juego -> board[row2][col2] != 'b') return false;

  // validar si es posible llegar a la casilla sin comer piezas avanzando (un movimiento de peon o reina)
  if ((col2 == col1 +1 || col2 == col1-1) && row2 == row1-1 && (juego -> turn == 0 || juego -> board[row1][col1] == 'X')) return true;
  if ((col2 == col1 +1 || col2 == col1-1) && row2 == row1+1 && (juego -> turn == 1 || juego -> board[row1][col1] == 'O')) return true;
  printf("no puedo llegar en un movimiento\n");

  //validar si es posible llegar comiendo una pieza hacia adelante (de peon o reina)
  if (juego -> turn == 0 && row2 == row1-2 &&
    ((col2 == col1 +2 && (juego -> board[row1-1][col1+1] == 'x' || juego -> board[row1-1][col1+1] == 'X'))
      || (col2 == col1-2 && (juego -> board[row1-1][col1-1] == 'x' || juego -> board[row1-1][col1-1] == 'X')))) return true;
  if (juego -> turn == 1 && row2 == row1+2 &&
    ((col2 == col1 +2 && (juego -> board[row1+1][col1+1] == 'o' || juego -> board[row1+1][col1+1] == 'O'))
      || (col2 == col1-2 && (juego -> board[row1+1][col1-1] == 'o' || juego -> board[row1-1][col1-1] == 'O')))) return true;

  // validar si puedo llegar comiendo una pieza pero como reina
  if (juego -> board[row1][col1] == 'X' && row2 == row1-2 &&
    ((col2 == col1 +2 && (juego -> board[row1-1][col1+1] == 'o' || juego -> board[row1-1][col1+1] == 'O'))
      || (col2 == col1-2 && (juego -> board[row1-1][col1-1] == 'o' || juego -> board[row1-1][col1-1] == 'O')))) return true;
  if (juego -> board[row1][col1] == 'O' && row2 == row1+2 &&
    ((col2 == col1 +2 && (juego -> board[row1+1][col1+1] == 'x' || juego -> board[row1+1][col1+1] == 'X'))
      || (col2 == col1-2 && (juego -> board[row1+1][col1-1] == 'x' || juego -> board[row1-1][col1-1] == 'X')))) return true;
  printf("no puedo llegar comiendo\n");
  return false;
}

//realiza el movimiento, primero valida si es o no correcto
void do_move(Game* juego, int row1, int col1, int row2, int col2){
  // primero reviso si es valido el movimiento
  if (valid(juego, row1, col1, row2, col2)){
    int next = 1; // es 1 si no come una ficha y le toca al siguente, y 0 si come una ficha, por lo que puede seguir jugando
    // si come una ficha, saco la pieza que come y disminuyo la cantidad de piezas del contrincante
    if (row2 == row1-2){
      if(juego-> turn == 0) juego -> c_piezasx --; // si el blanco comio una pieza negra
      else juego -> c_piezaso --; // si el negro comio una pieza blanca
      //actualizo el tablero (borro la pieza que comio)
      if (col2 == col1 -2) juego -> board[row1-1][col1-1] = 'b';
      else juego -> board[row1-1][col1+1] = 'b';
      next = 0;
    }
    else if (row2 == row1+2){
      if(juego-> turn == 0) juego -> c_piezasx --; // si el blanco comio una pieza negra
      else juego -> c_piezaso --; // si el negro comio una pieza blanca
      //actualizo el tablero (borro la pieza que comio)
      if (col2 == col1 -2) juego -> board[row1+1][col1-1] = 'b';
      else juego -> board[row1+1][col1+1] = 'b';
      next = 0;
    }
    //cambio la pieza de posicion
    char valor = juego->board[row1][col1];
    juego->board[row1][col1] = 'b';
    // si llega al final convertimos en reina , sino solo actualizamos
    if (valor == 'x' && row2 == 7) juego->board[row2][col2] = 'X';
    else if (valor == 'o' && row2 == 0) juego->board[row2][col2] = 'O';
    else juego->board[row2][col2] = valor;
    printf("\n ---------------------------\n    MOVIMIENTO VALIDO\n ---------------------------\n");
    if (next){
      if (juego->turn == 0) juego->turn =1;
      else juego->turn = 0;
    }
    board_print(juego);
  }
  else printf("\n ---------------------------\n    MOVIMIENTO INVALIDO\n ---------------------------\n");
}
