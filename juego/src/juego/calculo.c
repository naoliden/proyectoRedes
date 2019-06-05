// En este archivo .c están las definiciones de las funciones del modulo
#include "calculo.h"
/** Módulo estándar de números enteros */
#include <stdint.h>
/** Módulo estándar de Input y Output */
#include <stdio.h>


float probabilidad(float** arreglo, uint32_t entera, uint32_t media, uint32_t dias){
  float total= 0;
  if (dias == 0){
    total = (float)media/(float)(media + entera);
    arreglo[entera][media]=total;
    return total;
  }
  else {
    //vemos si esta en la tabla
    if (arreglo[entera][media] != 2) {
      return arreglo[entera][media];
    }
    else{
      //probabiblidad por sacar media
      if(!media==0) total = ((float)media/(float)(media+entera))*probabilidad(arreglo, entera, media-1, dias-1);
      //probabilidad de sacar una entera
      if (!entera == 0) total = total + ((float)entera/(float)(media+entera))*probabilidad(arreglo, entera-1, media+1, dias-1);
    }

    arreglo[entera][media] = total;
    return total;
  }
}
