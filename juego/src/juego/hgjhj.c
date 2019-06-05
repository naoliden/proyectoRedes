// En este archivo .c están las definiciones de las funciones del modulo
#include "calculo.h"
/** Módulo estándar de números enteros */
#include <stdint.h>
/** Módulo estándar de Input y Output */
#include <stdio.h>
// Libreria estandar de c
#include <stdlib.h>
/**
int main()
{
  //////// INPUT /////////

  // Cantidad de pastillas enteras
  uint32_t cantidad_pastillas = 3;
  // Cantidad de dias a evaluar, parte del dia 0
  uint32_t dias = 5;

  ///////// FIN INPUT ////////

  ///////// PROGRAMA ////////

  float resultado;
  float ** arreglo = malloc(cantidad_pastillas*2*sizeof(float*));
  for (int i = 0; i<cantidad_pastillas*2; i++){
    arreglo[i] = malloc(cantidad_pastillas*2*sizeof(float));
  }
  for (int i = 0;  i<cantidad_pastillas*2; i++){
    for  (int j = 0;  j<cantidad_pastillas*2; j++){
      arreglo[i][j] = 2;
    }
  }
  resultado = probabilidad(arreglo, cantidad_pastillas, 0, dias);
  printf("%lf\n", resultado);

  for (int i = 0;  i<cantidad_pastillas*2; i++){
    free(arreglo[i]);
  }
  free(arreglo);

}
**/
