/** \page test_dwt TEST UNITARIOS DWT
 * \brief Módulo de pruebas unitarias para DWT (Transformada Wavelet Discreta)
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de transformada wavelet discreta. Las pruebas validan
 * la inicialización de objetos DWT y comparan los resultados de la descomposición
 * wavelet con archivos de referencia pregenerados. Los tests solo se compilan y
 * ejecutan en modo DEBUG.
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
 * Se generan archivos CSV con las salidas calculadas para comparación.
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
 *   CREATE [label="Crear objeto DWT", fillcolor=lightblue];
 *   VERIFY_COEF [label="Verificar\ncoeficientes", shape=diamond, fillcolor=lightcyan];
 *   VERIFY_BUF [label="Verificar\nbuffers Z", shape=diamond, fillcolor=lightcyan];
 *   VERIFY_OUT [label="Verificar\nsalidas", shape=diamond, fillcolor=lightcyan];
 *   PASS [label="PASSED", fillcolor=lightgreen];
 *   FAIL [label="FAILED", fillcolor=lightcoral];
 *
 *   START -> INIT -> CREATE -> VERIFY_COEF;
 *   VERIFY_COEF -> VERIFY_BUF [label="OK"];
 *   VERIFY_COEF -> FAIL [label="Error"];
 *   VERIFY_BUF -> VERIFY_OUT [label="OK"];
 *   VERIFY_BUF -> FAIL [label="Error"];
 *   VERIFY_OUT -> PASS [label="OK"];
 *   VERIFY_OUT -> FAIL [label="Error"];
 * }
 * \enddot
 *
 * \subsection test_dwt_functional Test_DWT_Functional
 * Prueba la descomposición wavelet comparando con archivos de referencia:
 * - Lee archivo CSV de referencia según el tipo de filtro (Lagrange/DB4/DB8)
 * - Procesa señal impulso de 1024 muestras
 * - Compara salidas D2, D1 y A0 con referencia
 * - Genera archivo CSV con resultados calculados
 *
 * Formato de archivos CSV (1024 muestras total, M=2 niveles):
 * - Muestras 0-511: Secuencia D2 (detalle nivel 2)
 * - Muestras 512-767: Secuencia D1 (detalle nivel 1)
 * - Muestras 768-1023: Secuencia A0 (aproximación)
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_dwt Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 01/09/2025 | Dr. Carlos Romero | 1 | Implementación inicial con test de inicialización |
 * | 02/09/2025 | Dr. Carlos Romero | 2 | Añadido test funcional con comparación CSV |
 * | 02/09/2025 | Dr. Carlos Romero | 3 | Corrección formato CSV para M=2 niveles |
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
#include "dwt.h"
#include "fir_filter.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON_DWT  1e-5f
#define IMPULSE_LENGTH 1024
#define D2_LENGTH    512
#define D1_LENGTH    256
#define A0_LENGTH    256

/* Variable global para el archivo de log */
static FILE *dwt_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_DWT_Initialization(void);
int Test_DWT_Functional(void);
int Run_All_DWT_Tests(void);

/* Funciones auxiliares */
void test_dwt_printf(const char *format, ...);
int float_equals_dwt(float a, float b, float epsilon);
int read_reference_csv(const char *filename, float *buffer, int size);
int write_result_csv(const char *filename, float *d2, float *d1, float *a0);

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

int read_reference_csv(const char *filename, float *buffer, int size)
{
    FILE *file;
    int i;
    float value;

    file = fopen(filename, "r");
    if (file == NULL)
    {
        test_dwt_printf("ERROR: No se pudo abrir el archivo de referencia %s\n", filename);
        return TEST_KO;
    }

    for (i = 0; i < size; i++)
    {
        if (fscanf(file, "%f", &value) != 1)
        {
            test_dwt_printf("ERROR: No se pudo leer muestra %d del archivo %s\n", i, filename);
            fclose(file);
            return TEST_KO;
        }
        buffer[i] = value;
    }

    fclose(file);
    return TEST_OK;
}

int write_result_csv(const char *filename, float *d2, float *d1, float *a0)
{
    FILE *file;
    int i;

    file = fopen(filename, "w");
    if (file == NULL)
    {
        test_dwt_printf("ERROR: No se pudo crear el archivo de resultados %s\n", filename);
        return TEST_KO;
    }

    /* Escribir D2 (512 muestras) */
    for (i = 0; i < D2_LENGTH; i++)
    {
        fprintf(file, "%.6f\n", d2[i]);
    }

    /* Escribir D1 (256 muestras) */
    for (i = 0; i < D1_LENGTH; i++)
    {
        fprintf(file, "%.6f\n", d1[i]);
    }

    /* Escribir A0 (256 muestras) */
    for (i = 0; i < A0_LENGTH; i++)
    {
        fprintf(file, "%.6f\n", a0[i]);
    }

    fclose(file);
    return TEST_OK;
}

int Test_DWT_Initialization(void)
{
    int result = TEST_OK;
    DWT_OBJECT dwt_obj;
    int i, j;
    int all_zero_lp;
    int all_zero_hp;

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

    /* Verificar que no sean todos cero */
    all_zero_lp = 1;
    all_zero_hp = 1;

    for (i = 0; i < BUFFER_SIZE; i++)
    {
        if (dwt_obj.lp_coef[i] != 0.0f) all_zero_lp = 0;
        if (dwt_obj.hp_coef[i] != 0.0f) all_zero_hp = 0;
    }

    if (all_zero_lp || all_zero_hp)
    {
        test_dwt_printf("ERROR: Coeficientes no inicializados correctamente\n");
        result = TEST_KO;
    }
    else
    {
        test_dwt_printf("Coeficientes LP y HP inicializados correctamente\n");
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

int Test_DWT_Functional(void)
{
    int result = TEST_OK;
    DWT_OBJECT dwt_obj;
    float reference_data[IMPULSE_LENGTH];
    float *ref_d2, *ref_d1, *ref_a0;
    float calc_d2[D2_LENGTH];
    float calc_d1[D1_LENGTH];
    float calc_a0[A0_LENGTH];
    int d2_idx = 0, d1_idx = 0, a0_idx = 0;
    const char *reference_file;
    const char *result_file;
    int i;
    float error;
    int decim2,decim1;
    int errors_d2 = 0, errors_d1 = 0, errors_a0 = 0;
    float max_error_d2 = 0.0f, max_error_d1 = 0.0f, max_error_a0 = 0.0f;

    test_dwt_printf("\n=== Test DWT Functional ===\n");
    test_dwt_printf("Configuración: M=2 niveles de descomposición\n");

    /* Determinar archivos según el tipo de filtro */
#ifdef LAGRANGE
    reference_file = "C:\\Workspace\\C\\Windows_App\\NSDSP\\src\\Unit_Tests\\lagrange_M2_R3_delta_1024.csv";
    result_file = "C:\\Workspace\\C\\Windows_App\\NSDSP\\src\\Unit_Tests\\lagrange_result.csv";
    test_dwt_printf("Usando filtro LAGRANGE con M=%d\n", LAGRANGE_M);
#elif defined(DB4)
    reference_file = "C:\\Workspace\\C\\Windows_App\\NSDSP\\src\\Unit_Tests\\Db4_M2_delta_1024.csv";
    result_file = "C:\\Workspace\\C\\Windows_App\\NSDSP\\src\\Unit_Tests\\db4_result.csv";
    test_dwt_printf("Usando filtro Daubechies 4\n");
#elif defined(DB8)
    reference_file = "C:\\Workspace\\C\\Windows_App\\NSDSP\\src\\Unit_Tests\\Db8_M2_delta_1024.csv";
    result_file = "C:\\Workspace\\C\\Windows_App\\NSDSP\\src\\Unit_Tests\\db8_result.csv";
    test_dwt_printf("Usando filtro Daubechies 8\n");
#else
    test_dwt_printf("ERROR: No se ha definido ningún tipo de filtro\n");
    return TEST_KO;
#endif

    /* Leer archivo de referencia */
    test_dwt_printf("\nLeyendo archivo de referencia: %s\n", reference_file);
    if (read_reference_csv(reference_file, reference_data, IMPULSE_LENGTH) != TEST_OK)
    {
        test_dwt_printf("ERROR: No se pudo leer el archivo de referencia\n");
        return TEST_KO;
    }

    /* Establecer punteros a las secciones del archivo de referencia */
    ref_d2 = &reference_data[0];           /* Muestras 0-511: D2 */
    ref_d1 = &reference_data[512];         /* Muestras 512-767: D1 */
    ref_a0 = &reference_data[768];         /* Muestras 768-1023: A0 */

    test_dwt_printf("Formato del archivo de referencia:\n");
    test_dwt_printf("  D2: muestras 0-511 (512 muestras)\n");
    test_dwt_printf("  D1: muestras 512-767 (256 muestras)\n");
    test_dwt_printf("  A0: muestras 768-1023 (256 muestras)\n");

    /* Inicializar arrays de salida */
    for (i = 0; i < D2_LENGTH; i++) calc_d2[i] = 0.0f;
    for (i = 0; i < D1_LENGTH; i++) calc_d1[i] = 0.0f;
    for (i = 0; i < A0_LENGTH; i++) calc_a0[i] = 0.0f;

    /* Inicializar objeto DWT */
    Init_Fir();
    Init_DWT();
    dwt_api.get_dwt(&dwt_obj);

    test_dwt_printf("\nProcesando señal impulso de %d muestras...\n", IMPULSE_LENGTH);

    /* Inicializa diezmadores */
    decim2=0;
    decim1=0;

    /* Procesar señal impulso */
    for (i = 0; i < IMPULSE_LENGTH; i++)
    {
        float input_sample;

        /* Señal impulso: 1.0 en la primera muestra, 0.0 en el resto */
        if (i == 0)
            input_sample = 1.0f;
        else
            input_sample = 0.0f;

        /* Procesar con DWT */
        dwt_api.dwt(input_sample, &dwt_obj);

        /* Para WAVELET_LEVELS = 2:
         * - yout[0] = D2 (detalle nivel 1) - sale cada 2^1 = 2 muestras (decimación por 2)
         * - yout[1] = D1 (detalle nivel 2) - sale cada 2^2 = 4 muestras (decimación por 4)
         * - yout[2] = A0 (aproximación)    - sale cada 2^2 = 4 muestras (decimación por 4)
         */

        /* Guardar salidas cuando están disponibles según el patrón de decimación */

        /* D2 sale cada 2 muestras (después de decimación por 2) */
        if (decim2 == 0 && d2_idx < D2_LENGTH)
        {
            calc_d2[d2_idx++] = dwt_obj.yout[0];

        }

        /* D2 sale cada 4 muestras (después de decimación por 4) */
        if ( decim1== 0 && d1_idx < D1_LENGTH)
        {
            calc_d1[d1_idx++] = dwt_obj.yout[1];
        }

        /* A0 sale cada 4 muestras (después de decimación por 4) */
        if ( decim1== 0 && a0_idx < A0_LENGTH)
        {
            calc_a0[a0_idx++] = dwt_obj.yout[2];
        }

        decim2--;
            if (decim2<0)
            {
                decim2=1;
            }
        decim1--;
            if (decim1<0)
            {
                decim1=3;
            }
    }

    test_dwt_printf("\nMuestras recolectadas:\n");
    test_dwt_printf("  D2: %d muestras (esperadas: %d)\n", d2_idx, D2_LENGTH);
    test_dwt_printf("  D1: %d muestras (esperadas: %d)\n", d1_idx, D1_LENGTH);
    test_dwt_printf("  A0: %d muestras (esperadas: %d)\n", a0_idx, A0_LENGTH);

    /* Comparar resultados con referencia */
    test_dwt_printf("\nComparando resultados con referencia...\n");

    /* Comparar D2 */
    test_dwt_printf("\n--- Comparación D2 (Detalle Nivel 2) ---\n");
    for (i = 0; i < D2_LENGTH && i < d2_idx; i++)
    {
        error = fabs(calc_d2[i] - ref_d2[i]);
        if (error > EPSILON_DWT)
        {
            errors_d2++;
            if (error > max_error_d2) max_error_d2 = error;
            if (errors_d2 <= 5)  /* Mostrar solo los primeros 5 errores */
            {
                test_dwt_printf("  Error D2[%d]: calc=%.6f, ref=%.6f, diff=%.6f\n",
                               i, calc_d2[i], ref_d2[i], error);
            }
        }
    }

    /* Comparar D1 */
    test_dwt_printf("\n--- Comparación D1 (Detalle Nivel 1) ---\n");
    for (i = 0; i < D1_LENGTH && i < d1_idx; i++)
    {
        error = fabs(calc_d1[i] - ref_d1[i]);
        if (error > EPSILON_DWT)
        {
            errors_d1++;
            if (error > max_error_d1) max_error_d1 = error;
            if (errors_d1 <= 5)
            {
                test_dwt_printf("  Error D1[%d]: calc=%.6f, ref=%.6f, diff=%.6f\n",
                               i, calc_d1[i], ref_d1[i], error);
            }
        }
    }

    /* Comparar A0 */
    test_dwt_printf("\n--- Comparación A0 (Aproximación) ---\n");
    for (i = 0; i < A0_LENGTH && i < a0_idx; i++)
    {
        error = fabs(calc_a0[i] - ref_a0[i]);
        if (error > EPSILON_DWT)
        {
            errors_a0++;
            if (error > max_error_a0) max_error_a0 = error;
            if (errors_a0 <= 5)
            {
                test_dwt_printf("  Error A0[%d]: calc=%.6f, ref=%.6f, diff=%.6f\n",
                               i, calc_a0[i], ref_a0[i], error);
            }
        }
    }

    /* Resumen de errores */
    test_dwt_printf("\n========== Resumen de Comparación ==========\n");
    test_dwt_printf("D2 (Detalle Nivel 2):\n");
    test_dwt_printf("  Muestras comparadas: %d\n", d2_idx);
    test_dwt_printf("  Errores encontrados: %d\n", errors_d2);
    test_dwt_printf("  Error máximo: %.6f\n", max_error_d2);
    test_dwt_printf("  Resultado: %s\n", errors_d2 == 0 ? "EXACTO" : "DIFERENCIAS ENCONTRADAS");

    test_dwt_printf("\nD1 (Detalle Nivel 1):\n");
    test_dwt_printf("  Muestras comparadas: %d\n", d1_idx);
    test_dwt_printf("  Errores encontrados: %d\n", errors_d1);
    test_dwt_printf("  Error máximo: %.6f\n", max_error_d1);
    test_dwt_printf("  Resultado: %s\n", errors_d1 == 0 ? "EXACTO" : "DIFERENCIAS ENCONTRADAS");

    test_dwt_printf("\nA0 (Aproximación):\n");
    test_dwt_printf("  Muestras comparadas: %d\n", a0_idx);
    test_dwt_printf("  Errores encontrados: %d\n", errors_a0);
    test_dwt_printf("  Error máximo: %.6f\n", max_error_a0);
    test_dwt_printf("  Resultado: %s\n", errors_a0 == 0 ? "EXACTO" : "DIFERENCIAS ENCONTRADAS");
    test_dwt_printf("=============================================\n");

    /* Guardar resultados en archivo CSV */
    test_dwt_printf("\nGuardando resultados en %s...\n", result_file);
    if (write_result_csv(result_file, calc_d2, calc_d1, calc_a0) != TEST_OK)
    {
        test_dwt_printf("ERROR: No se pudo guardar el archivo de resultados\n");
        result = TEST_KO;
    }
    else
    {
        test_dwt_printf("Archivo de resultados guardado correctamente\n");
    }

    /* Determinar si el test pasó */
    if (errors_d1 > 0 || errors_d2 > 0 || errors_a0 > 0)
    {
        test_dwt_printf("\nADVERTENCIA: Se encontraron diferencias con el archivo de referencia\n");
        test_dwt_printf("Posibles causas:\n");
        test_dwt_printf("- Diferencias en la implementación del algoritmo\n");
        test_dwt_printf("- Diferencias de precisión numérica\n");
        test_dwt_printf("- Diferencias en las condiciones iniciales\n");
        /* Marcar como fallo si hay demasiados errores */
        if (errors_d1 > 10 || errors_d2 > 10 || errors_a0 > 10)
        {
            result = TEST_KO;
        }
    }

    if (result == TEST_OK)
        test_dwt_printf("\nTest DWT Functional: PASSED\n");
    else
        test_dwt_printf("\nTest DWT Functional: FAILED\n");

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

    test_result = Test_DWT_Functional();
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
