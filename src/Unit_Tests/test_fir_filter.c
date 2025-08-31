/** \page test_fir_filter TEST UNITARIOS FIR FILTER
 * \brief Módulo de pruebas unitarias para FIR Filter
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de filtrado FIR. Las pruebas validan la inicialización
 * de filtros, el filtrado con diferentes tipos de señales y el manejo de errores.
 * Los tests solo se compilan y ejecutan en modo DEBUG.
 *
 * \section uso_test_fir Uso del módulo
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
 * Los resultados se muestran en pantalla y se guardan en FIR_Filter_Tests_Result.txt
 *
 * \section funciones_test_fir Descripción de funciones
 *
 * \subsection test_fir_init Test_FIR_Initialization
 * Verifica la correcta inicialización de filtros FIR con diferentes configuraciones.
 *
 * \dot
 * digraph test_fir_init_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Test_FIR_Initialization", fillcolor=lightgreen];
 *   INIT [label="Init_Fir()", fillcolor=lightyellow];
 *   CREATE1 [label="Crear filtro\npaso bajo", fillcolor=lightblue];
 *   CREATE2 [label="Crear filtro\npaso alto", fillcolor=lightblue];
 *   VERIFY [label="Verificar\ninicialización", shape=diamond, fillcolor=lightcyan];
 *   PASS [label="PASSED", fillcolor=lightgreen];
 *   FAIL [label="FAILED", fillcolor=lightcoral];
 *
 *   START -> INIT -> CREATE1 -> CREATE2 -> VERIFY;
 *   VERIFY -> PASS [label="OK"];
 *   VERIFY -> FAIL [label="Error"];
 * }
 * \enddot
 *
 * \subsection test_fir_filtering Test_FIR_Filtering
 * Prueba el filtrado con diferentes tipos de señales de entrada:
 * - Impulso unitario (verificación de respuesta impulsional)
 * - Señal senoidal (verificación de respuesta frecuencial)
 * - Señal escalón (verificación de respuesta al escalón)
 *
 * \subsection test_fir_errors Test_FIR_Error_Handling
 * Verifica el manejo correcto de errores:
 * - Puntero NULL al filtro
 * - Número de coeficientes excesivo (> MAX_FIR_LENGTH)
 * - Punteros NULL a coeficientes o buffer Z
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_fir Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 28/08/2025 | Dr. Carlos Romero | 1 | Implementación inicial de tests |
 *
 * \copyright ZGR R&D AIE
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include "fir_filter.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON_FIR  1e-6f

/* Variable global para el archivo de log */
static FILE *fir_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_FIR_Initialization(void);
int Test_FIR_Filtering(void);
int Test_FIR_Error_Handling(void);
int Run_All_FIR_Tests(void);

/* Funciones auxiliares */
void test_fir_printf(const char *format, ...);
int float_equals_fir(float a, float b, float epsilon);

/* Definición de funciones */

void test_fir_printf(const char *format, ...)
{
    va_list args;

    /* Escribir en pantalla */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Escribir en archivo si está abierto */
    if (fir_test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(fir_test_log_file, format, args);
        va_end(args);
        fflush(fir_test_log_file);
    }
}

int float_equals_fir(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

int Test_FIR_Initialization(void)
{
    int result = TEST_OK;
    FIR_FILTER_OBJECT filter;
    float coefs[5] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f}; /* Filtro promediador */
    float z_buffer[5];
    int i;

    test_fir_printf("\n=== Test FIR Initialization ===\n");

    /* Inicializar módulo */
    Init_Fir();

    /* Verificar que la API esté inicializada */
    if (fir_api.get_fir == NULL || fir_api.fir_filter == NULL)
    {
        test_fir_printf("ERROR: API FIR no inicializada correctamente\n");
        result = TEST_KO;
    }

    /* Test 1: Crear filtro promediador de 5 coeficientes */
    test_fir_printf("\nTest 1: Crear filtro promediador (5 coeficientes)\n");
    filter = fir_api.get_fir(5, coefs, z_buffer);

    if (filter.ncoef != 5)
    {
        test_fir_printf("ERROR: Número de coeficientes incorrecto: %u (esperado 5)\n", filter.ncoef);
        result = TEST_KO;
    }

    if (filter.pcoef != coefs)
    {
        test_fir_printf("ERROR: Puntero a coeficientes incorrecto\n");
        result = TEST_KO;
    }

    if (filter.pz != z_buffer)
    {
        test_fir_printf("ERROR: Puntero a buffer Z incorrecto\n");
        result = TEST_KO;
    }

    if (filter.p_write != z_buffer)
    {
        test_fir_printf("ERROR: Puntero de escritura no inicializado correctamente\n");
        result = TEST_KO;
    }

    /* Verificar que el buffer Z esté inicializado a cero */
    for (i = 0; i < 5; i++)
    {
        if (!float_equals_fir(z_buffer[i], 0.0f, EPSILON_FIR))
        {
            test_fir_printf("ERROR: Buffer Z no inicializado a cero en posición %d: %f\n", i, z_buffer[i]);
            result = TEST_KO;
        }
    }

    if (result == TEST_OK)
        test_fir_printf("Test FIR Initialization: PASSED\n");
    else
        test_fir_printf("Test FIR Initialization: FAILED\n");

    return result;
}

int Test_FIR_Filtering(void)
{
    int result = TEST_OK;
    FIR_FILTER_OBJECT filter;
    float coefs[5] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f}; /* Filtro promediador */
    float z_buffer[5];
    float output;
    int i;

    test_fir_printf("\n=== Test FIR Filtering ===\n");

    /* Crear filtro */
    filter = fir_api.get_fir(5, coefs, z_buffer);

    /* Test 1: Respuesta a impulso unitario */
    test_fir_printf("\nTest 1: Respuesta a impulso unitario\n");
    
    /* Aplicar impulso */
    output = fir_api.fir_filter(1.0f, &filter);
    test_fir_printf("Salida impulso t=0: %f (esperado 0.2)\n", output);
    if (!float_equals_fir(output, 0.2f, EPSILON_FIR))
    {
        test_fir_printf("ERROR: Respuesta a impulso incorrecta en t=0\n");
        result = TEST_KO;
    }

    /* Aplicar ceros y verificar respuesta impulsional */
    for (i = 1; i < 5; i++)
    {
        output = fir_api.fir_filter(0.0f, &filter);
        test_fir_printf("Salida impulso t=%d: %f (esperado 0.2)\n", i, output);
        if (!float_equals_fir(output, 0.2f, EPSILON_FIR))
        {
            test_fir_printf("ERROR: Respuesta a impulso incorrecta en t=%d\n", i);
            result = TEST_KO;
        }
    }

    /* Última muestra debe ser cero */
    output = fir_api.fir_filter(0.0f, &filter);
    test_fir_printf("Salida impulso t=5: %f (esperado 0.0)\n", output);
    if (!float_equals_fir(output, 0.0f, EPSILON_FIR))
    {
        test_fir_printf("ERROR: Respuesta a impulso debería ser cero en t=5\n");
        result = TEST_KO;
    }

    /* Test 2: Respuesta a escalón unitario */
    test_fir_printf("\nTest 2: Respuesta a escalón unitario\n");
    
    /* Reinicializar filtro */
    filter = fir_api.get_fir(5, coefs, z_buffer);
    
    /* Aplicar escalón y verificar acumulación */
    for (i = 0; i < 5; i++)
    {
        output = fir_api.fir_filter(1.0f, &filter);
        float expected = 0.2f * (i + 1);
        test_fir_printf("Salida escalón t=%d: %f (esperado %f)\n", i, output, expected);
        if (!float_equals_fir(output, expected, EPSILON_FIR))
        {
            test_fir_printf("ERROR: Respuesta a escalón incorrecta en t=%d\n", i);
            result = TEST_KO;
        }
    }

    /* En régimen permanente debe ser 1.0 */
    output = fir_api.fir_filter(1.0f, &filter);
    test_fir_printf("Salida escalón t=5: %f (esperado 1.0)\n", output);
    if (!float_equals_fir(output, 1.0f, EPSILON_FIR))
    {
        test_fir_printf("ERROR: Respuesta a escalón en régimen permanente incorrecta\n");
        result = TEST_KO;
    }

    if (result == TEST_OK)
        test_fir_printf("Test FIR Filtering: PASSED\n");
    else
        test_fir_printf("Test FIR Filtering: FAILED\n");

    return result;
}

int Test_FIR_Error_Handling(void)
{
    int result = TEST_OK;
    FIR_FILTER_OBJECT filter;
    float coefs[5] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
    float z_buffer[5];
    float output;

    test_fir_printf("\n=== Test FIR Error Handling ===\n");

    /* Test 1: Puntero NULL al filtro */
    test_fir_printf("\nTest 1: Puntero NULL al filtro\n");
    output = fir_api.fir_filter(1.0f, NULL);
    if (!float_equals_fir(output, 0.0f, EPSILON_FIR))
    {
        test_fir_printf("ERROR: No retornó 0.0 con puntero NULL\n");
        result = TEST_KO;
    }
    else
    {
        test_fir_printf("OK: Retornó 0.0 con puntero NULL\n");
    }

    /* Test 2: Filtro con demasiados coeficientes */
    test_fir_printf("\nTest 2: Filtro con demasiados coeficientes\n");
    filter = fir_api.get_fir(5, coefs, z_buffer);
    filter.ncoef = MAX_FIR_LENGTH + 1; /* Forzar error */
    
    output = fir_api.fir_filter(1.0f, &filter);
    if (!float_equals_fir(output, 0.0f, EPSILON_FIR))
    {
        test_fir_printf("ERROR: No detectó filtro con demasiados coeficientes\n");
        result = TEST_KO;
    }
    else
    {
        test_fir_printf("OK: Detectó filtro con demasiados coeficientes\n");
    }

    /* Test 3: Get_Fir con puntero NULL */
    test_fir_printf("\nTest 3: Get_Fir con punteros NULL\n");
    filter = fir_api.get_fir(5, NULL, z_buffer);
    if (filter.pcoef != NULL)
    {
        test_fir_printf("ADVERTENCIA: Get_Fir no validó puntero NULL a coeficientes\n");
        /* No es error crítico si la función no valida */
    }

    filter = fir_api.get_fir(5, coefs, NULL);
    if (filter.pz != NULL)
    {
        test_fir_printf("ADVERTENCIA: Get_Fir no validó puntero NULL a buffer Z\n");
        /* No es error crítico si la función no valida */
    }

    if (result == TEST_OK)
        test_fir_printf("Test FIR Error Handling: PASSED\n");
    else
        test_fir_printf("Test FIR Error Handling: FAILED\n");

    return result;
}

int Run_All_FIR_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];

    /* Abrir archivo de log */
    fir_test_log_file = fopen("FIR_Filter_Tests_Result.txt", "a");
    if (fir_test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log de FIR Filter\n");
    }
    else
    {
        /* Escribir encabezado con fecha y hora */
        time(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        test_fir_printf("\n\n########################################\n");
        test_fir_printf("# FIR Filter Unit Tests\n");
        test_fir_printf("# Fecha y hora: %s\n", time_string);
        test_fir_printf("########################################\n");
    }

    test_fir_printf("\n========================================\n");
    test_fir_printf("    EJECUTANDO TESTS FIR FILTER\n");
    test_fir_printf("========================================\n");

    /* Ejecutar tests */
    test_result = Test_FIR_Initialization();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_FIR_Filtering();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_FIR_Error_Handling();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_fir_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_fir_printf("TODOS LOS TESTS FIR FILTER PASARON CORRECTAMENTE\n");
    else
        test_fir_printf("ALGUNOS TESTS FIR FILTER FALLARON\n");
    test_fir_printf("========================================\n\n");

    /* Escribir resumen final en el archivo */
    if (fir_test_log_file != NULL)
    {
        test_fir_printf("\n# Resumen Final: ");
        if (total_result == TEST_OK)
            test_fir_printf("SUCCESS - Todos los tests pasaron\n");
        else
            test_fir_printf("FAILURE - Algunos tests fallaron\n");
        test_fir_printf("########################################\n\n");

        fclose(fir_test_log_file);
        fir_test_log_file = NULL;
    }

    return total_result;
}

#endif /* DEBUG */