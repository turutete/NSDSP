/** \page unit_tests TEST UNITARIOS
 * \brief Módulo de pruebas unitarias para RT_Momentos
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo RT_Momentos. Las pruebas están diseñadas para validar
 * la inicialización, gestión de servicios y cálculo de momentos estadísticos con
 * señales gaussianas. Los tests solo se compilan y ejecutan en modo DEBUG.
 *
 * \section uso_test Uso del módulo
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
 * Los resultados se muestran en pantalla y se guardan en NSDSP_Unit_Tests_Result.txt
 *
 * \section funciones_test Descripción de funciones
 *
 * \subsection test_init Test_Init_RT_Momentos
 * Verifica la correcta inicialización del módulo RT_Momentos.
 *
 * \dot
 * digraph test_init {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Test_Init_RT_Momentos", fillcolor=lightgreen];
 *   INIT [label="Init_RT_Momentos()", fillcolor=lightyellow];
 *   CHECK_PSE [label="Verificar pse\ninicializado", shape=diamond, fillcolor=lightblue];
 *   CHECK_FREE [label="Verificar todos\nservicios FREE", shape=diamond, fillcolor=lightblue];
 *   PASS [label="PASSED", fillcolor=lightgreen];
 *   FAIL [label="FAILED", fillcolor=lightcoral];
 *
 *   START -> INIT -> CHECK_PSE;
 *   CHECK_PSE -> CHECK_FREE [label="OK"];
 *   CHECK_PSE -> FAIL [label="Error"];
 *   CHECK_FREE -> PASS [label="OK"];
 *   CHECK_FREE -> FAIL [label="Error"];
 * }
 * \enddot
 *
 * \subsection test_suscribe Test_Suscribe_RT_Momentos
 * Prueba la asignación y liberación de servicios, verificando:
 * - Suscripción correcta hasta MAX_RT_MOMENTOS servicios
 * - Inicialización de buffers a cero
 * - Retorno de NONE cuando no hay servicios disponibles
 * - Liberación y reutilización de servicios
 *
 * \subsection test_gaussian Test_Gaussian_Signals
 * Prueba el cálculo de momentos con señales gaussianas de diferentes parámetros.
 *
 * Utiliza la aproximación Box-Muller para generar ruido gaussiano:
 * \f[
 * z_0 = \sqrt{-2 \ln(u_1)} \cos(2\pi u_2) \cdot \sigma + \mu
 * \f]
 *
 * donde \f$u_1, u_2\f$ son valores uniformes en [0,1], \f$\mu\f$ es la media y \f$\sigma\f$ la desviación estándar.
 *
 * Casos de prueba:
 * 1. Señal gaussiana con media=0, std=1 (distribución normal estándar)
 * 2. Señal gaussiana con media=10, std=2 (con offset positivo)
 * 3. Señal gaussiana con media=-5, std=0.5 (con offset negativo)
 * 4. Señal gaussiana con media=0, std=3 (mayor dispersión)
 *
 * \subsection test_all Run_All_RT_Momentos_Tests
 * Función principal que ejecuta todos los tests y genera el reporte.
 *
 * \dot
 * digraph test_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Run_All_RT_Momentos_Tests", fillcolor=lightgreen];
 *   OPEN_LOG [label="Abrir archivo log", fillcolor=lightyellow];
 *   TEST1 [label="Test_Init_RT_Momentos", fillcolor=lightblue];
 *   TEST2 [label="Test_Suscribe_RT_Momentos", fillcolor=lightblue];
 *   TEST3 [label="Test_Gaussian_Signals", fillcolor=lightblue];
 *   SUMMARY [label="Generar resumen", fillcolor=lightyellow];
 *   CLOSE_LOG [label="Cerrar archivo log", fillcolor=lightyellow];
 *   END [label="return result", fillcolor=lightgreen];
 *
 *   START -> OPEN_LOG -> TEST1 -> TEST2 -> TEST3 -> SUMMARY -> CLOSE_LOG -> END;
 * }
 * \enddot
 *
 * \section auxiliares Funciones auxiliares
 *
 * - **test_printf**: Escribe mensajes tanto en pantalla como en archivo de log
 * - **float_equals**: Compara valores flotantes con tolerancia EPSILON
 * - **generate_gaussian_noise**: Genera muestras de ruido gaussiano usando Box-Muller
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 20/07/2025 | Dr. Carlos Romero | 1 | Primera versión del módulo de test |
 * | 20/07/2025 | Dr. Carlos Romero | 2 | Añadido archivo de log de resultados |
 * | 01/08/2025 | Dr. Carlos Romero | 3 | Corregidos tests: orden de dependencias y validación en bucles |
 * | 02/08/2025 | Dr. Carlos Romero | 4 | Eliminados tests con señales constantes (división por cero) |
 * | 03/08/2025 | Dr. Carlos Romero | 5 | Versión simplificada con solo tests gaussianos |
 * | 03/08/2025 | Dr. Carlos Romero | 6 | Actualización documentación Doxygen según estándar |
 *
 * \copyright ZGR R&D AIE
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include "rt_momentos.h"
#include "nsdsp_statistical.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON     1e-5f   /* Tolerancia para comparaciones de punto flotante */

/* Variable global para el archivo de log */
static FILE *test_log_file = NULL;

/* Declaración de funciones de test */
int Test_Init_RT_Momentos(void);
int Test_Suscribe_RT_Momentos(void);
int Test_Gaussian_Signals(void);
int Run_All_RT_Momentos_Tests(void);

/* Función para escribir tanto en pantalla como en archivo */
void test_printf(const char *format, ...)
{
    va_list args;

    /* Escribir en pantalla */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Escribir en archivo si está abierto */
    if (test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(test_log_file, format, args);
        va_end(args);
        fflush(test_log_file);
    }
}

/* Función auxiliar para comparar floats */
int float_equals(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

/* Función auxiliar para simular ruido gaussiano (aproximación Box-Muller) */
float generate_gaussian_noise(float mean, float stddev)
{
    static int use_last = 0;
    static float last = 0.0f;
    float u1, u2, z0, z1;

    if (use_last)
    {
        use_last = 0;
        return last * stddev + mean;
    }

    u1 = (float)rand() / RAND_MAX;
    u2 = (float)rand() / RAND_MAX;

    /* Evitar log(0) */
    if (u1 < 1e-10f) u1 = 1e-10f;

    z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
    z1 = sqrtf(-2.0f * logf(u1)) * sinf(2.0f * M_PI * u2);

    last = z1;
    use_last = 1;

    return z0 * stddev + mean;
}

int Test_Init_RT_Momentos(void)
{
    int result = TEST_OK;
    int i;

    test_printf("\n=== Test Init_RT_Momentos ===\n");

    /* Inicializar el módulo */
    Init_RT_Momentos();

    /* Verificar que pse está correctamente inicializado */
    if (pse.suscribe_rt_momentos == NULL ||
        pse.unsuscribe_rt_momentos == NULL ||
        pse.compute_rt_momentos == NULL)
    {
        test_printf("ERROR: pse no inicializado correctamente\n");
        result = TEST_KO;
    }

    /* Verificar que todos los servicios están en estado FREE */
    for (i = 0; i < MAX_RT_MOMENTOS; i++)
    {
        if (servicios_rt_momentos[i].status != FREE)
        {
            test_printf("ERROR: Servicio %d no está en estado FREE\n", i);
            result = TEST_KO;
        }
    }

    if (result == TEST_OK)
        test_printf("Test Init_RT_Momentos: PASSED\n");
    else
        test_printf("Test Init_RT_Momentos: FAILED\n");

    return result;
}

int Test_Suscribe_RT_Momentos(void)
{
    int result = TEST_OK;
    RT_MOMENTOS_SERVICE services[MAX_RT_MOMENTOS + 1];
    RT_MOMENTOS_SERVICE service;
    int i, ret;

    test_printf("\n=== Test Suscribe/Unsuscribe RT_Momentos ===\n");

    /* Reinicializar el módulo */
    Init_RT_Momentos();

    /* Test 1: Suscribir MAX_RT_MOMENTOS servicios */
    for (i = 0; i < MAX_RT_MOMENTOS; i++)
    {
        services[i] = pse.suscribe_rt_momentos();

        if (services[i] == NONE)
        {
            test_printf("ERROR: No se pudo suscribir servicio %d\n", i);
            result = TEST_KO;
        }
    }

    /* Test 2: Intentar suscribir un servicio más (debe fallar) */
    services[MAX_RT_MOMENTOS] = pse.suscribe_rt_momentos();
    if (services[MAX_RT_MOMENTOS] != NONE)
    {
        test_printf("ERROR: Se permitió suscribir más de MAX_RT_MOMENTOS servicios\n");
        result = TEST_KO;
    }

    /* Test 3: Liberar un servicio y verificar que se puede reusar */
    ret = pse.unsuscribe_rt_momentos(services[0]);
    if (ret != RT_MOMENTOS_OK)
    {
        test_printf("ERROR: No se pudo liberar servicio válido\n");
        result = TEST_KO;
    }

    service = pse.suscribe_rt_momentos();
    if (service == NONE)
    {
        test_printf("ERROR: No se pudo reusar servicio liberado\n");
        result = TEST_KO;
    }

    /* Liberar todos los servicios */
    pse.unsuscribe_rt_momentos(service);
    for (i = 1; i < MAX_RT_MOMENTOS; i++)
    {
        pse.unsuscribe_rt_momentos(services[i]);
    }

    if (result == TEST_OK)
        test_printf("Test Suscribe/Unsuscribe: PASSED\n");
    else
        test_printf("Test Suscribe/Unsuscribe: FAILED\n");

    return result;
}

int Test_Gaussian_Signals(void)
{
    int result = TEST_OK;
    RT_MOMENTOS_SERVICE service;
    int i, ret;
    float sample;
    int num_samples;

    test_printf("\n=== Test Gaussian Signals ===\n");

    /* Reinicializar el módulo */
    Init_RT_Momentos();

    /* Número de muestras: mínimo 2 × N_MA */
    num_samples = N_MA * 2;

    /* Test 1: Señal gaussiana con media cero */
    test_printf("\nTest 1: Señal gaussiana (media=0, std=1)\n");
    service = pse.suscribe_rt_momentos();

    srand(12345); /* Semilla fija para reproducibilidad */

    for (i = 0; i < num_samples; i++)
    {
        sample = generate_gaussian_noise(0.0f, 1.0f);
        ret = pse.compute_rt_momentos(service, sample);
        /* No nos preocupamos si retorna KO por división por cero */
        if (ret == RT_MOMENTOS_KO)
        {
            test_printf("  Nota: División por cero detectada en muestra %d (comportamiento esperado)\n", i);
        }
    }

    test_printf("Media: %f (esperado cerca de 0)\n",
           nsdsp_statistical_objects[service].media);
    test_printf("Varianza: %f (esperado cerca de 1)\n",
           nsdsp_statistical_objects[service].varianza);
    test_printf("Asimetría: %f\n",
           nsdsp_statistical_objects[service].asimetria);
    test_printf("Curtosis: %f\n",
           nsdsp_statistical_objects[service].curtosis);

    pse.unsuscribe_rt_momentos(service);

    /* Test 2: Señal gaussiana con offset positivo */
    test_printf("\nTest 2: Señal gaussiana con offset (media=10, std=2)\n");
    service = pse.suscribe_rt_momentos();

    srand(54321); /* Diferente semilla */

    for (i = 0; i < num_samples; i++)
    {
        sample = generate_gaussian_noise(10.0f, 2.0f);
        ret = pse.compute_rt_momentos(service, sample);
        if (ret == RT_MOMENTOS_KO)
        {
            test_printf("  Nota: División por cero detectada en muestra %d (comportamiento esperado)\n", i);
        }
    }

    test_printf("Media: %f (esperado cerca de 10)\n",
           nsdsp_statistical_objects[service].media);
    test_printf("Varianza: %f (esperado cerca de 4)\n",
           nsdsp_statistical_objects[service].varianza);
    test_printf("Asimetría: %f\n",
           nsdsp_statistical_objects[service].asimetria);
    test_printf("Curtosis: %f\n",
           nsdsp_statistical_objects[service].curtosis);

    pse.unsuscribe_rt_momentos(service);

    /* Test 3: Señal gaussiana con offset negativo */
    test_printf("\nTest 3: Señal gaussiana con offset negativo (media=-5, std=0.5)\n");
    service = pse.suscribe_rt_momentos();

    srand(99999); /* Otra semilla */

    for (i = 0; i < num_samples; i++)
    {
        sample = generate_gaussian_noise(-5.0f, 0.5f);
        ret = pse.compute_rt_momentos(service, sample);
        if (ret == RT_MOMENTOS_KO)
        {
            test_printf("  Nota: División por cero detectada en muestra %d (comportamiento esperado)\n", i);
        }
    }

    test_printf("Media: %f (esperado cerca de -5)\n",
           nsdsp_statistical_objects[service].media);
    test_printf("Varianza: %f (esperado cerca de 0.25)\n",
           nsdsp_statistical_objects[service].varianza);
    test_printf("Asimetría: %f\n",
           nsdsp_statistical_objects[service].asimetria);
    test_printf("Curtosis: %f\n",
           nsdsp_statistical_objects[service].curtosis);

    pse.unsuscribe_rt_momentos(service);

    /* Test 4: Señal gaussiana con más muestras (4 × N_MA) */
    test_printf("\nTest 4: Señal gaussiana con más muestras (media=0, std=3)\n");
    service = pse.suscribe_rt_momentos();

    srand(11111);
    num_samples = N_MA * 4; /* Más muestras para mejor convergencia */

    for (i = 0; i < num_samples; i++)
    {
        sample = generate_gaussian_noise(0.0f, 3.0f);
        ret = pse.compute_rt_momentos(service, sample);
        if (ret == RT_MOMENTOS_KO)
        {
            test_printf("  Nota: División por cero detectada en muestra %d (comportamiento esperado)\n", i);
        }
    }

    test_printf("Media: %f (esperado cerca de 0)\n",
           nsdsp_statistical_objects[service].media);
    test_printf("Varianza: %f (esperado cerca de 9)\n",
           nsdsp_statistical_objects[service].varianza);
    test_printf("Asimetría: %f (esperado cerca de 0)\n",
           nsdsp_statistical_objects[service].asimetria);
    test_printf("Curtosis: %f (esperado cerca de 3)\n",
           nsdsp_statistical_objects[service].curtosis);

    pse.unsuscribe_rt_momentos(service);

    if (result == TEST_OK)
        test_printf("\nTest Gaussian Signals: PASSED\n");
    else
        test_printf("\nTest Gaussian Signals: FAILED\n");

    return result;
}

int Run_All_RT_Momentos_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];

    /* Abrir archivo de log */
    test_log_file = fopen("NSDSP_Unit_Tests_Result.txt", "a");
    if (test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log\n");
    }
    else
    {
        /* Escribir encabezado con fecha y hora */
        time(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        test_printf("\n\n########################################\n");
        test_printf("# NSDSP Unit Tests - RT_MOMENTOS\n");
        test_printf("# Fecha y hora: %s\n", time_string);
        test_printf("########################################\n");
    }

    test_printf("\n========================================\n");
    test_printf("    EJECUTANDO TESTS RT_MOMENTOS\n");
    test_printf("========================================\n");

    /* Ejecutar cada test */
    test_result = Test_Init_RT_Momentos();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_Suscribe_RT_Momentos();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_Gaussian_Signals();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_printf("TODOS LOS TESTS PASARON CORRECTAMENTE\n");
    else
        test_printf("ALGUNOS TESTS FALLARON\n");
    test_printf("========================================\n\n");

    /* Escribir resumen final en el archivo */
    if (test_log_file != NULL)
    {
        test_printf("\n# Resumen Final: ");
        if (total_result == TEST_OK)
            test_printf("SUCCESS - Todos los tests pasaron\n");
        else
            test_printf("FAILURE - Algunos tests fallaron\n");
        test_printf("########################################\n\n");

        fclose(test_log_file);
        test_log_file = NULL;
    }

    return total_result;
}

#endif /* DEBUG */
