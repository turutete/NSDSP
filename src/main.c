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
 * gcc -DDEBUG -o test_nsdsp main.c nsdsp.c rt_momentos.c test_rt_momentos.c lagrange_halfband.c test_lagrange_halfband.c fir_filter.c test_fir_filter.c dwt.c test_dwt.c -lm
 * \endcode
 *
 * Para compilar en modo RELEASE:
 * \code
 * gcc -o nsdsp main.c nsdsp.c rt_momentos.c lagrange_halfband.c fir_filter.c dwt.c -lm
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
 *   RUN_RT_TESTS [label="Run_All_RT_Momentos_Tests()", fillcolor=lightblue];
 *   RUN_LAG_TESTS [label="Run_All_Lagrange_Tests()", fillcolor=lightblue];
 *   RUN_FIR_TESTS [label="Run_All_FIR_Tests()", fillcolor=lightblue];
 *   RUN_DWT_TESTS [label="Run_All_DWT_Tests()", fillcolor=lightblue];
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
 *   INIT_DEBUG -> RUN_RT_TESTS;
 *   RUN_RT_TESTS -> RUN_LAG_TESTS;
 *   RUN_LAG_TESTS -> RUN_FIR_TESTS;
 *   RUN_FIR_TESTS -> RUN_DWT_TESTS;
 *   RUN_DWT_TESTS -> CHECK_RESULT;
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
 * - Test_Lagrange_Halfband: Suite de pruebas del módulo de filtros de media banda
 *   - Test de parámetros inválidos
 *   - Test de diferentes órdenes de filtro
 *   - Test de simetría de coeficientes
 * - Test_FIR_Filter: Suite de pruebas del módulo de filtrado FIR general
 *   - Test de inicialización de filtros
 *   - Test de filtrado con señales conocidas
 *   - Test de manejo de errores
 * - Test_DWT: Suite de pruebas del módulo de transformada wavelet discreta
 *   - Test de inicialización de objetos DWT
 *   - Test de descomposición con diferentes señales
 *   - Test de descomposición multinivel
 *   - Test con diferentes tipos de filtros wavelet
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_main Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 20/07/2025 | Dr. Carlos Romero | 1 | Añadido soporte para tests unitarios |
 * | 03/08/2025 | Dr. Carlos Romero | 2 | Actualización documentación Doxygen según estándar |
 * | 04/08/2025 | Dr. Carlos Romero | 3 | Integración de tests de Lagrange Halfband |
 * | 14/08/2025 | Dr. Carlos Romero | 4 | Integración de tests de Interpolación/Decimación |
 * | 14/08/2025 | Dr. Carlos Romero | 5 | Cambio a tests de Descomposición Wavelet |
 * | 28/08/2025 | Dr. Carlos Romero | 6 | Integración de FIR Filter y DWT, eliminación wavelet_decim |
 *
 * \copyright ZGR R&D AIE
 */

#include <stdio.h>
#include <stdlib.h>
#include "nsdsp.h"

int main(int argc, char *argv[])
{
    int result = 0;
    int test_result;

#ifdef DEBUG
    printf("==============================================\n");
    printf("   NSDSP - MODO DEBUG - EJECUTANDO TESTS\n");
    printf("==============================================\n\n");

    /* Inicializar la librería NSDSP */
    Init_NSDSP();

    /* Ejecutar tests de RT_Momentos */
    test_result = Run_All_RT_Momentos_Tests();
    if (test_result != 0)
    {
        result = -1;
    }

    /* Ejecutar tests de Lagrange Halfband */
    test_result = Run_All_Lagrange_Tests();
    if (test_result != 0)
    {
        result = -1;
    }

    /* Ejecutar tests de FIR Filter */
    test_result = Run_All_FIR_Tests();
    if (test_result != 0)
    {
        result = -1;
    }

    /* Ejecutar tests de DWT */
    test_result = Run_All_DWT_Tests();
    if (test_result != 0)
    {
        result = -1;
    }

    /* Ejecutar tests de NSDSP Math */
    test_result = Run_All_NSDSP_Math_Tests();
    if (test_result != 0)
    {
        result = -1;
    }

    /* Ejecutar tests de ANN */
    test_result = Run_All_ANN_Tests();
    if (test_result != 0)
    {
        result = -1;
    }

    /* Aquí se pueden añadir más tests de otros módulos cuando estén disponibles */

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
    /* Código para modo RELEASE */
    printf("NSDSP - Non-Stationary Digital Signal Processing Library\n");
    printf("Copyright ZGR R&D AIE\n\n");

    /* Inicializar la librería */
    Init_NSDSP();

    /* Aquí iría el código de la aplicación en modo release */
    printf("Librería inicializada correctamente.\n");
    printf("Módulos disponibles:\n");
    printf("  - RT_Momentos: Cálculo de momentos estadísticos en tiempo real\n");
    printf("  - Lagrange Halfband: Filtros de media banda de Lagrange\n");
    printf("  - FIR_Filter: Filtrado FIR general\n");
    printf("  - DWT: Transformada Wavelet Discreta\n");

#endif

    return result;
}
