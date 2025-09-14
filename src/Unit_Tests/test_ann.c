/** \page test_ann TEST UNITARIOS ANN
 * \brief Módulo de pruebas unitarias para redes neuronales artificiales
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de redes neuronales artificiales. Las pruebas validan
 * la creación de servicios ANN, configuración de capas y manejo de errores.
 * Los tests solo se compilan y ejecutan en modo DEBUG.
 *
 * \section uso_test_ann Uso del módulo
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
 * Los resultados se muestran en pantalla y se guardan en ANN_Tests_Result.txt
 *
 * \section funciones_test_ann Descripción de funciones
 *
 * \subsection test_get_ann Test_Get_ANN
 * Verifica la correcta creación de servicios ANN con diferentes configuraciones:
 * - Red de 2 capas con función RELU
 * - Red de 1 capa con función SIGMOID
 * - Red con máximo número de capas (LMAX)
 * - Validación de parámetros inválidos
 * - Manejo de punteros NULL
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_ann Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 15/09/2025 | Dr. Carlos Romero | 1 | Implementación inicial de tests |
 *
 * \copyright ZGR R&D AIE
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include "ann.h"
#include "test_ann.h"

#define TEST_OK     0
#define TEST_KO     -1

/* Variable global para el archivo de log */
static FILE *ann_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_Get_ANN(void);
int Run_All_ANN_Tests(void);

/* Funciones auxiliares */
void test_ann_printf(const char *format, ...);

/* Definición de funciones */

void test_ann_printf(const char *format, ...)
{
    va_list args;

    /* Escribir en pantalla */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Escribir en archivo si está abierto */
    if (ann_test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(ann_test_log_file, format, args);
        va_end(args);
        fflush(ann_test_log_file);
    }
}

int Test_Get_ANN(void)
{
    int result = TEST_OK;
    ANN_SERVICE service;
    unsigned int i;

    /* Datos para test 1: Red de 2 capas */
    /* Capa 1: 3 entradas, 4 neuronas */
    float w1_data[12] = {0.1f, 0.2f, 0.3f,
                         0.4f, 0.5f, 0.6f,
                         0.7f, 0.8f, 0.9f,
                         1.0f, 1.1f, 1.2f};
    float b1_data[4] = {0.1f, 0.2f, 0.3f, 0.4f};

    /* Capa 2: 4 entradas, 2 neuronas */
    float w2_data[8] = {0.1f, 0.2f, 0.3f, 0.4f,
                        0.5f, 0.6f, 0.7f, 0.8f};
    float b2_data[2] = {0.1f, 0.2f};

    MATRIZ pesos[2];
    MATRIZ bias[2];

    test_ann_printf("\n=== Test Get_ANN ===\n");

    /* Inicializar módulo */
    Init_ANN();

    /* Verificar que la API esté inicializada */
    if (ann_api.get_ann == NULL)
    {
        test_ann_printf("ERROR: API ANN no inicializada correctamente\n");
        result = TEST_KO;
        return result;
    }

    /* Test 1: Crear red de 2 capas con RELU */
    test_ann_printf("\nTest 1: Red de 2 capas con función RELU\n");

    /* Configurar matrices de pesos y bias */
    pesos[0].filas = 4;
    pesos[0].columnas = 3;
    pesos[0].pmatriz = w1_data;

    pesos[1].filas = 2;
    pesos[1].columnas = 4;
    pesos[1].pmatriz = w2_data;

    bias[0].filas = 4;
    bias[0].columnas = 1;
    bias[0].pmatriz = b1_data;

    bias[1].filas = 2;
    bias[1].columnas = 1;
    bias[1].pmatriz = b2_data;

    /* Crear servicio ANN */
    service = ann_api.get_ann(2, RELU, pesos, bias);

    /* Verificar servicio creado */
    if (service.net.levels != 2)
    {
        test_ann_printf("ERROR: Número de capas incorrecto: %u (esperado 2)\n", service.net.levels);
        result = TEST_KO;
    }

    if (service.trigger != RELU)
    {
        test_ann_printf("ERROR: Trigger incorrecto: %d (esperado RELU)\n", service.trigger);
        result = TEST_KO;
    }

    /* Verificar dimensiones de x0 y y0 */
    if (service.x0.filas != 3 || service.x0.columnas != 1)
    {
        test_ann_printf("ERROR: Dimensiones de x0 incorrectas: %ux%u (esperado 3x1)\n",
                       service.x0.filas, service.x0.columnas);
        result = TEST_KO;
    }

    if (service.y0.filas != 2 || service.y0.columnas != 1)
    {
        test_ann_printf("ERROR: Dimensiones de y0 incorrectas: %ux%u (esperado 2x1)\n",
                       service.y0.filas, service.y0.columnas);
        result = TEST_KO;
    }

    /* Verificar que las capas apuntan correctamente */
    for (i = 0; i < 2; i++)
    {
        if (service.net.layers[i] == NULL)
        {
            test_ann_printf("ERROR: Capa %u es NULL\n", i);
            result = TEST_KO;
        }
        else
        {
            if (service.net.layers[i]->pesos != &pesos[i])
            {
                test_ann_printf("ERROR: Pesos de capa %u no apuntan correctamente\n", i);
                result = TEST_KO;
            }
            if (service.net.layers[i]->bias != &bias[i])
            {
                test_ann_printf("ERROR: Bias de capa %u no apunta correctamente\n", i);
                result = TEST_KO;
            }
        }
    }

    if (result == TEST_OK)
    {
        test_ann_printf("Red de 2 capas creada correctamente\n");
    }

    /* Test 2: Número de capas excede LMAX */
    test_ann_printf("\nTest 2: Número de capas excede LMAX\n");

    service = ann_api.get_ann(LMAX + 1, SIGMOID, pesos, bias);

    if (service.net.levels != 0)
    {
        test_ann_printf("ERROR: No detectó levels > LMAX\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Detección de levels > LMAX: PASSED\n");
    }

    /* Test 3: Punteros NULL */
    test_ann_printf("\nTest 3: Manejo de punteros NULL\n");

    service = ann_api.get_ann(2, TANH, NULL, bias);
    if (service.net.levels != 0)
    {
        test_ann_printf("ERROR: No detectó puntero NULL en pesos\n");
        result = TEST_KO;
    }

    service = ann_api.get_ann(2, TANH, pesos, NULL);
    if (service.net.levels != 0)
    {
        test_ann_printf("ERROR: No detectó puntero NULL en bias\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Manejo de punteros NULL: PASSED\n");
    }

    /* Test 4: Cero capas */
    test_ann_printf("\nTest 4: Cero capas\n");

    service = ann_api.get_ann(0, STEP, pesos, bias);

    if (service.net.levels != 0)
    {
        test_ann_printf("ERROR: No detectó levels = 0\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Detección de levels = 0: PASSED\n");
    }

    /* Test 5: Red con LMAX capas */
    test_ann_printf("\nTest 5: Red con LMAX capas\n");

    /* Crear datos para LMAX capas */
    float w_max[LMAX][4];
    float b_max[LMAX];
    MATRIZ pesos_max[LMAX];
    MATRIZ bias_max[LMAX];

    for (i = 0; i < LMAX; i++)
    {
        w_max[i][0] = (float)i;
        w_max[i][1] = (float)i + 0.1f;
        w_max[i][2] = (float)i + 0.2f;
        w_max[i][3] = (float)i + 0.3f;
        b_max[i] = (float)i * 0.1f;

        pesos_max[i].filas = 2;
        pesos_max[i].columnas = 2;
        pesos_max[i].pmatriz = w_max[i];

        bias_max[i].filas = 2;
        bias_max[i].columnas = 1;
        bias_max[i].pmatriz = &b_max[i];
    }

    service = ann_api.get_ann(LMAX, SOFT, pesos_max, bias_max);

    if (service.net.levels != LMAX)
    {
        test_ann_printf("ERROR: No pudo crear red con LMAX capas\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Red con LMAX capas creada correctamente\n");
    }

    if (result == TEST_OK)
        test_ann_printf("\nTest Get_ANN: PASSED\n");
    else
        test_ann_printf("\nTest Get_ANN: FAILED\n");

    return result;
}

int Run_All_ANN_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];

    /* Abrir archivo de log */
    ann_test_log_file = fopen("ANN_Tests_Result.txt", "a");
    if (ann_test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log de ANN\n");
    }
    else
    {
        /* Escribir encabezado con fecha y hora */
        time(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        test_ann_printf("\n\n########################################\n");
        test_ann_printf("# ANN (Artificial Neural Networks) Unit Tests\n");
        test_ann_printf("# Fecha y hora: %s\n", time_string);
        test_ann_printf("########################################\n");
    }

    test_ann_printf("\n========================================\n");
    test_ann_printf("    EJECUTANDO TESTS ANN\n");
    test_ann_printf("========================================\n");

    /* Ejecutar tests */
    test_result = Test_Get_ANN();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_ann_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_ann_printf("TODOS LOS TESTS ANN PASARON CORRECTAMENTE\n");
    else
        test_ann_printf("ALGUNOS TESTS ANN FALLARON\n");
    test_ann_printf("========================================\n\n");

    /* Escribir resumen final en el archivo */
    if (ann_test_log_file != NULL)
    {
        test_ann_printf("\n# Resumen Final: ");
        if (total_result == TEST_OK)
            test_ann_printf("SUCCESS - Todos los tests pasaron\n");
        else
            test_ann_printf("FAILURE - Algunos tests fallaron\n");
        test_ann_printf("########################################\n\n");

        fclose(ann_test_log_file);
        ann_test_log_file = NULL;
    }

    return total_result;
}

#endif /* DEBUG */
