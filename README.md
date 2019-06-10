
---
    GRUPO: grupo001
---

INTEGRANTES:

- Mari Elida Tuhus - 18410308

- Anne Wulff Wold - 18410294

- Vanesa Blasco - 14638002

- Gianfranco Bordoni - 1463189J

- Norman Oliden - 14201399

---

SUPUESTOS:

- Se ingresa un puerto y una IP válida al momento de ejecutar el servidor y el cliente
- La asignación de nicknames y el juego no comienzan hasta que se hayan conectado dos jugadores al servidor

COMENTARIOS:

- Se definieron las estructuras mail y my_msg para representar los mensajes. Éstas poseen el id del mensaje, el tamaño, y el texto del mensaje.
- Tanto en el servidor como en el cliente, la función reciveMessage es la encargada de recibir los paquetes, y crear y retornar la estructura correspondiente al mensaje.
- Se definió la estructura Game pa representar el juego de damas. Ésta contiene diversos datos útiles durante la ejecución del juego, tales como el tablero, el turno, la cantidad de piezas restante, y el ganador del juego. Además, posee un contador de movimientos en los que no exista un cambio de piezas en el tablero, para el caso de empate.
- En cada turno, se le pregunta a cada jugador si es que quiere enviar un mensaje por el chat del juego.
- Al finalizar el juego, se les consulta a ambos jugadores si es que quieren jugar otra partida. Si y solo si ambos responden afirmativamente, se dará inicio a una nueva partida.
