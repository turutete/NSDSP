/** \page main MAIN
 * \brief Módulo principal de pruebas de la librería NSDSP
 *
 * Este módulo ejecuta los scripts de prueba de todas las funciones de la librería
 * NSDSP (Non-Stationary Digital Signal Processing). En modo DEBUG, ejecuta
 * automáticamente todos los tests unitarios disponibles para verificar el correcto
 * funcionamiento de los módulos. En modo RELEASE, muestra un mensaje de bienvenida
 * e inicializa la librería para su uso.
 *
 * \section uso_main Uso del módulo
 *
 * El programa detecta automáticamente el modo de compilación:
 * - En modo DEBUG: Ejecuta todos los tests unitarios y reporta resultados
 * - En modo RELEASE: Inicializa la librería y muestra mensaje de estado
 *
 * Para compilar en modo DEBUG:
 * \code
 * gcc -DDEBUG -o test_nsdsp main.c nsdsp.c rt_momentos.c test_rt_momentos.c -lm
 * \endcode
 *
 * Para compilar en modo RELEASE:
 * \code
 * gcc -o nsdsp main.c nsdsp.c rt_momentos.c -lm
 * \endcode
 *
 * \section funciones_main Descripción de funciones
 *
 * \subsection main_func main
 * Función principal del programa que realiza las siguientes operaciones:
 *
 * \dot
 * digraph main_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="main()", fillcolor=lightgreen];
 *   DEBUG_CHECK [label="¿Modo DEBUG?", shape=diamond, fillcolor=lightyellow];
 *   INIT_DEBUG [label="Inicializar NSDSP", fillcolor=lightblue];
 *   RUN_TESTS [label="Run_All_RT_Momentos_Tests()", fillcolor=lightblue];
 *   CHECK_RESULT [label="¿Tests OK?", shape=diamond, fillcolor=lightyellow];
 *   SUCCESS_MSG [label="Mostrar éxito", fillcolor=lightgreen];
 *   FAIL_MSG [label="Mostrar fallo", fillcolor=lightcoral];
 *   INIT_RELEASE [label="Inicializar NSDSP", fillcolor=lightblue];
 *   RELEASE_MSG [label="Mostrar mensaje\nbienvenida", fillcolor=lightgreen];
 *   END [label="return result", fillcolor=lightgreen];
 *
 *   START -> DEBUG_CHECK;
 *   DEBUG_CHECK -> INIT_DEBUG [label="Sí"];
 *   DEBUG_CHECK -> INIT_RELEASE [label="No"];
 *   INIT_DEBUG -> RUN_TESTS;
 *   RUN_TESTS -> CHECK_RESULT;
 *   CHECK_RESULT -> SUCCESS_MSG [label="= 0"];
 *   CHECK_RESULT -> FAIL_MSG [label="≠ 0"];
 *   SUCCESS_MSG -> END;
 *   FAIL_MSG -> END;
 *   INIT_RELEASE -> RELEASE_MSG;
 *   RELEASE_MSG -> END;
 * }
 * \enddot
 *
 * En modo DEBUG, la función ejecuta todos los tests unitarios disponibles
 * y retorna 0 si todos pasan correctamente, o un valor negativo si alguno falla.
 *
 * \section tests_disponibles Tests disponibles
 * - Test_RT_Momentos: Suite completa de pruebas del módulo de cálculo de momentos
 *   - Test de inicialización
 *   - Test de suscripción/desuscripción de servicios
 *   - Test de cálculo con señales gaussianas
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_main Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 20/07/2025 | Dr. Carlos Romero | 1 | Añadido soporte para tests unitarios |
 * | 03/08/2025 | Dr. Carlos Romero | 2 | Actualización documentación Doxygen según estándar |
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
