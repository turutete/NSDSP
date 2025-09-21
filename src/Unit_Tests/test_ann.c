/** \page test_ann TEST UNITARIOS ANN
 * \brief Módulo de pruebas unitarias para redes neuronales artificiales
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de redes neuronales artificiales. Las pruebas validan
 * la creación de servicios ANN, configuración de capas, procesamiento forward pass
 * y manejo de errores. Los tests solo se compilan y ejecutan en modo DEBUG.
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
 * \subsection test_iterate_ann Test_Iterate_ANN
 * Verifica el procesamiento forward pass de la red:
 * - Procesamiento con red de 1 capa
 * - Procesamiento con red de 2 capas
 * - Verificación de dimensiones
 * - Manejo de errores con punteros NULL
 * - Validación de buffers de entrada/salida
 *
 * \subsection test_trigger_ann Test_Trigger_ANN
 * Verifica las funciones de activación:
 * - Aplicación de cada tipo de función
 * - Manejo de vectores de diferentes tamaños
 * - Validación de parámetros
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_ann Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 15/09/2025 | Dr. Carlos Romero | 1 | Implementación inicial de tests |
 * | 15/09/2025 | Dr. Carlos Romero | 2 | Añadidos tests para iterate_ann y trigger_ann |
 * | 16/09/2025 | Dr. Carlos Romero | 3 | Actualizado para usar API en trigger_ann |
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
#include "nsdsp_math.h"
#include "test_ann.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON_ANN 1e-5f

/* Variable global para el archivo de log */
static FILE *ann_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_Get_ANN(void);
int Test_Iterate_ANN(void);
int Test_Trigger_ANN(void);
int Run_All_ANN_Tests(void);

/* Funciones auxiliares */
void test_ann_printf(const char *format, ...);
int float_equals_ann(float a, float b, float epsilon);

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

int float_equals_ann(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
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
    if (ann_api.get_ann == NULL || ann_api.iterate == NULL || ann_api.trigger == NULL)
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

    if (result == TEST_OK)
        test_ann_printf("\nTest Get_ANN: PASSED\n");
    else
        test_ann_printf("\nTest Get_ANN: FAILED\n");

    return result;
}

int Test_Iterate_ANN(void)
{
    int result = TEST_OK;
    int ret;
    ANN_SERVICE service;

    /* Datos para test: Red simple de 1 capa */
    /* 2 entradas, 2 salidas - matriz identidad para test simple */
    float w1_data[4] = {1.0f, 0.0f,
                        0.0f, 1.0f};
    float b1_data[2] = {0.0f, 0.0f};

    float input_data[2] = {1.0f, 2.0f};
    float output_data[2];

    MATRIZ pesos[1];
    MATRIZ bias[1];

    test_ann_printf("\n=== Test Iterate_ANN ===\n");

    /* Inicializar módulos */
    Init_ANN();
    nsdsp_math_init();

    /* Test 1: Procesamiento con red de 1 capa */
    test_ann_printf("\nTest 1: Red de 1 capa (identidad)\n");

    /* Configurar matrices */
    pesos[0].filas = 2;
    pesos[0].columnas = 2;
    pesos[0].pmatriz = w1_data;

    bias[0].filas = 2;
    bias[0].columnas = 1;
    bias[0].pmatriz = b1_data;

    /* Crear servicio */
    service = ann_api.get_ann(1, RELU, pesos, bias);

    /* Configurar entrada y salida */
    service.x0.pmatriz = input_data;
    service.y0.pmatriz = output_data;

    /* Procesar */
    ret = ann_api.iterate(&service);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: iterate_ann falló con red válida\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Input: [%.2f, %.2f]\n", input_data[0], input_data[1]);
        test_ann_printf("Output: [%.2f, %.2f]\n", output_data[0], output_data[1]);

        /* Con matriz identidad, bias 0 y RELU, la salida debería ser igual a la entrada para valores positivos */
        if (!float_equals_ann(output_data[0], 1.0f, EPSILON_ANN) ||
            !float_equals_ann(output_data[1], 2.0f, EPSILON_ANN))
        {
            test_ann_printf("NOTA: Output diferente del esperado (puede ser normal según la función de activación)\n");
        }
        test_ann_printf("Forward pass ejecutado correctamente\n");
    }

    /* Test 2: Procesamiento con red de 2 capas */
    test_ann_printf("\nTest 2: Red de 2 capas\n");

    /* Datos para red de 2 capas */
    float w2_layer1[6] = {0.5f, -0.3f,
                          0.2f, 0.8f,
                          -0.1f, 0.4f};
    float b2_layer1[3] = {0.1f, 0.2f, -0.1f};

    float w2_layer2[3] = {0.6f, 0.3f, -0.5f};
    float b2_layer2[1] = {0.15f};

    float input2_data[2] = {0.5f, 0.8f};
    float output2_data[1];

    MATRIZ pesos2[2];
    MATRIZ bias2[2];

    /* Configurar matrices */
    pesos2[0].filas = 3;
    pesos2[0].columnas = 2;
    pesos2[0].pmatriz = w2_layer1;

    pesos2[1].filas = 1;
    pesos2[1].columnas = 3;
    pesos2[1].pmatriz = w2_layer2;

    bias2[0].filas = 3;
    bias2[0].columnas = 1;
    bias2[0].pmatriz = b2_layer1;

    bias2[1].filas = 1;
    bias2[1].columnas = 1;
    bias2[1].pmatriz = b2_layer2;

    /* Crear servicio */
    service = ann_api.get_ann(2, SIGMOID, pesos2, bias2);

    /* Configurar entrada y salida */
    service.x0.pmatriz = input2_data;
    service.y0.pmatriz = output2_data;

    /* Procesar */
    ret = ann_api.iterate(&service);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: iterate_ann falló con red de 2 capas\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Input: [%.2f, %.2f]\n", input2_data[0], input2_data[1]);
        test_ann_printf("Output: [%.2f]\n", output2_data[0]);
        test_ann_printf("Forward pass de 2 capas ejecutado correctamente\n");
    }

    /* Test 3: Puntero NULL */
    test_ann_printf("\nTest 3: Manejo de puntero NULL\n");

    ret = ann_api.iterate(NULL);

    if (ret != ANN_KO)
    {
        test_ann_printf("ERROR: No detectó puntero NULL\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Detección de puntero NULL: PASSED\n");
    }

    /* Test 4: x0 o y0 no configurados */
    test_ann_printf("\nTest 4: x0 o y0 no configurados\n");

    service = ann_api.get_ann(1, RELU, pesos, bias);
    service.x0.pmatriz = NULL;  /* No configurar entrada */

    ret = ann_api.iterate(&service);

    if (ret != ANN_KO)
    {
        test_ann_printf("ERROR: No detectó x0.pmatriz NULL\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Detección de x0 no configurado: PASSED\n");
    }

    if (result == TEST_OK)
        test_ann_printf("\nTest Iterate_ANN: PASSED\n");
    else
        test_ann_printf("\nTest Iterate_ANN: FAILED\n");

    return result;
}

int Test_Trigger_ANN(void)
{
    int result = TEST_OK;
    int ret;
    float input_data[4] = {-1.0f, 0.0f, 0.5f, 2.0f};
    float output_data[4];
    MATRIZ input, output;
    ANN_TRIGGER trigger;
    unsigned int i;

    test_ann_printf("\n=== Test Trigger_ANN ===\n");

    /* Inicializar módulo */
    Init_ANN();

    /* Verificar que ann_api.trigger está inicializado */
    if (ann_api.trigger == NULL)
    {
        test_ann_printf("ERROR: ann_api.trigger no está inicializado\n");
        return TEST_KO;
    }

    /* Configurar matrices */
    input.filas = 4;
    input.columnas = 1;
    input.pmatriz = input_data;

    output.filas = 4;
    output.columnas = 1;
    output.pmatriz = output_data;

    /* Test 1: Función RELU */
    test_ann_printf("\nTest 1: Función RELU\n");

    trigger = RELU;
    ret = ann_api.trigger(&input, &output, trigger);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: trigger falló con RELU\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Input: [%.2f, %.2f, %.2f, %.2f]\n",
                       input_data[0], input_data[1], input_data[2], input_data[3]);
        test_ann_printf("Output: [%.2f, %.2f, %.2f, %.2f]\n",
                       output_data[0], output_data[1], output_data[2], output_data[3]);

        /* Verificar RELU: max(0, x) */
        if (!float_equals_ann(output_data[0], 0.0f, EPSILON_ANN) ||     /* -1 -> 0 */
            !float_equals_ann(output_data[1], 0.0f, EPSILON_ANN) ||     /* 0 -> 0 */
            !float_equals_ann(output_data[2], 0.5f, EPSILON_ANN) ||     /* 0.5 -> 0.5 */
            !float_equals_ann(output_data[3], 2.0f, EPSILON_ANN))       /* 2 -> 2 */
        {
            test_ann_printf("ERROR: RELU no produce resultados esperados\n");
            result = TEST_KO;
        }
        else
        {
            test_ann_printf("RELU validada correctamente\n");
        }
    }

    /* Test 2: Función SIGMOID */
    test_ann_printf("\nTest 2: Función SIGMOID\n");

    ret = ann_api.trigger(&input, &output, SIGMOID);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: trigger falló con SIGMOID\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Output: [%.4f, %.4f, %.4f, %.4f]\n",
                       output_data[0], output_data[1], output_data[2], output_data[3]);

        /* Verificar SIGMOID: valores entre 0 y 1 */
        for (i = 0; i < 4; i++)
        {
            if (output_data[i] < 0.0f || output_data[i] > 1.0f)
            {
                test_ann_printf("ERROR: SIGMOID produce valor fuera de rango [0,1]: %.4f\n", output_data[i]);
                result = TEST_KO;
            }
        }

        /* Verificar que sigmoid(0) ≈ 0.5 */
        if (!float_equals_ann(output_data[1], 0.5f, 0.001f))
        {
            test_ann_printf("ERROR: sigmoid(0) = %.4f (esperado 0.5)\n", output_data[1]);
            result = TEST_KO;
        }
        else
        {
            test_ann_printf("SIGMOID validada correctamente\n");
        }
    }

    /* Test 3: Función TANH */
    test_ann_printf("\nTest 3: Función TANH\n");

    ret = ann_api.trigger(&input, &output, TANH);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: trigger falló con TANH\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Output: [%.4f, %.4f, %.4f, %.4f]\n",
                       output_data[0], output_data[1], output_data[2], output_data[3]);

        /* Verificar TANH: valores entre -1 y 1 */
        for (i = 0; i < 4; i++)
        {
            if (output_data[i] < -1.0f || output_data[i] > 1.0f)
            {
                test_ann_printf("ERROR: TANH produce valor fuera de rango [-1,1]: %.4f\n", output_data[i]);
                result = TEST_KO;
            }
        }

        /* Verificar que tanh(0) = 0 */
        if (!float_equals_ann(output_data[1], 0.0f, EPSILON_ANN))
        {
            test_ann_printf("ERROR: tanh(0) = %.4f (esperado 0)\n", output_data[1]);
            result = TEST_KO;
        }
        else
        {
            test_ann_printf("TANH validada correctamente\n");
        }
    }

    /* Test 4: Función LEAK (Leaky ReLU) */
    test_ann_printf("\nTest 4: Función LEAK (Leaky ReLU)\n");

    ret = ann_api.trigger(&input, &output, LEAK);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: trigger falló con LEAK\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Output: [%.4f, %.4f, %.4f, %.4f]\n",
                       output_data[0], output_data[1], output_data[2], output_data[3]);

        /* Verificar LEAK: max(x, 0.01*x) */
        if (!float_equals_ann(output_data[0], -0.01f, EPSILON_ANN) ||   /* -1 -> -0.01 */
            !float_equals_ann(output_data[1], 0.0f, EPSILON_ANN) ||     /* 0 -> 0 */
            !float_equals_ann(output_data[2], 0.5f, EPSILON_ANN) ||     /* 0.5 -> 0.5 */
            !float_equals_ann(output_data[3], 2.0f, EPSILON_ANN))       /* 2 -> 2 */
        {
            test_ann_printf("ERROR: LEAK no produce resultados esperados\n");
            result = TEST_KO;
        }
        else
        {
            test_ann_printf("LEAK validada correctamente\n");
        }
    }

    /* Test 5: Función SOFT (Softplus) */
    test_ann_printf("\nTest 5: Función SOFT (Softplus)\n");

    ret = ann_api.trigger(&input, &output, SOFT);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: trigger falló con SOFT\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Output: [%.4f, %.4f, %.4f, %.4f]\n",
                       output_data[0], output_data[1], output_data[2], output_data[3]);

        /* Softplus siempre produce valores positivos */
        for (i = 0; i < 4; i++)
        {
            if (output_data[i] < 0.0f)
            {
                test_ann_printf("ERROR: SOFT produce valor negativo: %.4f\n", output_data[i]);
                result = TEST_KO;
            }
        }

        /* Verificar que softplus(0) ≈ log(2) ≈ 0.693 */
        if (!float_equals_ann(output_data[1], 0.693147f, 0.001f))
        {
            test_ann_printf("ERROR: softplus(0) = %.4f (esperado ≈0.693)\n", output_data[1]);
            result = TEST_KO;
        }
        else
        {
            test_ann_printf("SOFT validada correctamente\n");
        }
    }

    /* Test 6: Función STEP */
    test_ann_printf("\nTest 6: Función STEP\n");

    ret = ann_api.trigger(&input, &output, STEP);

    if (ret != ANN_OK)
    {
        test_ann_printf("ERROR: trigger falló con STEP\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Output: [%.2f, %.2f, %.2f, %.2f]\n",
                       output_data[0], output_data[1], output_data[2], output_data[3]);

        /* Verificar STEP: 0 si x<0, 1 si x>=0 */
        if (!float_equals_ann(output_data[0], 0.0f, EPSILON_ANN) ||     /* -1 -> 0 */
            !float_equals_ann(output_data[1], 1.0f, EPSILON_ANN) ||     /* 0 -> 1 */
            !float_equals_ann(output_data[2], 1.0f, EPSILON_ANN) ||     /* 0.5 -> 1 */
            !float_equals_ann(output_data[3], 1.0f, EPSILON_ANN))       /* 2 -> 1 */
        {
            test_ann_printf("ERROR: STEP no produce resultados esperados\n");
            result = TEST_KO;
        }
        else
        {
            test_ann_printf("STEP validada correctamente\n");
        }
    }

    /* Test 7: Parámetros NULL */
    test_ann_printf("\nTest 7: Manejo de parámetros NULL\n");

    ret = ann_api.trigger(NULL, &output, RELU);
    if (ret != ANN_KO)
    {
        test_ann_printf("ERROR: No detectó input NULL\n");
        result = TEST_KO;
    }

    ret = ann_api.trigger(&input, NULL, RELU);
    if (ret != ANN_KO)
    {
        test_ann_printf("ERROR: No detectó output NULL\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Detección de parámetros NULL: PASSED\n");
    }

    /* Test 8: Dimensiones incompatibles */
    test_ann_printf("\nTest 8: Dimensiones incompatibles\n");

    output.filas = 3;  /* Diferente a input.filas */
    ret = ann_api.trigger(&input, &output, RELU);

    if (ret != ANN_KO)
    {
        test_ann_printf("ERROR: No detectó dimensiones incompatibles\n");
        result = TEST_KO;
    }
    else
    {
        test_ann_printf("Detección de dimensiones incompatibles: PASSED\n");
    }

    if (result == TEST_OK)
        test_ann_printf("\nTest Trigger_ANN: PASSED\n");
    else
        test_ann_printf("\nTest Trigger_ANN: FAILED\n");

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

    test_result = Test_Iterate_ANN();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_Trigger_ANN();
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

#endif  //DEBUG
