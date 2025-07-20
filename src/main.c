/** \page main.c
 * \brief Módulo principal de pruebas de la librería NSDSP
 *
 * Este módulo ejecuta los scripts de prueba de todas las funciones de la librería
 * NSDSP (Non-Stationary Digital Signal Processing). En modo DEBUG, ejecuta
 * automáticamente todos los tests unitarios disponibles.
 *
 * \section funciones_main Funciones disponibles
 *
 * \subsection main_func main
 * Función principal que:
 * - En modo DEBUG: Ejecuta todos los tests unitarios disponibles
 * - En modo RELEASE: Muestra mensaje de bienvenida
 *
 * \section uso_main Uso del programa
 *
 * El programa detecta automáticamente el modo de compilación:
 * - DEBUG: Ejecuta todos los tests unitarios e informa de los resultados
 * - RELEASE: Ejecuta la aplicación normal sin tests
 *
 * \section tests_disponibles Tests disponibles
 * - Test_RT_Momentos: Pruebas del módulo de cálculo de momentos estadísticos
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_main Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 20/07/2025 | Dr. Carlos Romero | 1 | Añadido soporte para tests unitarios |
 *
 * \copyright ZGR R&D AIE
 */

#include <stdio.h>
#include <stdlib.h>
#include "nsdsp.h"

int main(int argc, char *argv[])
{
    int result = 0;

#ifdef DEBUG
    printf("==============================================\n");
    printf("   NSDSP - MODO DEBUG - EJECUTANDO TESTS\n");
    printf("==============================================\n\n");

    // Inicializar la librería NSDSP
    Init_NSDSP();

    // Ejecutar todos los tests disponibles
    result = Run_All_RT_Momentos_Tests();

    // Aquí se pueden añadir más tests de otros módulos cuando estén disponibles
    // result |= Run_All_Other_Module_Tests();

    if (result == 0)
    {
        printf("\n==============================================\n");
        printf("   TODOS LOS TESTS COMPLETADOS CON ÉXITO\n");
        printf("==============================================\n");
    }
    else
    {
        printf("\n==============================================\n");
        printf("   ATENCIÓN: ALGUNOS TESTS FALLARON\n");
        printf("==============================================\n");
    }

#else
    // Código para modo RELEASE
    printf("NSDSP - Non-Stationary Digital Signal Processing Library\n");
    printf("Copyright ZGR R&D AIE\n\n");

    // Inicializar la librería
    Init_NSDSP();

    // Aquí iría el código de la aplicación en modo release
    printf("Librería inicializada correctamente.\n");

#endif

    return result;
}
