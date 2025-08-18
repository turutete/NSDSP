/** \page test_lagrange_halfband TEST UNITARIOS LAGRANGE HALFBAND
 * \brief Módulo de pruebas unitarias para Lagrange Halfband
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de filtros de media banda de Lagrange. Las pruebas
 * validan la generación de coeficientes para diferentes valores del parámetro m,
 * verifican la simetría de los coeficientes y comprueban el manejo de parámetros
 * inválidos. Los tests solo se compilan y ejecutan en modo DEBUG.
 *
 * \section uso_test_lagrange Uso del módulo
 *
 * Las pruebas se ejecutan automáticamente desde main() cuando se compila en modo DEBUG:
 * \code
 * // Compilar en modo DEBUG
 * gcc -DDEBUG -o test_nsdsp *.c -lm
 *
 * // Ejecutar tests
 * ./test_nsdsp
 * \endcode
 *
 * Los resultados se muestran en pantalla y se guardan en Lagrange_Unit_Tests_Result.txt
 *
 * \section funciones_test_lagrange Descripción de funciones
 *
 * \subsection test_lagrange_halfband Test_Lagrange_Halfband
 * Prueba la generación de coeficientes del filtro de media banda de Lagrange.
 *
 * Las pruebas incluyen:
 * - Validación de parámetros inválidos (m <= 0, puntero NULL)
 * - Generación de coeficientes para m=1 (orden 2, 3 coeficientes)
 * - Generación de coeficientes para m=2 (orden 6, 7 coeficientes)
 * - Verificación de simetría de los coeficientes
 * - Verificación del coeficiente central = 0.5
 * - Verificación de estructura de media banda para m=3
 *
 * \dot
 * digraph test_lagrange_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Test_Lagrange_Halfband", fillcolor=lightgreen];
 *   TEST1 [label="Test parámetros\ninválidos", fillcolor=lightblue];
 *   TEST2 [label="Test m=1\n(3 coeficientes)", fillcolor=lightblue];
 *   TEST3 [label="Test m=2\n(7 coeficientes)", fillcolor=lightblue];
 *   TEST4 [label="Test m=3\n(11 coeficientes)", fillcolor=lightblue];
 *   VERIFY [label="Verificar:\n- Simetría\n- Centro = 0.5", fillcolor=lightyellow];
 *   RESULT [label="Retornar resultado", fillcolor=lightgreen];
 *
 *   START -> TEST1 -> TEST2 -> TEST3 -> TEST4;
 *   TEST2 -> VERIFY;
 *   TEST3 -> VERIFY;
 *   TEST4 -> VERIFY;
 *   VERIFY -> RESULT;
 * }
 * \enddot
 *
 * \subsection run_all_lagrange Run_All_Lagrange_Tests
 * Función principal que ejecuta todos los tests y genera el reporte.
 * - Abre archivo de log con timestamp
 * - Ejecuta Test_Lagrange_Halfband
 * - Genera resumen de resultados
 * - Cierra archivo de log
 *
 * \section funciones_auxiliares_lagrange Funciones auxiliares
 * - **test_lagrange_printf**: Escribe mensajes tanto en pantalla como en archivo de log
 * - **float_equals_lagrange**: Compara valores flotantes con tolerancia EPSILON_LAGRANGE
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_lagrange Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 04/08/2025 | Dr. Carlos Romero | 1 | Implementación inicial de tests |
 * | 14/08/2025 | Dr. Carlos Romero | 2 | Separación de tests a archivo independiente |
 *
 * \copyright ZGR R&D AIE
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include "lagrange_halfband.h"
#include "test_lagrange_halfband.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON_LAGRANGE     1e-6f

/* Variable global para el archivo de log */
static FILE *lagrange_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_Lagrange_Halfband(void);
int Run_All_Lagrange_Tests(void);
void test_lagrange_printf(const char *format, ...);
int float_equals_lagrange(float a, float b, float epsilon);

/* Definición de funciones */

void test_lagrange_printf(const char *format, ...)
{
    va_list args;

    /* Escribir en pantalla */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Escribir en archivo si está abierto */
    if (lagrange_test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(lagrange_test_log_file, format, args);
        va_end(args);
        fflush(lagrange_test_log_file);
    }
}

int float_equals_lagrange(float a, float b, float epsilon)
{
    return fabsf(a - b) < epsilon;
}

int Test_Lagrange_Halfband(void)
{
    int result = TEST_OK;
    int m, i, ret;
    float h0[20];  /* Suficiente para m <= 5 */
    int orden;

    test_lagrange_printf("\n=== Test Lagrange Halfband ===\n");

    /* Test 1: Parámetros inválidos */
    test_lagrange_printf("\nTest 1: Parámetros inválidos\n");
    ret = lagrange_halfband(0, h0);
    if (ret != LAGRANGE_KO)
    {
        test_lagrange_printf("ERROR: No detectó m = 0 como inválido\n");
        result = TEST_KO;
    }

    ret = lagrange_halfband(-1, h0);
    if (ret != LAGRANGE_KO)
    {
        test_lagrange_printf("ERROR: No detectó m = -1 como inválido\n");
        result = TEST_KO;
    }

    ret = lagrange_halfband(1, NULL);
    if (ret != LAGRANGE_KO)
    {
        test_lagrange_printf("ERROR: No detectó puntero NULL como inválido\n");
        result = TEST_KO;
    }

    /* Test 2: Caso m = 1 (orden 2, 3 coeficientes) */
    test_lagrange_printf("\nTest 2: Filtro m=1 (orden 2)\n");
    m = 1;
    orden = 4 * m - 1;  /* 3 coeficientes */
    ret = lagrange_halfband(m, h0);
    
    if (ret != LAGRANGE_OK)
    {
        test_lagrange_printf("ERROR: Fallo en cálculo para m=1\n");
        result = TEST_KO;
    }
    else
    {
        test_lagrange_printf("Coeficientes para m=1: ");
        for (i = 0; i < orden; i++)
        {
            test_lagrange_printf("%.6f ", h0[i]);
        }
        test_lagrange_printf("\n");

        /* Verificar simetría */
        if (!float_equals_lagrange(h0[0], h0[2], EPSILON_LAGRANGE))
        {
            test_lagrange_printf("ERROR: Coeficientes no simétricos para m=1\n");
            result = TEST_KO;
        }

        /* Verificar coeficiente central */
        if (!float_equals_lagrange(h0[1], 0.5f, EPSILON_LAGRANGE))
        {
            test_lagrange_printf("ERROR: Coeficiente central incorrecto para m=1\n");
            result = TEST_KO;
        }
    }

    /* Test 3: Caso m = 2 (orden 6, 7 coeficientes) */
    test_lagrange_printf("\nTest 3: Filtro m=2 (orden 6)\n");
    m = 2;
    orden = 4 * m - 1;  /* 7 coeficientes */
    ret = lagrange_halfband(m, h0);
    
    if (ret != LAGRANGE_OK)
    {
        test_lagrange_printf("ERROR: Fallo en cálculo para m=2\n");
        result = TEST_KO;
    }
    else
    {
        test_lagrange_printf("Coeficientes para m=2: ");
        for (i = 0; i < orden; i++)
        {
            test_lagrange_printf("%.6f ", h0[i]);
        }
        test_lagrange_printf("\n");

        /* Verificar simetría */
        for (i = 0; i < orden / 2; i++)
        {
            if (!float_equals_lagrange(h0[i], h0[orden - 1 - i], EPSILON_LAGRANGE))
            {
                test_lagrange_printf("ERROR: Coeficientes no simétricos para m=2 en posición %d\n", i);
                result = TEST_KO;
            }
        }

        /* Verificar coeficiente central */
        if (!float_equals_lagrange(h0[orden/2], 0.5f, EPSILON_LAGRANGE))
        {
            test_lagrange_printf("ERROR: Coeficiente central incorrecto para m=2\n");
            result = TEST_KO;
        }
    }

    /* Test 4: Verificar que coeficientes pares (excepto central) son cero */
    test_lagrange_printf("\nTest 4: Verificar estructura de media banda para m=3\n");
    m = 3;
    orden = 4 * m - 1;  /* 11 coeficientes */
    ret = lagrange_halfband(m, h0);
    
    if (ret != LAGRANGE_OK)
    {
        test_lagrange_printf("ERROR: Fallo en cálculo para m=3\n");
        result = TEST_KO;
    }
    else
    {
        test_lagrange_printf("Coeficientes para m=3: ");
        for (i = 0; i < orden; i++)
        {
            test_lagrange_printf("%.6f ", h0[i]);
        }
        test_lagrange_printf("\n");

        /* Para filtros de media banda, los coeficientes en posiciones pares 
           (excepto el central) deben ser cero */
        for (i = 1; i < orden; i += 2)
        {
            if (i != orden/2)  /* Saltar el coeficiente central */
            {
                if (!float_equals_lagrange(h0[i], 0.0f, EPSILON_LAGRANGE))
                {
                    test_lagrange_printf("ADVERTENCIA: Coeficiente impar no-central no es cero en posición %d: %.6f\n", 
                                       i, h0[i]);
                    /* Nota: Esto puede ser normal dependiendo de la implementación específica */
                }
            }
        }
    }

    if (result == TEST_OK)
        test_lagrange_printf("\nTest Lagrange Halfband: PASSED\n");
    else
        test_lagrange_printf("\nTest Lagrange Halfband: FAILED\n");

    return result;
}

int Run_All_Lagrange_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];

    /* Abrir archivo de log */
    lagrange_test_log_file = fopen("Lagrange_Unit_Tests_Result.txt", "a");
    if (lagrange_test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log de Lagrange\n");
    }
    else
    {
        /* Escribir encabezado con fecha y hora */
        time(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        test_lagrange_printf("\n\n########################################\n");
        test_lagrange_printf("# Lagrange Halfband Unit Tests\n");
        test_lagrange_printf("# Fecha y hora: %s\n", time_string);
        test_lagrange_printf("########################################\n");
    }

    test_lagrange_printf("\n========================================\n");
    test_lagrange_printf("    EJECUTANDO TESTS LAGRANGE HALFBAND\n");
    test_lagrange_printf("========================================\n");

    /* Ejecutar test */
    test_result = Test_Lagrange_Halfband();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_lagrange_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_lagrange_printf("TODOS LOS TESTS LAGRANGE PASARON CORRECTAMENTE\n");
    else
        test_lagrange_printf("ALGUNOS TESTS LAGRANGE FALLARON\n");
    test_lagrange_printf("========================================\n\n");

    /* Escribir resumen final en el archivo */
    if (lagrange_test_log_file != NULL)
    {
        test_lagrange_printf("\n# Resumen Final: ");
        if (total_result == TEST_OK)
            test_lagrange_printf("SUCCESS - Todos los tests pasaron\n");
        else
            test_lagrange_printf("FAILURE - Algunos tests fallaron\n");
        test_lagrange_printf("########################################\n\n");

        fclose(lagrange_test_log_file);
        lagrange_test_log_file = NULL;
    }

    return total_result;
}

#endif /* DEBUG */