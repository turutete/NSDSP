/** \page   page_1   Main.c
 *
 *
 *  Este módulo principal ejecuta los scripts de prueba de todas las funciones de la librería
 *  NSDSP (Non Stationary Digital Signal Processing)
 *
 *
 * \section submodules Submódulos
 *
 * - \subpage	page_2
 * - \subpage	page_3
 *
 *
 *  \section module1_changes Historial de Cambios
 *
 *  | Fecha | Autores | Versión | Descripción |
 *  |:--: |:--:|:--:|:--:|
 *  | 09/06/2025| Autores |1.0.0.0|Primera edición|
 *
 * \section	module1_methods	Descripción
 *
 *
 * ***Prototipo:*** int main(int argc, char * argc[])
 * \param[in] 	argc	Número de argumentos de la llamada a la función
 * \param[in]   argc[]  Lista de cadenas alfanuméricas de entradas a la función
 * \return		Retorna un entero: 0: OK -1: KO
 *
 *  Esta es la función principal de la aplicación. Los parámetros de entrada son los nombres
 *  de las funciones de la librería que se quiere verificar:
 *
 *  funcion_1
 *  funcion_2
 *
 * La función ejecuta el script de prueba de cada una de las funciones y retorna por pantalla
 * el resultado SUCCESS o FAULT de cada test, y genera un fichero de texto con el resultado.
 *
 * \copyright  ZIGOR R&D AIE
 *
 */


#include <stdio.h>
#include <stdlib.h>

int main()
{
    printf("Hello world!\n");
    return 0;
}
