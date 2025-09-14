/** \page test_nsdsp_math TEST UNITARIOS NSDSP MATH
 * \brief Módulo de pruebas unitarias para operaciones matemáticas
 *
 * Este módulo contiene las funciones de test unitario para verificar el correcto
 * funcionamiento del módulo de operaciones matemáticas. Las pruebas validan
 * la multiplicación de matrices con diferentes configuraciones y casos límite,
 * así como las operaciones de suma y resta de matrices. Los tests solo se
 * compilan y ejecutan en modo DEBUG.
 *
 * \section uso_test_math Uso del módulo
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
 * Los resultados se muestran en pantalla y se guardan en NSDSP_Math_Tests_Result.txt
 *
 * \section funciones_test_math Descripción de funciones
 *
 * \subsection test_matriz_producto Test_Matriz_Producto
 * Verifica la correcta multiplicación de matrices con diferentes casos:
 * - Matrices 2×3 × 3×2 = 2×2
 * - Matrices 3×3 × 3×3 = 3×3 (matrices cuadradas)
 * - Matrices 1×4 × 4×1 = 1×1 (vectores)
 * - Matriz identidad
 * - Dimensiones incompatibles
 * - Punteros NULL
 *
 * \dot
 * digraph test_producto_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Test_Matriz_Producto", fillcolor=lightgreen];
 *   INIT [label="nsdsp_math_init()", fillcolor=lightyellow];
 *   TEST1 [label="Test 2×3 × 3×2", fillcolor=lightblue];
 *   TEST2 [label="Test matrices cuadradas", fillcolor=lightblue];
 *   TEST3 [label="Test vectores", fillcolor=lightblue];
 *   TEST4 [label="Test matriz identidad", fillcolor=lightblue];
 *   TEST5 [label="Test dimensiones incompatibles", fillcolor=lightblue];
 *   TEST6 [label="Test punteros NULL", fillcolor=lightblue];
 *   RESULT [label="Retornar resultado", fillcolor=lightgreen];
 *
 *   START -> INIT -> TEST1 -> TEST2 -> TEST3 -> TEST4 -> TEST5 -> TEST6 -> RESULT;
 * }
 * \enddot
 *
 * \subsection test_matriz_suma Test_Matriz_Suma
 * Verifica las operaciones de suma y resta de matrices:
 * - Suma de matrices 3×3
 * - Resta de matrices 3×3
 * - Matrices de diferentes dimensiones
 * - Punteros NULL
 * - Verificación con signo = 0 (debe sumar)
 *
 * \dot
 * digraph test_suma_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Test_Matriz_Suma", fillcolor=lightgreen];
 *   INIT [label="nsdsp_math_init()", fillcolor=lightyellow];
 *   TEST1 [label="Test suma 3×3", fillcolor=lightblue];
 *   TEST2 [label="Test resta 3×3", fillcolor=lightblue];
 *   TEST3 [label="Test dimensiones incompatibles", fillcolor=lightblue];
 *   TEST4 [label="Test punteros NULL", fillcolor=lightblue];
 *   TEST5 [label="Test signo = 0", fillcolor=lightblue];
 *   RESULT [label="Retornar resultado", fillcolor=lightgreen];
 *
 *   START -> INIT -> TEST1 -> TEST2 -> TEST3 -> TEST4 -> TEST5 -> RESULT;
 * }
 * \enddot
 *
 * \subsection run_all_math_tests Run_All_NSDSP_Math_Tests
 * Función principal que ejecuta todos los tests y genera el reporte.
 * - Abre archivo de log con timestamp
 * - Ejecuta Test_Matriz_Producto
 * - Ejecuta Test_Matriz_Suma
 * - Genera resumen de resultados
 * - Cierra archivo de log
 *
 * \section funciones_auxiliares_math Funciones auxiliares
 * - **test_math_printf**: Escribe mensajes tanto en pantalla como en archivo de log
 * - **float_equals_math**: Compara valores flotantes con tolerancia EPSILON_MATH
 * - **print_matriz**: Imprime una matriz formateada para visualización
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_test_math Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 10/09/2025 | Dr. Carlos Romero | 1 | Implementación inicial de tests |
 * | 13/09/2025 | Dr. Carlos Romero | 2 | Añadidos tests para suma/resta de matrices |
 *
 * \copyright ZGR R&D AIE
 */

#ifdef DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdarg.h>
#include "nsdsp_math.h"
#include "test_nsdsp_math.h"

#define TEST_OK     0
#define TEST_KO     -1
#define EPSILON_MATH  1e-6f

/* Variable global para el archivo de log */
static FILE *math_test_log_file = NULL;

/* Declaración de funciones de test */
int Test_Matriz_Producto(void);
int Test_Matriz_Suma(void);
int Run_All_NSDSP_Math_Tests(void);

/* Funciones auxiliares */
void test_math_printf(const char *format, ...);
int float_equals_math(float a, float b, float epsilon);
void print_matriz(const char *nombre, MATRIZ *m);

/* Definición de funciones */

void test_math_printf(const char *format, ...)
{
    va_list args;

    /* Escribir en pantalla */
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    /* Escribir en archivo si está abierto */
    if (math_test_log_file != NULL)
    {
        va_start(args, format);
        vfprintf(math_test_log_file, format, args);
        va_end(args);
        fflush(math_test_log_file);
    }
}

int float_equals_math(float a, float b, float epsilon)
{
    return fabs(a - b) < epsilon;
}

void print_matriz(const char *nombre, MATRIZ *m)
{
    unsigned int i, j;
    float *p;

    if (m == NULL || m->pmatriz == NULL)
    {
        test_math_printf("%s: NULL\n", nombre);
        return;
    }

    test_math_printf("%s (%u×%u):\n", nombre, m->filas, m->columnas);
    p = m->pmatriz;

    for (i = 0; i < m->filas; i++)
    {
        test_math_printf("  [");
        for (j = 0; j < m->columnas; j++)
        {
            test_math_printf(" %7.3f", p[i * m->columnas + j]);
        }
        test_math_printf(" ]\n");
    }
}

int Test_Matriz_Producto(void)
{
    int result = TEST_OK;
    int ret;
    unsigned int i;

    /* Datos para test 1: 2×3 × 3×2 = 2×2 */
    float datos_m1_test1[6] = {1.0f, 2.0f, 3.0f,
                                4.0f, 5.0f, 6.0f};
    float datos_m2_test1[6] = {7.0f, 8.0f,
                                9.0f, 10.0f,
                                11.0f, 12.0f};
    float datos_m3_test1[4];
    float esperado_test1[4] = {58.0f, 64.0f,
                               139.0f, 154.0f};

    /* Datos para test 2: Matrices cuadradas 3×3 */
    float datos_m1_test2[9] = {1.0f, 0.0f, 2.0f,
                                0.0f, 1.0f, 1.0f,
                                2.0f, 1.0f, 0.0f};
    float datos_m2_test2[9] = {1.0f, 2.0f, 0.0f,
                                0.0f, 1.0f, 1.0f,
                                1.0f, 0.0f, 2.0f};
    float datos_m3_test2[9];
    float esperado_test2[9] = {3.0f, 2.0f, 4.0f,
                               1.0f, 1.0f, 3.0f,
                               2.0f, 5.0f, 1.0f};

    /* Datos para test 3: Vector fila × Vector columna = Escalar */
    float datos_m1_test3[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float datos_m2_test3[4] = {2.0f, 3.0f, 4.0f, 5.0f};
    float datos_m3_test3[1];
    float esperado_test3 = 40.0f;

    /* Datos para test 4: Matriz identidad */
    float datos_identidad[9] = {1.0f, 0.0f, 0.0f,
                                 0.0f, 1.0f, 0.0f,
                                 0.0f, 0.0f, 1.0f};
    float datos_matriz[9] = {5.0f, 6.0f, 7.0f,
                             8.0f, 9.0f, 10.0f,
                             11.0f, 12.0f, 13.0f};
    float datos_resultado[9];

    /* Datos para test 5: Dimensiones incompatibles */
    float datos_m1_test5[6] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    float datos_m2_test5[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float datos_m3_test5[6];

    MATRIZ m1, m2, m3;

    /* Inicializar el módulo */
    nsdsp_math_init();

    test_math_printf("\n=== Test Matriz Producto ===\n");

    /* Test 1: Multiplicación básica 2×3 × 3×2 = 2×2 */
    test_math_printf("\nTest 1: Multiplicación 2×3 × 3×2 = 2×2\n");

    m1.filas = 2;
    m1.columnas = 3;
    m1.pmatriz = datos_m1_test1;

    m2.filas = 3;
    m2.columnas = 2;
    m2.pmatriz = datos_m2_test1;

    m3.filas = 2;
    m3.columnas = 2;
    m3.pmatriz = datos_m3_test1;

    ret = nsdsp_math_api.product(&m1, &m2, &m3);

    if (ret != NSDSP_MATH_OK)
    {
        test_math_printf("ERROR: Fallo en multiplicación con dimensiones válidas\n");
        result = TEST_KO;
    }
    else
    {
        print_matriz("M1", &m1);
        print_matriz("M2", &m2);
        print_matriz("M3 (resultado)", &m3);

        for (i = 0; i < 4; i++)
        {
            if (!float_equals_math(datos_m3_test1[i], esperado_test1[i], EPSILON_MATH))
            {
                test_math_printf("ERROR: Elemento [%u] incorrecto: %.3f (esperado %.3f)\n",
                               i, datos_m3_test1[i], esperado_test1[i]);
                result = TEST_KO;
            }
        }

        if (result == TEST_OK)
        {
            test_math_printf("Resultado correcto\n");
        }
    }

    /* Test 2: Matrices cuadradas 3×3 */
    test_math_printf("\nTest 2: Matrices cuadradas 3×3\n");

    m1.filas = 3;
    m1.columnas = 3;
    m1.pmatriz = datos_m1_test2;

    m2.filas = 3;
    m2.columnas = 3;
    m2.pmatriz = datos_m2_test2;

    m3.filas = 3;
    m3.columnas = 3;
    m3.pmatriz = datos_m3_test2;

    ret = nsdsp_math_api.product(&m1, &m2, &m3);

    if (ret != NSDSP_MATH_OK)
    {
        test_math_printf("ERROR: Fallo en multiplicación de matrices cuadradas\n");
        result = TEST_KO;
    }
    else
    {
        for (i = 0; i < 9; i++)
        {
            if (!float_equals_math(datos_m3_test2[i], esperado_test2[i], EPSILON_MATH))
            {
                test_math_printf("ERROR: Elemento [%u] incorrecto: %.3f (esperado %.3f)\n",
                               i, datos_m3_test2[i], esperado_test2[i]);
                result = TEST_KO;
                break;
            }
        }

        if (i == 9)
        {
            test_math_printf("Matrices cuadradas: PASSED\n");
        }
    }

    /* Test 3: Vector fila × Vector columna */
    test_math_printf("\nTest 3: Vector fila (1×4) × Vector columna (4×1) = Escalar (1×1)\n");

    m1.filas = 1;
    m1.columnas = 4;
    m1.pmatriz = datos_m1_test3;

    m2.filas = 4;
    m2.columnas = 1;
    m2.pmatriz = datos_m2_test3;

    m3.filas = 1;
    m3.columnas = 1;
    m3.pmatriz = datos_m3_test3;

    ret = nsdsp_math_api.product(&m1, &m2, &m3);

    if (ret != NSDSP_MATH_OK)
    {
        test_math_printf("ERROR: Fallo en producto de vectores\n");
        result = TEST_KO;
    }
    else
    {
        test_math_printf("Resultado: %.3f (esperado %.3f)\n", datos_m3_test3[0], esperado_test3);

        if (!float_equals_math(datos_m3_test3[0], esperado_test3, EPSILON_MATH))
        {
            test_math_printf("ERROR: Producto escalar incorrecto\n");
            result = TEST_KO;
        }
        else
        {
            test_math_printf("Producto escalar: PASSED\n");
        }
    }

    /* Test 4: Matriz identidad */
    test_math_printf("\nTest 4: Multiplicación por matriz identidad\n");

    m1.filas = 3;
    m1.columnas = 3;
    m1.pmatriz = datos_identidad;

    m2.filas = 3;
    m2.columnas = 3;
    m2.pmatriz = datos_matriz;

    m3.filas = 3;
    m3.columnas = 3;
    m3.pmatriz = datos_resultado;

    ret = nsdsp_math_api.product(&m1, &m2, &m3);

    if (ret != NSDSP_MATH_OK)
    {
        test_math_printf("ERROR: Fallo con matriz identidad\n");
        result = TEST_KO;
    }
    else
    {
        for (i = 0; i < 9; i++)
        {
            if (!float_equals_math(datos_resultado[i], datos_matriz[i], EPSILON_MATH))
            {
                test_math_printf("ERROR: I×M != M en elemento [%u]\n", i);
                result = TEST_KO;
                break;
            }
        }

        if (i == 9)
        {
            test_math_printf("Matriz identidad: PASSED\n");
        }
    }

    /* Test 5: Dimensiones incompatibles */
    test_math_printf("\nTest 5: Dimensiones incompatibles (2×3 × 2×2)\n");

    m1.filas = 2;
    m1.columnas = 3;
    m1.pmatriz = datos_m1_test5;

    m2.filas = 2;  /* Incompatible: columnas(m1) != filas(m2) */
    m2.columnas = 2;
    m2.pmatriz = datos_m2_test5;

    m3.filas = 2;
    m3.columnas = 3;
    m3.pmatriz = datos_m3_test5;

    /* Inicializar m3 con valores no cero para verificar que se limpia */
    for (i = 0; i < 6; i++)
    {
        datos_m3_test5[i] = 99.0f;
    }

    ret = nsdsp_math_api.product(&m1, &m2, &m3);

    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó dimensiones incompatibles\n");
        result = TEST_KO;
    }
    else
    {
        /* Verificar que M3 se llenó con ceros */
        for (i = 0; i < 6; i++)
        {
            if (!float_equals_math(datos_m3_test5[i], 0.0f, EPSILON_MATH))
            {
                test_math_printf("ERROR: M3[%u] no es cero: %.3f\n", i, datos_m3_test5[i]);
                result = TEST_KO;
                break;
            }
        }

        if (i == 6)
        {
            test_math_printf("Detección de dimensiones incompatibles: PASSED\n");
        }
    }

    /* Test 6: Punteros NULL */
    test_math_printf("\nTest 6: Manejo de punteros NULL\n");

    ret = nsdsp_math_api.product(NULL, &m2, &m3);
    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó puntero NULL en M1\n");
        result = TEST_KO;
    }

    ret = nsdsp_math_api.product(&m1, NULL, &m3);
    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó puntero NULL en M2\n");
        result = TEST_KO;
    }

    ret = nsdsp_math_api.product(&m1, &m2, NULL);
    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó puntero NULL en M3\n");
        result = TEST_KO;
    }
    else
    {
        test_math_printf("Manejo de punteros NULL: PASSED\n");
    }

    if (result == TEST_OK)
        test_math_printf("\nTest Matriz Producto: PASSED\n");
    else
        test_math_printf("\nTest Matriz Producto: FAILED\n");

    return result;
}

int Test_Matriz_Suma(void)
{
    int result = TEST_OK;
    int ret;
    unsigned int i;

    /* Datos para test 1: Suma de matrices 3×3 */
    float datos_m1_suma[9] = {1.0f, 2.0f, 3.0f,
                              4.0f, 5.0f, 6.0f,
                              7.0f, 8.0f, 9.0f};
    float datos_m2_suma[9] = {9.0f, 8.0f, 7.0f,
                              6.0f, 5.0f, 4.0f,
                              3.0f, 2.0f, 1.0f};
    float datos_m3_suma[9];
    float esperado_suma[9] = {10.0f, 10.0f, 10.0f,
                              10.0f, 10.0f, 10.0f,
                              10.0f, 10.0f, 10.0f};

    /* Datos para test 2: Resta de matrices 3×3 */
    float esperado_resta[9] = {-8.0f, -6.0f, -4.0f,
                                -2.0f, 0.0f, 2.0f,
                                4.0f, 6.0f, 8.0f};

    /* Datos para test 3: Matrices de diferentes dimensiones */
    float datos_m1_diff[6] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f};
    float datos_m2_diff[4] = {1.0f, 2.0f, 3.0f, 4.0f};
    float datos_m3_diff[6];

    MATRIZ m1, m2, m3;

    /* Inicializar el módulo */
    nsdsp_math_init();

    test_math_printf("\n=== Test Matriz Suma/Resta ===\n");

    /* Test 1: Suma de matrices 3×3 */
    test_math_printf("\nTest 1: Suma de matrices 3×3\n");

    m1.filas = 3;
    m1.columnas = 3;
    m1.pmatriz = datos_m1_suma;

    m2.filas = 3;
    m2.columnas = 3;
    m2.pmatriz = datos_m2_suma;

    m3.filas = 3;
    m3.columnas = 3;
    m3.pmatriz = datos_m3_suma;

    ret = nsdsp_math_api.suma(&m1, &m2, &m3, 1);

    if (ret != NSDSP_MATH_OK)
    {
        test_math_printf("ERROR: Fallo en suma con dimensiones válidas\n");
        result = TEST_KO;
    }
    else
    {
        print_matriz("M1", &m1);
        print_matriz("M2", &m2);
        print_matriz("M3 (suma)", &m3);

        for (i = 0; i < 9; i++)
        {
            if (!float_equals_math(datos_m3_suma[i], esperado_suma[i], EPSILON_MATH))
            {
                test_math_printf("ERROR: Elemento [%u] incorrecto: %.3f (esperado %.3f)\n",
                               i, datos_m3_suma[i], esperado_suma[i]);
                result = TEST_KO;
            }
        }

        if (result == TEST_OK)
        {
            test_math_printf("Suma correcta\n");
        }
    }

    /* Test 2: Resta de matrices 3×3 */
    test_math_printf("\nTest 2: Resta de matrices 3×3\n");

    ret = nsdsp_math_api.suma(&m1, &m2, &m3, -1);

    if (ret != NSDSP_MATH_OK)
    {
        test_math_printf("ERROR: Fallo en resta con dimensiones válidas\n");
        result = TEST_KO;
    }
    else
    {
        print_matriz("M3 (resta M1-M2)", &m3);

        for (i = 0; i < 9; i++)
        {
            if (!float_equals_math(datos_m3_suma[i], esperado_resta[i], EPSILON_MATH))
            {
                test_math_printf("ERROR: Elemento [%u] incorrecto: %.3f (esperado %.3f)\n",
                               i, datos_m3_suma[i], esperado_resta[i]);
                result = TEST_KO;
            }
        }

        if (result == TEST_OK)
        {
            test_math_printf("Resta correcta\n");
        }
    }

    /* Test 3: Dimensiones incompatibles */
    test_math_printf("\nTest 3: Dimensiones incompatibles (2×3 + 2×2)\n");

    m1.filas = 2;
    m1.columnas = 3;
    m1.pmatriz = datos_m1_diff;

    m2.filas = 2;
    m2.columnas = 2;
    m2.pmatriz = datos_m2_diff;

    m3.filas = 2;
    m3.columnas = 3;
    m3.pmatriz = datos_m3_diff;

    /* Inicializar m3 con valores no cero para verificar que se limpia */
    for (i = 0; i < 6; i++)
    {
        datos_m3_diff[i] = 99.0f;
    }

    ret = nsdsp_math_api.suma(&m1, &m2, &m3, 1);

    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó dimensiones incompatibles\n");
        result = TEST_KO;
    }
    else
    {
        /* Verificar que M3 se llenó con ceros */
        for (i = 0; i < 6; i++)
        {
            if (!float_equals_math(datos_m3_diff[i], 0.0f, EPSILON_MATH))
            {
                test_math_printf("ERROR: M3[%u] no es cero: %.3f\n", i, datos_m3_diff[i]);
                result = TEST_KO;
                break;
            }
        }

        if (i == 6)
        {
            test_math_printf("Detección de dimensiones incompatibles: PASSED\n");
        }
    }

    /* Test 4: Punteros NULL */
    test_math_printf("\nTest 4: Manejo de punteros NULL\n");

    ret = nsdsp_math_api.suma(NULL, &m2, &m3, 1);
    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó puntero NULL en M1\n");
        result = TEST_KO;
    }

    ret = nsdsp_math_api.suma(&m1, NULL, &m3, 1);
    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó puntero NULL en M2\n");
        result = TEST_KO;
    }

    ret = nsdsp_math_api.suma(&m1, &m2, NULL, 1);
    if (ret != NSDSP_MATH_KO)
    {
        test_math_printf("ERROR: No detectó puntero NULL en M3\n");
        result = TEST_KO;
    }
    else
    {
        test_math_printf("Manejo de punteros NULL: PASSED\n");
    }

    /* Test 5: Verificar signo = 0 (debe sumar) */
    test_math_printf("\nTest 5: Signo = 0 (debe sumar)\n");

    m1.filas = 2;
    m1.columnas = 2;
    m1.pmatriz = datos_m1_diff;

    m2.filas = 2;
    m2.columnas = 2;
    m2.pmatriz = datos_m2_diff;

    m3.filas = 2;
    m3.columnas = 2;
    m3.pmatriz = datos_m3_diff;

    ret = nsdsp_math_api.suma(&m1, &m2, &m3, 0);

    if (ret != NSDSP_MATH_OK)
    {
        test_math_printf("ERROR: Fallo con signo = 0\n");
        result = TEST_KO;
    }
    else
    {
        /* Verificar que se realizó la suma */
        for (i = 0; i < 4; i++)
        {
            float esperado = datos_m1_diff[i] + datos_m2_diff[i];
            if (!float_equals_math(datos_m3_diff[i], esperado, EPSILON_MATH))
            {
                test_math_printf("ERROR: Elemento [%u] incorrecto: %.3f (esperado %.3f)\n",
                               i, datos_m3_diff[i], esperado);
                result = TEST_KO;
                break;
            }
        }

        if (i == 4)
        {
            test_math_printf("Suma con signo = 0: PASSED\n");
        }
    }

    if (result == TEST_OK)
        test_math_printf("\nTest Matriz Suma/Resta: PASSED\n");
    else
        test_math_printf("\nTest Matriz Suma/Resta: FAILED\n");

    return result;
}

int Run_All_NSDSP_Math_Tests(void)
{
    int total_result = TEST_OK;
    int test_result;
    time_t current_time;
    char time_string[100];

    /* Abrir archivo de log */
    math_test_log_file = fopen("NSDSP_Math_Tests_Result.txt", "a");
    if (math_test_log_file == NULL)
    {
        printf("WARNING: No se pudo abrir el archivo de log de NSDSP Math\n");
    }
    else
    {
        /* Escribir encabezado con fecha y hora */
        time(&current_time);
        strftime(time_string, sizeof(time_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
        test_math_printf("\n\n########################################\n");
        test_math_printf("# NSDSP Math Unit Tests\n");
        test_math_printf("# Fecha y hora: %s\n", time_string);
        test_math_printf("########################################\n");
    }

    test_math_printf("\n========================================\n");
    test_math_printf("    EJECUTANDO TESTS NSDSP MATH\n");
    test_math_printf("========================================\n");

    /* Ejecutar tests */
    test_result = Test_Matriz_Producto();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_result = Test_Matriz_Suma();
    if (test_result != TEST_OK) total_result = TEST_KO;

    test_math_printf("\n========================================\n");
    if (total_result == TEST_OK)
        test_math_printf("TODOS LOS TESTS NSDSP MATH PASARON CORRECTAMENTE\n");
    else
        test_math_printf("ALGUNOS TESTS NSDSP MATH FALLARON\n");
    test_math_printf("========================================\n\n");

    /* Escribir resumen final en el archivo */
    if (math_test_log_file != NULL)
    {
        test_math_printf("\n# Resumen Final: ");
        if (total_result == TEST_OK)
            test_math_printf("SUCCESS - Todos los tests pasaron\n");
        else
            test_math_printf("FAILURE - Algunos tests fallaron\n");
        test_math_printf("########################################\n\n");

        fclose(math_test_log_file);
        math_test_log_file = NULL;
    }

    return total_result;
}

#endif /* DEBUG */
