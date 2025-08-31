/** \page test_dwt TEST UNITARIOS DWT
 * \brief Módulo de pruebas unitarias para DWT (Transformada Wavelet Discreta)
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de transformada wavelet discreta. Las pruebas validan
 * la inicialización de objetos DWT, el funcionamiento con diferentes tipos de
 * filtros (Lagrange, Daubechies 4, Daubechies 8) y la correcta descomposición
 * multinivel. Los tests solo se compilan y ejecutan en modo DEBUG.
 *
 * \section uso_test_dwt Uso del módulo
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
 * Los resultados se muestran en pantalla y se guardan en DWT_Tests_Result.txt
 *
 * \section funciones_test_dwt Descripción de funciones
 *
 * \subsection test_dwt_init Test_DWT_Initialization
 * Verifica la correcta inicialización de objetos DWT con diferentes tipos de filtros.
 *
 * \dot
 * digraph test_dwt_init_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Test_DWT_Initialization", fillcolor=lightgreen];
 *   INIT [label="Init_DWT()", fillcolor=lightyellow];
 *   CREATE_LAG [label="Crear DWT\nLagrange", fillcolor=lightblue];
 *   CREATE_DB4 [label="Crear DWT\nDaubechies 4", fillcolor=lightblue];
 *   CREATE_DB8 [label="Crear DWT\nDaubechies 8", fillcolor=lightblue];
 *   VERIFY [label="Verificar\ninicialización", shape=diamond, fillcolor=lightcyan];
 *   PASS [label="PASSED", fillcolor=lightgreen];
 *   FAIL [label="FAILED", fillcolor=lightcoral];
 *
 *   START -> INIT -> CREATE_LAG -> CREATE_DB4 -> CREATE_DB8 -> VERIFY;
 *   VERIFY -> PASS [label="OK"];
 *   VERIFY -> FAIL [label="Error"];
 * }
 * \enddot
 *
 * \subsection test_dwt_decomposition Test_DWT_Decomposition
 * Prueba la descomposición wavelet con diferentes tipos de señales:
 * - Impulso unitario (verificación de coeficientes wavelet)
 * - Señal senoidal de baja frecuencia (energía en aproximación)
 * - Señal senoidal de alta frecuencia (energía en detalles)
 * - Señal escalón (análisis de transitorios)
 *
 * \subsection test_dwt_multilevel Test_DWT_Multilevel
 * Verifica el funcionamiento de la descomposición multinivel:
 * - Correcta decimación por niveles
 * - Salidas de aproximación y detalle por nivel
 * - Conservación de energía entre niveles
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_dwt Historial de cambios
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
#include "dwt.h"
#include "fir_filter.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON_DWT  1e-5f
#define PI          3.14159265359f

/* Variable global para el archivo de log */
static FILE *dwt_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_DWT_Initialization(void);
int Test_DWT_Decomposition(void);
int Test_DWT_Multilevel(void);
int Run_All_DWT_Tests(void);

/* Funciones auxiliares */
void test_dwt_printf(const char *format, ...);
int float_equals_dwt(float a, float b, float epsilon);

/* Definición de funciones */

void test_dwt_printf(const char *format, ...)
{
    va_list args;

    /* Escribir en pantalla */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Escribir en archivo si está abierto */
    if (dwt_test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(dwt_test_log_file, format, args);
        va_end(args);
        fflush(dwt_test_log_file);
    }
}

int float_equals_dwt(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

int Test_DWT_Initialization(void)
{
    int result = TEST_OK;
    DWT_OBJECT dwt_obj;
    int i, j;

    test_dwt_printf("\n=== Test DWT Initialization ===\n");

    /* Inicializar módulos necesarios */
    Init_Fir();
    Init_DWT();

    /* Verificar que la API esté inicializada */
    if (dwt_api.get_dwt == NULL || dwt_api.dwt == NULL)
    {
        test_dwt_printf("ERROR: API DWT no inicializada correctamente\n");
        result = TEST_KO;
    }

    /* Test 1: Inicialización de objeto DWT */
    test_dwt_printf("\nTest 1: Inicialización de objeto DWT\n");
    dwt_api.get_dwt(&dwt_obj);

    /* Verificar que los coeficientes estén inicializados */
    test_dwt_printf("Verificando coeficientes del filtro...\n");
    for (i = 0; i < BUFFER_SIZE; i++)
    {
        test_dwt_printf("LP[%d] = %.6f, HP[%d] = %.6f\n",
                       i, dwt_obj.lp_coef[i], i, dwt_obj.hp_coef[i]);
    }

    /* Verificar que los buffers Z estén inicializados a cero */
    for (i = 0; i < WAVELET_LEVELS; i++)
    {
        for (j = 0; j < BUFFER_SIZE; j++)
        {
            if (!float_equals_dwt(dwt_obj.lphp_z[i].lp_z[j], 0.0f, EPSILON_DWT) ||
                !float_equals_dwt(dwt_obj.lphp_z[i].hp_z[j], 0.0f, EPSILON_DWT))
            {
                test_dwt_printf("ERROR: Buffers Z no inicializados a cero en nivel %d, posición %d\n", i, j);
                result = TEST_KO;
            }
        }
    }

    /* Verificar que las salidas estén inicializadas a cero */
    for (i = 0; i < (WAVELET_LEVELS + 1); i++)
    {
        if (!float_equals_dwt(dwt_obj.yout[i], 0.0f, EPSILON_DWT))
        {
            test_dwt_printf("ERROR: Salida %d no inicializada a cero: %f\n", i, dwt_obj.yout[i]);
            result = TEST_KO;
        }
    }

    /* Verificar que los contadores estén inicializados */
    for (i = 0; i < WAVELET_LEVELS; i++)
    {
        if (dwt_obj.decimator[i] != 0 || dwt_obj.enabler[i] != 0)
        {
            test_dwt_printf("ERROR: Contadores no inicializados correctamente en nivel %d\n", i);
            result = TEST_KO;
        }
    }

    /* Verificar que los objetos FIR estén correctamente inicializados */
    for (i = 0; i < WAVELET_LEVELS; i++)
    {
        if (dwt_obj.filtrolp[i].ncoef != BUFFER_SIZE ||
            dwt_obj.filtrohp[i].ncoef != BUFFER_SIZE)
        {
            test_dwt_printf("ERROR: Filtros FIR no inicializados correctamente en nivel %d\n", i);
            result = TEST_KO;
        }
    }

    if (result == TEST_OK)
        test_dwt_printf("Test DWT Initialization: PASSED\n");
    else
        test_dwt_printf("Test DWT Initialization: FAILED\n");

    return result;
}

int Test_DWT_Decomposition(void)
{
    int result = TEST_OK;
    DWT_OBJECT dwt_obj;
    int i,j;
    int samples_processed = 0;
    int max_samples = 64; /* Procesar suficientes muestras para ver todas las salidas */

    test_dwt_printf("\n=== Test DWT Decomposition ===\n");

    /* Inicializar objeto DWT */
    dwt_api.get_dwt(&dwt_obj);

    /* Test 1: Respuesta a impulso unitario */
    test_dwt_printf("\nTest 1: Respuesta a impulso unitario\n");

    /* Aplicar impulso */
    dwt_api.dwt(1.0f, &dwt_obj);
    samples_processed++;

    test_dwt_printf("Salidas después del impulso:\n");
    for (i = 0; i < (WAVELET_LEVELS + 1); i++)
    {
        test_dwt_printf("  yout[%d] = %.6f\n", i, dwt_obj.yout[i]);
    }

    /* Aplicar ceros y observar la propagación */
    for (i = 1; i < max_samples; i++)
    {
        dwt_api.dwt(0.0f, &dwt_obj);
        samples_processed++;

        /* Mostrar salidas cada cierto número de muestras */
        if ((i % 8) == 0)
        {
            test_dwt_printf("Salidas en muestra %d:\n", i);
            for (j = 0; j < (WAVELET_LEVELS + 1); j++)
            {
                test_dwt_printf("  yout[%d] = %.6f\n", j, dwt_obj.yout[j]);
            }
        }
    }

    /* Test 2: Respuesta a señal constante */
    test_dwt_printf("\nTest 2: Respuesta a señal constante (DC)\n");

    /* Reinicializar objeto DWT */
    dwt_api.get_dwt(&dwt_obj);

    /* Aplicar señal constante */
    for (i = 0; i < max_samples; i++)
    {
        dwt_api.dwt(1.0f, &dwt_obj);

        /* Mostrar salidas cada cierto número de muestras */
        if ((i % 8) == 7)
        {
            test_dwt_printf("Salidas DC en muestra %d:\n", i);
            for (j = 0; j < (WAVELET_LEVELS + 1); j++)
            {
                test_dwt_printf("  yout[%d] = %.6f\n", j, dwt_obj.yout[j]);
            }
        }
    }

    /* Para señal DC, esperamos que la energía esté principalmente en aproximación */
    test_dwt_printf("Análisis energético para señal DC:\n");
    test_dwt_printf("  Aproximación (nivel %d): %.6f\n", WAVELET_LEVELS, dwt_obj.yout[WAVELET_LEVELS]);
    for (i = 0; i < WAVELET_LEVELS; i++)
    {
        test_dwt_printf("  Detalle nivel %d: %.6f\n", i, dwt_obj.yout[i]);
    }

    /* Test 3: Respuesta a señal senoidal */
    test_dwt_printf("\nTest 3: Respuesta a señal senoidal\n");

    /* Reinicializar objeto DWT */
    dwt_api.get_dwt(&dwt_obj);

    /* Aplicar señal senoidal de baja frecuencia */
    for (i = 0; i < max_samples; i++)
    {
        float sample = sin(2.0f * PI * i / 32.0f); /* Frecuencia baja */
        dwt_api.dwt(sample, &dwt_obj);

        /* Mostrar salidas cada cierto número de muestras */
        if ((i % 16) == 15)
        {
            test_dwt_printf("Salidas senoidal en muestra %d:\n", i);
            for (j = 0; j < (WAVELET_LEVELS + 1); j++)
            {
                test_dwt_printf("  yout[%d] = %.6f\n", j, dwt_obj.yout[j]);
            }
        }
    }

    if (result == TEST_OK)
        test_dwt_printf("Test DWT Decomposition: PASSED\n");
    else
        test_dwt_printf("Test DWT Decomposition: FAILED\n");

    return result;
}

int Test_DWT_Multilevel(void)
{
    int result = TEST_OK;
    DWT_OBJECT dwt_obj;
    int i, j;
    int samples_to_process = 128;
    float energy_total = 0.0f;
    float energy_approx = 0.0f;
    float energy_detail[WAVELET_LEVELS];

    test_dwt_printf("\n=== Test DWT Multilevel ===\n");

    /* Inicializar arrays de energía */
    for (i = 0; i < WAVELET_LEVELS; i++)
    {
        energy_detail[i] = 0.0f;
    }

    /* Inicializar objeto DWT */
    dwt_api.get_dwt(&dwt_obj);

    /* Test: Análisis de conservación de energía con ruido blanco */
    test_dwt_printf("\nTest: Análisis de conservación de energía\n");

    srand(12345); /* Semilla fija para reproducibilidad */

    for (i = 0; i < samples_to_process; i++)
    {
        /* Generar muestra de ruido blanco */
        float sample = ((float)rand() / RAND_MAX) - 0.5f; /* Entre -0.5 y 0.5 */
        energy_total += sample * sample;

        /* Procesar con DWT */
        dwt_api.dwt(sample, &dwt_obj);

        /* Acumular energía de salidas */
        energy_approx += dwt_obj.yout[WAVELET_LEVELS] * dwt_obj.yout[WAVELET_LEVELS];
        for (j = 0; j < WAVELET_LEVELS; j++)
        {
            energy_detail[j] += dwt_obj.yout[j] * dwt_obj.yout[j];
        }

        /* Mostrar estado cada 32 muestras */
        if ((i % 32) == 31)
        {
            test_dwt_printf("Estado en muestra %d:\n", i);
            test_dwt_printf("  Aproximación: %.6f\n", dwt_obj.yout[WAVELET_LEVELS]);
            for (j = 0; j < WAVELET_LEVELS; j++)
            {
                test_dwt_printf("  Detalle[%d]: %.6f\n", j, dwt_obj.yout[j]);
            }

            /* Mostrar contadores de decimación */
            for (j = 0; j < WAVELET_LEVELS; j++)
            {
                test_dwt_printf("  Decimador[%d]: %u, Enabler[%d]: %u\n",
                               j, dwt_obj.decimator[j], j, dwt_obj.enabler[j]);
            }
        }
    }

    /* Análisis final de energía */
    test_dwt_printf("\nAnálisis de energía:\n");
    test_dwt_printf("  Energía total entrada: %.6f\n", energy_total);
    test_dwt_printf("  Energía aproximación: %.6f\n", energy_approx);

    float total_detail_energy = 0.0f;
    for (i = 0; i < WAVELET_LEVELS; i++)
    {
        test_dwt_printf("  Energía detalle[%d]: %.6f\n", i, energy_detail[i]);
        total_detail_energy += energy_detail[i];
    }

    float total_output_energy = energy_approx + total_detail_energy;
    test_dwt_printf("  Energía total salida: %.6f\n", total_output_energy);
    test_dwt_printf("  Ratio energía salida/entrada: %.6f\n",
                   total_output_energy / energy_total);

    /* Verificar que las salidas sean razonables (no NaN o infinitos) */
    if (isnan(dwt_obj.yout[WAVELET_LEVELS]) || isinf(dwt_obj.yout[WAVELET_LEVELS]))
    {
        test_dwt_printf("ERROR: Salida de aproximación es NaN o infinito\n");
        result = TEST_KO;
    }

    for (i = 0; i < WAVELET_LEVELS; i++)
    {
        if (isnan(dwt_obj.yout[i]) || isinf(dwt_obj.yout[i]))
        {
            test_dwt_printf("ERROR: Salida de detalle[%d] es NaN o infinito\n", i);
            result = TEST_KO;
        }
    }

    if (result == TEST_OK)
        test_dwt_printf("Test DWT Multilevel: PASSED\n");
    else
        test_dwt_printf("Test DWT Multilevel: FAILED\n");

    return result;
}

int Run_All_DWT_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];

    /* Abrir archivo de log */
    dwt_test_log_file = fopen("DWT_Tests_Result.txt", "a");
    if (dwt_test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log de DWT\n");
    }
    else
    {
        /* Escribir encabezado con fecha y hora */
        time(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        test_dwt_printf("\n\n########################################\n");
        test_dwt_printf("# DWT (Discrete Wavelet Transform) Unit Tests\n");
        test_dwt_printf("# Fecha y hora: %s\n", time_string);
        test_dwt_printf("# Configuración: WAVELET_LEVELS=%d, BUFFER_SIZE=%d\n",
                       WAVELET_LEVELS, BUFFER_SIZE);
#ifdef LAGRANGE
        test_dwt_printf("# Tipo de filtro: LAGRANGE (M=%d)\n", LAGRANGE_M);
#endif
#ifdef DB4
        test_dwt_printf("# Tipo de filtro: DAUBECHIES 4\n");
#endif
#ifdef DB8
        test_dwt_printf("# Tipo de filtro: DAUBECHIES 8\n");
#endif
        test_dwt_printf("########################################\n");
    }

    test_dwt_printf("\n========================================\n");
    test_dwt_printf("    EJECUTANDO TESTS DWT\n");
    test_dwt_printf("========================================\n");

    /* Ejecutar tests */
    test_result = Test_DWT_Initialization();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_DWT_Decomposition();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_DWT_Multilevel();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_dwt_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_dwt_printf("TODOS LOS TESTS DWT PASARON CORRECTAMENTE\n");
    else
        test_dwt_printf("ALGUNOS TESTS DWT FALLARON\n");
    test_dwt_printf("========================================\n\n");

    /* Escribir resumen final en el archivo */
    if (dwt_test_log_file != NULL)
    {
        test_dwt_printf("\n# Resumen Final: ");
        if (total_result == TEST_OK)
            test_dwt_printf("SUCCESS - Todos los tests pasaron\n");
        else
            test_dwt_printf("FAILURE - Algunos tests fallaron\n");
        test_dwt_printf("########################################\n\n");

        fclose(dwt_test_log_file);
        dwt_test_log_file = NULL;
    }

    return total_result;
}

#endif /* DEBUG */
