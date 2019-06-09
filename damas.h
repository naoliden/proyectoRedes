// Indica que este archivo solo debe compilarse una vez
// Esto debe ir al inicio de cada .h
#pragma once
/** Módulo estándar de números enteros */
#include <stdint.h>
// Libreria estandar de c
#include <stdlib.h>
// Libreria para bool
#include <stdbool.h>

typedef struct game {
  // 0 si le toca al jugador piezas blancas o y 1 si le toca al jugador piezas negras x
  int turn;

  // indica si termino o no el juego
  bool termino;
  int winner;
  int count; // contador que indica la cantidad de movimientos relizados sin comer piezas y con empate

  // cantidad de piezas blancas (o O) en el tablero
  int c_piezaso;
  // cantidad de piezas negras (x, X) en el tablero
  int c_piezasx;

  char board [8][8];

} Game;

// Funcion que inicializa el juego
Game* init_game();

// Funcion que imprime el tablero
void board_print(Game* juego);

// indica si el movimiento es valido
bool valid(Game* juego, int row1, int col1, int row2, int col2);

//realiza el movimiento, primero valida si es o no correcto
void do_move(Game* juego, int row1, int col1, int row2, int col2);

// Indica si existen movimientos validos para el jugador que le toca
bool moves(Game* juego);
