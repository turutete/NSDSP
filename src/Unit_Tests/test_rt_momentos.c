/** \file test_rt_momentos.c
 * \brief Módulo de pruebas unitarias para RT_Momentos
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo RT_Momentos. Las pruebas incluyen:
 * - Test de inicialización del módulo
 * - Test de suscripción/desuscripción de servicios
 * - Test de cálculo de momentos estadísticos
 * - Test de casos límite y errores
 *
 * Los resultados de las pruebas se muestran por pantalla y se guardan en el archivo
 * NSDSP_Unit_Tests_Result.txt
 *
 * \section uso_test Uso de las pruebas
 * Las pruebas se ejecutan automáticamente desde main() cuando se compila en modo DEBUG.
 * Cada test retorna TEST_OK (0) si pasa correctamente o TEST_KO (-1) si falla.
 *
 * \section funciones_test Funciones de test
 *
 * \subsection test_init Test_Init_RT_Momentos
 * Verifica que la inicialización del módulo se realiza correctamente y que
 * todos los servicios quedan en estado FREE.
 *
 * \subsection test_suscribe Test_Suscribe_RT_Momentos
 * Prueba la asignación de servicios, verificando que:
 * - Se pueden suscribir hasta MAX_RT_MOMENTOS servicios
 * - Los buffers se inicializan correctamente a cero
 * - Se retorna NONE cuando no hay servicios disponibles
 *
 * \subsection test_unsuscribe Test_Unsuscribe_RT_Momentos
 * Verifica la liberación de servicios:
 * - Un servicio asignado se puede liberar correctamente
 * - Un servicio no asignado retorna error
 * - Un servicio liberado puede reasignarse
 *
 * \subsection test_compute Test_Compute_RT_Momentos
 * Prueba el cálculo de momentos con diferentes señales:
 * - Señal constante: verifica media correcta y varianza cero
 * - Señal sinusoidal: verifica cálculo de los 4 momentos
 * - Señal con ruido gaussiano simulado
 *
 * \subsection test_edge Test_Edge_Cases_RT_Momentos
 * Prueba casos límite:
 * - División por cero cuando la varianza es cero
 * - IDs de servicio inválidos
 * - Señales con valores extremos
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 20/07/2025 | Dr. Carlos Romero | 1 | Primera versión del módulo de test |
 * | 20/07/2025 | Dr. Carlos Romero | 2 | Añadido archivo de log de resultados |
 *
 * \copyright ZGR R&D AIE
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <time.h>
#include <stdarg.h>
#include "rt_momentos.h"
#include "nsdsp_statistical.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON     1e-5f   // Tolerancia para comparaciones de punto flotante

// Variable global para el archivo de log
static FILE *test_log_file = NULL;

// Declaración de funciones de test
int Test_Init_RT_Momentos(void);
int Test_Suscribe_RT_Momentos(void);
int Test_Unsuscribe_RT_Momentos(void);
int Test_Compute_RT_Momentos(void);
int Test_Edge_Cases_RT_Momentos(void);
int Run_All_RT_Momentos_Tests(void);

// Función para escribir tanto en pantalla como en archivo
void test_printf(const char *format, ...)
{
    va_list args;
    
    // Escribir en pantalla
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    
    // Escribir en archivo si está abierto
    if (test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(test_log_file, format, args);
        va_end(args);
        fflush(test_log_file);
    }
}

// Función auxiliar para comparar floats
int float_equals(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

// Función auxiliar para generar señal sinusoidal
float generate_sine_sample(int n, float amplitude, float frequency, float phase)
{
    return amplitude * sinf(2.0f * M_PI * frequency * n + phase);
}

// Función auxiliar para simular ruido gaussiano (aproximación)
float generate_gaussian_noise(float mean, float stddev)
{
    static int use_last = 0;
    static float last = 0.0f;
    
    if (use_last)
    {
        use_last = 0;
        return last * stddev + mean;
    }
    
    float u1 = (float)rand() / RAND_MAX;
    float u2 = (float)rand() / RAND_MAX;
    
    float z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * M_PI * u2);
    float z1 = sqrtf(-2.0f * logf(u1)) * sinf(2.0f * M_PI * u2);
    
    last = z1;
    use_last = 1;
    
    return z0 * stddev + mean;
}

int Test_Init_RT_Momentos(void)
{
    int result = TEST_OK;
    int i;
    
    test_printf("\n=== Test Init_RT_Momentos ===\n");
    
    // Inicializar el módulo
    Init_RT_Momentos();
    
    // Verificar que pse está correctamente inicializado
    if (pse.suscribe_rt_momentos == NULL || 
        pse.unsuscribe_rt_momentos == NULL || 
        pse.compute_rt_momentos == NULL)
    {
        test_printf("ERROR: pse no inicializado correctamente\n");
        result = TEST_KO;
    }
    
    // Verificar que todos los servicios están en estado FREE
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
    int i, j;
    
    test_printf("\n=== Test Suscribe_RT_Momentos ===\n");
    
    // Reinicializar el módulo
    Init_RT_Momentos();
    
    // Test 1: Suscribir MAX_RT_MOMENTOS servicios
    for (i = 0; i < MAX_RT_MOMENTOS; i++)
    {
        services[i] = pse.suscribe_rt_momentos();
        
        if (services[i] == NONE)
        {
            test_printf("ERROR: No se pudo suscribir servicio %d\n", i);
            result = TEST_KO;
        }
        else
        {
            // Verificar que el servicio está asignado
            if (servicios_rt_momentos[services[i]].status != ASIGNED)
            {
                test_printf("ERROR: Servicio %d no está marcado como ASIGNED\n", services[i]);
                result = TEST_KO;
            }
            
            // Verificar inicialización de buffers
            for (j = 0; j < N_MA; j++)
            {
                if (!float_equals(servicios_rt_momentos[services[i]].z_buffers.mu_z.buffer_z[j], 0.0f, EPSILON))
                {
                    test_printf("ERROR: Buffer mu_z[%d] no inicializado a cero\n", j);
                    result = TEST_KO;
                    break;
                }
            }
            
            // Verificar inicialización de momentos
            if (!float_equals(servicios_rt_momentos[services[i]].mu, 0.0f, EPSILON) ||
                !float_equals(servicios_rt_momentos[services[i]].var2, 0.0f, EPSILON) ||
                !float_equals(servicios_rt_momentos[services[i]].A, 0.0f, EPSILON) ||
                !float_equals(servicios_rt_momentos[services[i]].C, 0.0f, EPSILON))
            {
                test_printf("ERROR: Momentos no inicializados a cero\n");
                result = TEST_KO;
            }
        }
    }
    
    // Test 2: Intentar suscribir un servicio más (debe fallar)
    services[MAX_RT_MOMENTOS] = pse.suscribe_rt_momentos();
    if (services[MAX_RT_MOMENTOS] != NONE)
    {
        test_printf("ERROR: Se permitió suscribir más de MAX_RT_MOMENTOS servicios\n");
        result = TEST_KO;
    }
    
    // Liberar todos los servicios para siguientes tests
    for (i = 0; i < MAX_RT_MOMENTOS; i++)
    {
        pse.unsuscribe_rt_momentos(services[i]);
    }
    
    if (result == TEST_OK)
        test_printf("Test Suscribe_RT_Momentos: PASSED\n");
    else
        test_printf("Test Suscribe_RT_Momentos: FAILED\n");
    
    return result;
}

int Test_Unsuscribe_RT_Momentos(void)
{
    int result = TEST_OK;
    RT_MOMENTOS_SERVICE service1, service2;
    int ret;
    
    test_printf("\n=== Test Unsuscribe_RT_Momentos ===\n");
    
    // Reinicializar el módulo
    Init_RT_Momentos();
    
    // Test 1: Suscribir y desuscribir un servicio
    service1 = pse.suscribe_rt_momentos();
    ret = pse.unsuscribe_rt_momentos(service1);
    
    if (ret != RT_MOMENTOS_OK)
    {
        test_printf("ERROR: No se pudo desuscribir servicio válido\n");
        result = TEST_KO;
    }
    
    // Verificar que el servicio está libre
    if (servicios_rt_momentos[service1].status != FREE)
    {
        test_printf("ERROR: Servicio no marcado como FREE después de desuscribir\n");
        result = TEST_KO;
    }
    
    // Test 2: Intentar desuscribir un servicio no asignado
    ret = pse.unsuscribe_rt_momentos(service1);
    if (ret != RT_MOMENTOS_KO)
    {
        test_printf("ERROR: Se permitió desuscribir servicio no asignado\n");
        result = TEST_KO;
    }
    
    // Test 3: Intentar desuscribir con ID inválido
    ret = pse.unsuscribe_rt_momentos(-1);
    if (ret != RT_MOMENTOS_KO)
    {
        test_printf("ERROR: Se permitió desuscribir con ID negativo\n");
        result = TEST_KO;
    }
    
    ret = pse.unsuscribe_rt_momentos(MAX_RT_MOMENTOS);
    if (ret != RT_MOMENTOS_KO)
    {
        test_printf("ERROR: Se permitió desuscribir con ID fuera de rango\n");
        result = TEST_KO;
    }
    
    // Test 4: Verificar que se puede reusar un servicio liberado
    service1 = pse.suscribe_rt_momentos();
    service2 = pse.suscribe_rt_momentos();
    pse.unsuscribe_rt_momentos(service1);
    service1 = pse.suscribe_rt_momentos();
    
    if (service1 == NONE)
    {
        test_printf("ERROR: No se pudo reusar servicio liberado\n");
        result = TEST_KO;
    }
    
    // Limpiar
    pse.unsuscribe_rt_momentos(service1);
    pse.unsuscribe_rt_momentos(service2);
    
    if (result == TEST_OK)
        test_printf("Test Unsuscribe_RT_Momentos: PASSED\n");
    else
        test_printf("Test Unsuscribe_RT_Momentos: FAILED\n");
    
    return result;
}

int Test_Compute_RT_Momentos(void)
{
    int result = TEST_OK;
    RT_MOMENTOS_SERVICE service;
    int i, ret;
    float sample;
    
    test_printf("\n=== Test Compute_RT_Momentos ===\n");
    
    // Reinicializar el módulo
    Init_RT_Momentos();
    
    // Test 1: Señal constante
    test_printf("\nTest 1: Señal constante (valor = 5.0)\n");
    service = pse.suscribe_rt_momentos();
    
    for (i = 0; i < N_MA * 2; i++)
    {
        ret = pse.compute_rt_momentos(service, 5.0f);
        if (ret != RT_MOMENTOS_OK && ret != RT_MOMENTOS_KO)
        {
            test_printf("ERROR: Compute retornó valor inesperado\n");
            result = TEST_KO;
        }
    }
    
    // Verificar resultados esperados para señal constante
    if (!float_equals(nsdsp_statistical_objects[service].media, 5.0f, EPSILON))
    {
        test_printf("ERROR: Media incorrecta. Esperado: 5.0, Obtenido: %f\n", 
               nsdsp_statistical_objects[service].media);
        result = TEST_KO;
    }
    
    if (!float_equals(nsdsp_statistical_objects[service].varianza, 0.0f, EPSILON))
    {
        test_printf("ERROR: Varianza incorrecta. Esperado: 0.0, Obtenido: %f\n", 
               nsdsp_statistical_objects[service].varianza);
        result = TEST_KO;
    }
    
    // Asimetría y curtosis deberían ser 0 debido a la división por cero
    if (!float_equals(nsdsp_statistical_objects[service].asimetria, 0.0f, EPSILON))
    {
        test_printf("ERROR: Asimetría incorrecta. Esperado: 0.0, Obtenido: %f\n", 
               nsdsp_statistical_objects[service].asimetria);
        result = TEST_KO;
    }
    
    pse.unsuscribe_rt_momentos(service);
    
    // Test 2: Señal sinusoidal
    test_printf("\nTest 2: Señal sinusoidal\n");
    service = pse.suscribe_rt_momentos();
    
    for (i = 0; i < N_MA * 4; i++)
    {
        sample = generate_sine_sample(i, 1.0f, 0.1f, 0.0f);
        ret = pse.compute_rt_momentos(service, sample);
    }
    
    test_printf("Media: %f (esperado cerca de 0)\n", nsdsp_statistical_objects[service].media);
    test_printf("Varianza: %f (esperado cerca de 0.5)\n", nsdsp_statistical_objects[service].varianza);
    test_printf("Asimetría: %f (esperado cerca de 0)\n", nsdsp_statistical_objects[service].asimetria);
    test_printf("Curtosis: %f\n", nsdsp_statistical_objects[service].curtosis);
    
    // La media de una sinusoidal completa debería ser cercana a 0
    if (!float_equals(nsdsp_statistical_objects[service].media, 0.0f, 0.1f))
    {
        test_printf("WARNING: Media de sinusoidal no cercana a cero\n");
    }
    
    pse.unsuscribe_rt_momentos(service);
    
    // Test 3: Señal con offset y ruido
    test_printf("\nTest 3: Señal con offset (10) y ruido\n");
    service = pse.suscribe_rt_momentos();
    
    srand(12345); // Semilla fija para reproducibilidad
    
    for (i = 0; i < N_MA * 4; i++)
    {
        sample = 10.0f + generate_gaussian_noise(0.0f, 2.0f);
        ret = pse.compute_rt_momentos(service, sample);
    }
    
    test_printf("Media: %f (esperado cerca de 10)\n", nsdsp_statistical_objects[service].media);
    test_printf("Varianza: %f (esperado cerca de 4)\n", nsdsp_statistical_objects[service].varianza);
    test_printf("Asimetría: %f\n", nsdsp_statistical_objects[service].asimetria);
    test_printf("Curtosis: %f\n", nsdsp_statistical_objects[service].curtosis);
    
    // Verificar que la media está cerca del offset
    if (!float_equals(nsdsp_statistical_objects[service].media, 10.0f, 0.5f))
    {
        test_printf("ERROR: Media no cercana al offset esperado\n");
        result = TEST_KO;
    }
    
    pse.unsuscribe_rt_momentos(service);
    
    if (result == TEST_OK)
        test_printf("\nTest Compute_RT_Momentos: PASSED\n");
    else
        test_printf("\nTest Compute_RT_Momentos: FAILED\n");
    
    return result;
}

int Test_Edge_Cases_RT_Momentos(void)
{
    int result = TEST_OK;
    RT_MOMENTOS_SERVICE service;
    int ret, i;
    
    test_printf("\n=== Test Edge Cases RT_Momentos ===\n");
    
    // Reinicializar el módulo
    Init_RT_Momentos();
    
    // Test 1: Compute con servicio no asignado
    ret = pse.compute_rt_momentos(0, 1.0f);
    if (ret != RT_MOMENTOS_KO)
    {
        test_printf("ERROR: Compute permitió servicio no asignado\n");
        result = TEST_KO;
    }
    
    // Test 2: Compute con ID inválido
    ret = pse.compute_rt_momentos(-1, 1.0f);
    if (ret != RT_MOMENTOS_KO)
    {
        test_printf("ERROR: Compute permitió ID negativo\n");
        result = TEST_KO;
    }
    
    ret = pse.compute_rt_momentos(MAX_RT_MOMENTOS, 1.0f);
    if (ret != RT_MOMENTOS_KO)
    {
        test_printf("ERROR: Compute permitió ID fuera de rango\n");
        result = TEST_KO;
    }
    
    // Test 3: Valores extremos
    test_printf("\nTest con valores extremos\n");
    service = pse.suscribe_rt_momentos();
    
    // Valores muy grandes
    for (i = 0; i < 10; i++)
    {
        ret = pse.compute_rt_momentos(service, 1e6f);
    }
    
    test_printf("Media con valores grandes: %f\n", nsdsp_statistical_objects[service].media);
    
    // Valores muy pequeños
    for (i = 0; i < 10; i++)
    {
        ret = pse.compute_rt_momentos(service, 1e-6f);
    }
    
    // Valores alternando positivos y negativos
    for (i = 0; i < N_MA; i++)
    {
        ret = pse.compute_rt_momentos(service, (i % 2 == 0) ? 1.0f : -1.0f);
    }
    
    test_printf("Media con valores alternados: %f (esperado cerca de 0)\n", 
           nsdsp_statistical_objects[service].media);
    
    pse.unsuscribe_rt_momentos(service);
    
    // Test 4: Transición de varianza cero a no-cero
    test_printf("\nTest transición varianza cero a no-cero\n");
    service = pse.suscribe_rt_momentos();
    
    // Primero llenar con valores constantes
    for (i = 0; i < N_MA; i++)
    {
        ret = pse.compute_rt_momentos(service, 1.0f);
    }
    
    // Verificar que retorna KO por división por cero
    if (ret != RT_MOMENTOS_KO)
    {
        test_printf("ERROR: No detectó división por cero con varianza = 0\n");
        result = TEST_KO;
    }
    
    // Ahora introducir variabilidad
    for (i = 0; i < N_MA; i++)
    {
        ret = pse.compute_rt_momentos(service, (float)i);
    }
    
    // Ahora debería retornar OK
    if (ret != RT_MOMENTOS_OK)
    {
        test_printf("ERROR: No recuperó estado OK después de añadir varianza\n");
        result = TEST_KO;
    }
    
    pse.unsuscribe_rt_momentos(service);
    
    if (result == TEST_OK)
        test_printf("\nTest Edge Cases: PASSED\n");
    else
        test_printf("\nTest Edge Cases: FAILED\n");
    
    return result;
}

int Run_All_RT_Momentos_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];
    
    // Abrir archivo de log
    test_log_file = fopen("NSDSP_Unit_Tests_Result.txt", "a");
    if (test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log\n");
    }
    else
    {
        // Escribir encabezado con fecha y hora
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
    
    // Ejecutar cada test
    test_result = Test_Init_RT_Momentos();
    if (test_result != TEST_OK) total_result = TEST_KO;
    
    test_result = Test_Suscribe_RT_Momentos();
    if (test_result != TEST_OK) total_result = TEST_KO;
    
    test_result = Test_Unsuscribe_RT_Momentos();
    if (test_result != TEST_OK) total_result = TEST_KO;
    
    test_result = Test_Compute_RT_Momentos();
    if (test_result != TEST_OK) total_result = TEST_KO;
    
    test_result = Test_Edge_Cases_RT_Momentos();
    if (test_result != TEST_OK) total_result = TEST_KO;
    
    test_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_printf("TODOS LOS TESTS PASARON CORRECTAMENTE\n");
    else
        test_printf("ALGUNOS TESTS FALLARON\n");
    test_printf("========================================\n\n");
    
    // Escribir resumen final en el archivo
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

#endif // DEBUG