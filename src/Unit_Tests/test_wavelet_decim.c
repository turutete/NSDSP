/** \page test_wavelet_decim TEST UNITARIOS WAVELET
 * \brief Módulo de pruebas unitarias para Wavelet_Decim
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de descomposición wavelet. Las pruebas incluyen:
 * - Test de reserva de servicios
 * - Test de inicialización de filtros
 * - Test funcional con archivos CSV de referencia
 *
 * Los tests funcionales utilizan archivos CSV de entrada y comparan las salidas
 * con archivos de referencia para validar el correcto funcionamiento de los
 * filtros Lagrange, Daubechies 4 y Daubechies 8.
 *
 * \section config_test_wavelet Configuración
 *
 * El directorio de archivos de test se configura con TEST_DATA_DIR.
 * Por defecto: "../src/Unit_Test"
 *
 * \section archivos_test_wavelet Archivos utilizados
 *
 * Entrada:
 * - delta_1024.csv: Señal de entrada de 1024 muestras
 *
 * Salidas generadas:
 * - lagrange_out.csv: Salida del filtro Lagrange M=3 L=2
 * - Db4_out.csv: Salida del filtro Daubechies 4 L=2
 * - Db8_out.csv: Salida del filtro Daubechies 8 L=2
 *
 * Referencias para comparación:
 * - lagrange_M2_R3_delta_1024.csv
 * - Db4_M2_delta_1024.csv
 * - Db8_M2_delta_1024.csv
 *
 * \section formato_salida_wavelet Formato de salida
 *
 * Vector de 1024 elementos organizado como:
 * - [0-511]: Coeficientes HP del nivel 1 (512 muestras)
 * - [512-767]: Coeficientes HP del nivel 2 (256 muestras)
 * - [768-1023]: Coeficientes LP del nivel 2 (256 muestras)
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_wavelet Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 14/08/2025 | Dr. Carlos Romero | 1 | Primera versión del módulo de test |
 * | 15/08/2025 | Dr. Carlos Romero | 2 | Añadidos tests con archivos CSV |
 *
 * \copyright ZGR R&D AIE
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include "wavelet_decim.h"

/* Configuración del directorio de test */
#ifndef TEST_DATA_DIR
#define TEST_DATA_DIR "C:/Workspace/C/Windows_App/NSDSP/src/Unit_Tests"
#endif

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON_WAVELET  1e-5f
#define NUM_SAMPLES 1024
#define MAX_LINE_LENGTH 256

/* Variable global para el archivo de log */
static FILE *wavelet_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_Wavelet_Service_Reservation(void);
int Test_Wavelet_Initialization(void);
int Test_Wavelet_Functional(void);
int Test_Wavelet_Lagrange_CSV(void);
int Test_Wavelet_DB4_CSV(void);
int Test_Wavelet_DB8_CSV(void);
int Run_All_Wavelet_Decim_Tests(void);

/* Funciones auxiliares */
void test_wavelet_printf(const char *format, ...);
int float_equals_wavelet(float a, float b, float epsilon);
int read_csv_file(const char *filename, float *data, int max_samples);
int write_csv_file(const char *filename, float *data, int num_samples);
int compare_csv_files(const char *output_file, const char *reference_file, float epsilon);
int process_wavelet_filter(wavelet_filter_type filter_type, int m, int levels,
                           float *input, int input_size, float *output);

/* Definición de funciones */

void test_wavelet_printf(const char *format, ...)
{
    va_list args;

    /* Escribir en pantalla */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Escribir en archivo si está abierto */
    if (wavelet_test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(wavelet_test_log_file, format, args);
        va_end(args);
        fflush(wavelet_test_log_file);
    }
}

int float_equals_wavelet(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

int read_csv_file(const char *filename, float *data, int max_samples)
{
    FILE *file;
    char line[MAX_LINE_LENGTH];
    char full_path[512];
    int count = 0;

    /* Construir ruta completa */
    snprintf(full_path, sizeof(full_path), "%s/%s", TEST_DATA_DIR, filename);

    file = fopen(full_path, "r");
    if (file == NULL)
    {
        test_wavelet_printf("ERROR: No se pudo abrir el archivo %s\n", full_path);
        return -1;
    }

    /* Leer línea por línea */
    while (fgets(line, sizeof(line), file) && count < max_samples)
    {
        /* Convertir string a float */
        data[count] = (float)atof(line);
        count++;
    }

    fclose(file);

    test_wavelet_printf("Leídas %d muestras de %s\n", count, filename);
    return count;
}

int write_csv_file(const char *filename, float *data, int num_samples)
{
    FILE *file;
    char full_path[512];
    int i;

    /* Construir ruta completa */
    snprintf(full_path, sizeof(full_path), "%s/%s", TEST_DATA_DIR, filename);

    file = fopen(full_path, "w");
    if (file == NULL)
    {
        test_wavelet_printf("ERROR: No se pudo crear el archivo %s\n", full_path);
        return TEST_KO;
    }

    /* Escribir cada muestra en una línea */
    for (i = 0; i < num_samples; i++)
    {
        fprintf(file, "%.6f\n", data[i]);
    }

    fclose(file);

    test_wavelet_printf("Escritas %d muestras en %s\n", num_samples, filename);
    return TEST_OK;
}

int compare_csv_files(const char *output_file, const char *reference_file, float epsilon)
{
    float output_data[NUM_SAMPLES];
    float reference_data[NUM_SAMPLES];
    int output_count, reference_count;
    int i;
    int differences = 0;
    float max_error = 0.0f;
    float error;

    /* Leer archivo de salida */
    output_count = read_csv_file(output_file, output_data, NUM_SAMPLES);
    if (output_count < 0)
    {
        return TEST_KO;
    }

    /* Leer archivo de referencia */
    reference_count = read_csv_file(reference_file, reference_data, NUM_SAMPLES);
    if (reference_count < 0)
    {
        return TEST_KO;
    }

    /* Verificar que tienen el mismo número de muestras */
    if (output_count != reference_count)
    {
        test_wavelet_printf("ERROR: Número de muestras diferente: salida=%d, referencia=%d\n",
                           output_count, reference_count);
        return TEST_KO;
    }

    /* Comparar muestra por muestra */
    for (i = 0; i < output_count; i++)
    {
        error = fabs(output_data[i] - reference_data[i]);
        if (error > max_error)
        {
            max_error = error;
        }

        if (!float_equals_wavelet(output_data[i], reference_data[i], epsilon))
        {
            differences++;
            if (differences <= 10)  /* Mostrar solo las primeras 10 diferencias */
            {
                test_wavelet_printf("  Diferencia en muestra %d: salida=%.6f, referencia=%.6f, error=%.6f\n",
                                   i, output_data[i], reference_data[i], error);
            }
        }
    }

    if (differences > 0)
    {
        test_wavelet_printf("Total de diferencias: %d de %d muestras (%.2f%%)\n",
                           differences, output_count,
                           (float)differences * 100.0f / output_count);
        test_wavelet_printf("Error máximo: %.6f\n", max_error);
        return TEST_KO;
    }

    test_wavelet_printf("Comparación exitosa: todas las muestras coinciden (epsilon=%.6f)\n", epsilon);
    return TEST_OK;
}

int process_wavelet_filter(wavelet_filter_type filter_type, int m, int levels,
                           float *input, int input_size, float *output)
{
    WAVELET_HANDLE handle;
    WAVELET_OUTPUT wavelet_out;
    int i;
    int hp1_index = 0;
    int hp2_index = 512;
    int lp2_index = 768;
    int ret;
    int hp1_count = 0;
    int hp2_count = 0;
    int lp2_count = 0;

    /* Inicializar buffer de salida */
    for (i = 0; i < NUM_SAMPLES; i++)
    {
        output[i] = 0.0f;
    }

    /* Suscribir servicio */
    handle = wavelet_api.suscribe_wavelet_decimator(filter_type, m, levels);
    if (handle < 0)
    {
        test_wavelet_printf("ERROR: No se pudo suscribir servicio wavelet\n");
        return TEST_KO;
    }

    /* Procesar todas las muestras de entrada */
    for (i = 0; i < input_size; i++)
    {
        ret = wavelet_api.process_wavelet_decomposition(handle, input[i], &wavelet_out);

        if (ret != WAVELET_DECIM_OK)
        {
            test_wavelet_printf("ERROR: Fallo en process_wavelet_decomposition en muestra %d\n", i);
            wavelet_api.unsuscribe_wavelet_decimator(handle);
            return TEST_KO;
        }

        /* Guardar coeficientes cuando estén listos */

        /* HP Nivel 1: disponible cada 2 muestras */
        if (wavelet_out.detail_ready[0] && hp1_index < 512)
        {
            output[hp1_index++] = wavelet_out.detail[0];
            hp1_count++;
        }

        /* HP Nivel 2 y LP Nivel 2: disponibles cada 4 muestras */
        if (wavelet_out.ready)
        {
            /* HP Nivel 2 */
            if (hp2_index < 768)
            {
                output[hp2_index++] = wavelet_out.detail[1];
                hp2_count++;
            }

            /* LP Nivel 2 (aproximación) */
            if (lp2_index < 1024)
            {
                output[lp2_index++] = wavelet_out.approximation;
                lp2_count++;
            }
        }
    }

    test_wavelet_printf("Coeficientes generados: HP1=%d, HP2=%d, LP2=%d\n",
                       hp1_count, hp2_count, lp2_count);

    /* Liberar servicio */
    wavelet_api.unsuscribe_wavelet_decimator(handle);

    return TEST_OK;
}

int Test_Wavelet_Service_Reservation(void)
{
    int result = TEST_OK;
    WAVELET_HANDLE handles[MAX_WAVELET_SERVICES + 1];
    int i;

    test_wavelet_printf("\n=== Test Reserva de Servicios ===\n");

    /* Inicializar módulo */
    Init_Wavelet_Decim();

    /* Test 1: Reservar todos los servicios disponibles */
    test_wavelet_printf("\nTest 1: Reservar %d servicios\n", MAX_WAVELET_SERVICES);
    for (i = 0; i < MAX_WAVELET_SERVICES; i++)
    {
        handles[i] = wavelet_api.suscribe_wavelet_decimator(WAVELET_LAGRANGE, 1, 1);
        if (handles[i] < 0)
        {
            test_wavelet_printf("ERROR: No se pudo reservar servicio %d\n", i);
            result = TEST_KO;
        }
    }

    /* Test 2: Intentar reservar uno más (debe fallar) */
    test_wavelet_printf("\nTest 2: Intentar reservar servicio adicional (debe fallar)\n");
    handles[MAX_WAVELET_SERVICES] = wavelet_api.suscribe_wavelet_decimator(WAVELET_LAGRANGE, 1, 1);
    if (handles[MAX_WAVELET_SERVICES] >= 0)
    {
        test_wavelet_printf("ERROR: Se permitió reservar más de MAX_WAVELET_SERVICES\n");
        result = TEST_KO;
    }
    else
    {
        test_wavelet_printf("OK: Rechazó correctamente el servicio adicional\n");
    }

    /* Liberar todos los servicios */
    for (i = 0; i < MAX_WAVELET_SERVICES; i++)
    {
        wavelet_api.unsuscribe_wavelet_decimator(handles[i]);
    }

    /* Test 3: Parámetros incorrectos */
    test_wavelet_printf("\nTest 3: Detección de parámetros incorrectos\n");

    /* m inválido para Lagrange */
    handles[0] = wavelet_api.suscribe_wavelet_decimator(WAVELET_LAGRANGE, 0, 1);
    if (handles[0] >= 0)
    {
        test_wavelet_printf("ERROR: Aceptó m=0 para Lagrange\n");
        result = TEST_KO;
        wavelet_api.unsuscribe_wavelet_decimator(handles[0]);
    }
    else
    {
        test_wavelet_printf("OK: Rechazó m=0 para Lagrange\n");
    }

    handles[0] = wavelet_api.suscribe_wavelet_decimator(WAVELET_LAGRANGE, 17, 1);
    if (handles[0] >= 0)
    {
        test_wavelet_printf("ERROR: Aceptó m=17 para Lagrange\n");
        result = TEST_KO;
        wavelet_api.unsuscribe_wavelet_decimator(handles[0]);
    }
    else
    {
        test_wavelet_printf("OK: Rechazó m=17 para Lagrange\n");
    }

    /* Niveles inválidos */
    handles[0] = wavelet_api.suscribe_wavelet_decimator(WAVELET_DB4, 0, 0);
    if (handles[0] >= 0)
    {
        test_wavelet_printf("ERROR: Aceptó 0 niveles\n");
        result = TEST_KO;
        wavelet_api.unsuscribe_wavelet_decimator(handles[0]);
    }
    else
    {
        test_wavelet_printf("OK: Rechazó 0 niveles\n");
    }

    handles[0] = wavelet_api.suscribe_wavelet_decimator(WAVELET_DB4, 0, MAX_DECOMP_LEVELS + 1);
    if (handles[0] >= 0)
    {
        test_wavelet_printf("ERROR: Aceptó más de MAX_DECOMP_LEVELS\n");
        result = TEST_KO;
        wavelet_api.unsuscribe_wavelet_decimator(handles[0]);
    }
    else
    {
        test_wavelet_printf("OK: Rechazó más de MAX_DECOMP_LEVELS\n");
    }

    if (result == TEST_OK)
        test_wavelet_printf("\nTest Reserva de Servicios: PASSED\n");
    else
        test_wavelet_printf("\nTest Reserva de Servicios: FAILED\n");

    return result;
}

int Test_Wavelet_Initialization(void)
{
    int result = TEST_OK;
    WAVELET_HANDLE handle;
    wavelet_filter_type type;
    int levels, num_coeffs;
    int i, j;

    test_wavelet_printf("\n=== Test Inicialización de Filtros ===\n");

    /* Inicializar módulo */
    Init_Wavelet_Decim();

    /* Test 1: Inicialización Lagrange */
    test_wavelet_printf("\nTest 1: Inicialización Lagrange m=3, L=2\n");
    handle = wavelet_api.suscribe_wavelet_decimator(WAVELET_LAGRANGE, 3, 2);
    if (handle < 0)
    {
        test_wavelet_printf("ERROR: No se pudo inicializar Lagrange\n");
        result = TEST_KO;
    }
    else
    {
        wavelet_api.get_wavelet_info(handle, &type, &levels, &num_coeffs);
        test_wavelet_printf("Tipo=%d, Niveles=%d, Coeficientes=%d\n", type, levels, num_coeffs);

        /* Verificar inicialización de buffers */
        for (i = 0; i < levels; i++)
        {
            if (wavelet_services[handle].levels[i].sample_counter != 0)
            {
                test_wavelet_printf("ERROR: sample_counter no inicializado en nivel %d\n", i);
                result = TEST_KO;
            }

            if (wavelet_services[handle].levels[i].lowpass_buffer.write_index != 0 ||
                wavelet_services[handle].levels[i].highpass_buffer.write_index != 0)
            {
                test_wavelet_printf("ERROR: write_index no inicializado en nivel %d\n", i);
                result = TEST_KO;
            }

            /* Verificar que los buffers están en cero */
            for (j = 0; j < num_coeffs; j++)
            {
                if (wavelet_services[handle].levels[i].lowpass_buffer.buffer[j] != 0.0f ||
                    wavelet_services[handle].levels[i].highpass_buffer.buffer[j] != 0.0f)
                {
                    test_wavelet_printf("ERROR: Buffers no inicializados a cero en nivel %d\n", i);
                    result = TEST_KO;
                    break;
                }
            }
        }

        wavelet_api.unsuscribe_wavelet_decimator(handle);
    }

    /* Test 2: Inicialización DB4 */
    test_wavelet_printf("\nTest 2: Inicialización Daubechies 4, L=2\n");
    handle = wavelet_api.suscribe_wavelet_decimator(WAVELET_DB4, 0, 2);
    if (handle < 0)
    {
        test_wavelet_printf("ERROR: No se pudo inicializar DB4\n");
        result = TEST_KO;
    }
    else
    {
        wavelet_api.get_wavelet_info(handle, &type, &levels, &num_coeffs);
        test_wavelet_printf("Tipo=%d, Niveles=%d, Coeficientes=%d\n", type, levels, num_coeffs);

        if (num_coeffs != 4)
        {
            test_wavelet_printf("ERROR: DB4 debe tener 4 coeficientes, tiene %d\n", num_coeffs);
            result = TEST_KO;
        }

        wavelet_api.unsuscribe_wavelet_decimator(handle);
    }

    /* Test 3: Inicialización DB8 */
    test_wavelet_printf("\nTest 3: Inicialización Daubechies 8, L=2\n");
    handle = wavelet_api.suscribe_wavelet_decimator(WAVELET_DB8, 0, 2);
    if (handle < 0)
    {
        test_wavelet_printf("ERROR: No se pudo inicializar DB8\n");
        result = TEST_KO;
    }
    else
    {
        wavelet_api.get_wavelet_info(handle, &type, &levels, &num_coeffs);
        test_wavelet_printf("Tipo=%d, Niveles=%d, Coeficientes=%d\n", type, levels, num_coeffs);

        if (num_coeffs != 8)
        {
            test_wavelet_printf("ERROR: DB8 debe tener 8 coeficientes, tiene %d\n", num_coeffs);
            result = TEST_KO;
        }

        wavelet_api.unsuscribe_wavelet_decimator(handle);
    }

    if (result == TEST_OK)
        test_wavelet_printf("\nTest Inicialización de Filtros: PASSED\n");
    else
        test_wavelet_printf("\nTest Inicialización de Filtros: FAILED\n");

    return result;
}

int Test_Wavelet_Lagrange_CSV(void)
{
    int result = TEST_OK;
    float input_data[NUM_SAMPLES];
    float output_data[NUM_SAMPLES];
    int num_samples;

    test_wavelet_printf("\n=== Test Funcional Lagrange M=3 L=2 ===\n");

    /* Leer archivo de entrada */
    num_samples = read_csv_file("delta_1024.csv", input_data, NUM_SAMPLES);
    if (num_samples != NUM_SAMPLES)
    {
        test_wavelet_printf("ERROR: Se esperaban %d muestras, se leyeron %d\n",
                           NUM_SAMPLES, num_samples);
        return TEST_KO;
    }

    /* Procesar con filtro Lagrange M=3, L=2 */
    test_wavelet_printf("Procesando con Lagrange M=3, L=2...\n");
    result = process_wavelet_filter(WAVELET_LAGRANGE, 3, 2, input_data, NUM_SAMPLES, output_data);
    if (result != TEST_OK)
    {
        test_wavelet_printf("ERROR: Fallo en procesamiento Lagrange\n");
        return TEST_KO;
    }

    /* Escribir archivo de salida */
    result = write_csv_file("lagrange_out.csv", output_data, NUM_SAMPLES);
    if (result != TEST_OK)
    {
        return TEST_KO;
    }

    /* Comparar con referencia */
    test_wavelet_printf("Comparando con archivo de referencia...\n");
    result = compare_csv_files("lagrange_out.csv", "lagrange_M2_R3_delta_1024.csv", EPSILON_WAVELET);

    if (result == TEST_OK)
        test_wavelet_printf("\nTest Funcional Lagrange: PASSED\n");
    else
        test_wavelet_printf("\nTest Funcional Lagrange: FAILED\n");

    return result;
}

int Test_Wavelet_DB4_CSV(void)
{
    int result = TEST_OK;
    float input_data[NUM_SAMPLES];
    float output_data[NUM_SAMPLES];
    int num_samples;

    test_wavelet_printf("\n=== Test Funcional Daubechies 4 L=2 ===\n");

    /* Leer archivo de entrada */
    num_samples = read_csv_file("delta_1024.csv", input_data, NUM_SAMPLES);
    if (num_samples != NUM_SAMPLES)
    {
        test_wavelet_printf("ERROR: Se esperaban %d muestras, se leyeron %d\n",
                           NUM_SAMPLES, num_samples);
        return TEST_KO;
    }

    /* Procesar con filtro DB4, L=2 */
    test_wavelet_printf("Procesando con Daubechies 4, L=2...\n");
    result = process_wavelet_filter(WAVELET_DB4, 0, 2, input_data, NUM_SAMPLES, output_data);
    if (result != TEST_OK)
    {
        test_wavelet_printf("ERROR: Fallo en procesamiento DB4\n");
        return TEST_KO;
    }

    /* Escribir archivo de salida */
    result = write_csv_file("Db4_out.csv", output_data, NUM_SAMPLES);
    if (result != TEST_OK)
    {
        return TEST_KO;
    }

    /* Comparar con referencia */
    test_wavelet_printf("Comparando con archivo de referencia...\n");
    result = compare_csv_files("Db4_out.csv", "Db4_M2_delta_1024.csv", EPSILON_WAVELET);

    if (result == TEST_OK)
        test_wavelet_printf("\nTest Funcional DB4: PASSED\n");
    else
        test_wavelet_printf("\nTest Funcional DB4: FAILED\n");

    return result;
}

int Test_Wavelet_DB8_CSV(void)
{
    int result = TEST_OK;
    float input_data[NUM_SAMPLES];
    float output_data[NUM_SAMPLES];
    int num_samples;

    test_wavelet_printf("\n=== Test Funcional Daubechies 8 L=2 ===\n");

    /* Leer archivo de entrada */
    num_samples = read_csv_file("delta_1024.csv", input_data, NUM_SAMPLES);
    if (num_samples != NUM_SAMPLES)
    {
        test_wavelet_printf("ERROR: Se esperaban %d muestras, se leyeron %d\n",
                           NUM_SAMPLES, num_samples);
        return TEST_KO;
    }

    /* Procesar con filtro DB8, L=2 */
    test_wavelet_printf("Procesando con Daubechies 8, L=2...\n");
    result = process_wavelet_filter(WAVELET_DB8, 0, 2, input_data, NUM_SAMPLES, output_data);
    if (result != TEST_OK)
    {
        test_wavelet_printf("ERROR: Fallo en procesamiento DB8\n");
        return TEST_KO;
    }

    /* Escribir archivo de salida */
    result = write_csv_file("Db8_out.csv", output_data, NUM_SAMPLES);
    if (result != TEST_OK)
    {
        return TEST_KO;
    }

    /* Comparar con referencia */
    test_wavelet_printf("Comparando con archivo de referencia...\n");
    result = compare_csv_files("Db8_out.csv", "Db8_M2_delta_1024.csv", EPSILON_WAVELET);

    if (result == TEST_OK)
        test_wavelet_printf("\nTest Funcional DB8: PASSED\n");
    else
        test_wavelet_printf("\nTest Funcional DB8: FAILED\n");

    return result;
}

int Test_Wavelet_Functional(void)
{
    int result = TEST_OK;
    int test_result;

    test_wavelet_printf("\n=== Tests Funcionales con Archivos CSV ===\n");

    /* Inicializar módulo */
    Init_Wavelet_Decim();

    /* Test Lagrange */
    test_result = Test_Wavelet_Lagrange_CSV();
    if (test_result != TEST_OK) result = TEST_KO;

    /* Test DB4 */
    test_result = Test_Wavelet_DB4_CSV();
    if (test_result != TEST_OK) result = TEST_KO;

    /* Test DB8 */
    test_result = Test_Wavelet_DB8_CSV();
    if (test_result != TEST_OK) result = TEST_KO;

    return result;
}

int Run_All_Wavelet_Decim_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];

    /* Abrir archivo de log */
    wavelet_test_log_file = fopen("Wavelet_Decim_Tests_Result.txt", "a");
    if (wavelet_test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log de Wavelet\n");
    }
    else
    {
        /* Escribir encabezado con fecha y hora */
        time(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        test_wavelet_printf("\n\n########################################\n");
        test_wavelet_printf("# Wavelet Decimation Unit Tests\n");
        test_wavelet_printf("# Fecha y hora: %s\n", time_string);
        test_wavelet_printf("# Directorio de datos: %s\n", TEST_DATA_DIR);
        test_wavelet_printf("########################################\n");
    }

    test_wavelet_printf("\n========================================\n");
    test_wavelet_printf("    EJECUTANDO TESTS WAVELET DECIM\n");
    test_wavelet_printf("========================================\n");

    /* Ejecutar tests */
    test_result = Test_Wavelet_Service_Reservation();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_Wavelet_Initialization();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_Wavelet_Functional();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_wavelet_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_wavelet_printf("TODOS LOS TESTS WAVELET PASARON CORRECTAMENTE\n");
    else
        test_wavelet_printf("ALGUNOS TESTS WAVELET FALLARON\n");
    test_wavelet_printf("========================================\n\n");

    /* Escribir resumen final en el archivo */
    if (wavelet_test_log_file != NULL)
    {
        test_wavelet_printf("\n# Resumen Final: ");
        if (total_result == TEST_OK)
            test_wavelet_printf("SUCCESS - Todos los tests pasaron\n");
        else
            test_wavelet_printf("FAILURE - Algunos tests fallaron\n");
        test_wavelet_printf("########################################\n\n");

        fclose(wavelet_test_log_file);
        wavelet_test_log_file = NULL;
    }

    return total_result;
}

#endif /* DEBUG */
