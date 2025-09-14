/** \page nsdsp_math OPERACIONES MATEMÁTICAS
 * \brief Módulo de operaciones matemáticas para la librería NSDSP
 *
 * Este módulo implementa operaciones matemáticas avanzadas para el procesamiento
 * digital de señales, incluyendo operaciones con matrices. El módulo está diseñado
 * para trabajar con memoria estática y ser eficiente en sistemas embebidos.
 * Utiliza una estructura API para acceder a todas las funciones disponibles.
 *
 * \section uso_math Uso del módulo
 *
 * Para utilizar este módulo:
 * 1. Inicializar con nsdsp_math_init() (llamado automáticamente por Init_NSDSP())
 * 2. Crear objetos MATRIZ con las dimensiones deseadas y punteros a buffers de datos
 * 3. Llamar a las funciones matemáticas a través de nsdsp_math_api
 *
 * Ejemplo de uso para operaciones con matrices:
 * \code
 * #include "nsdsp_math.h"
 *
 * int main(void) {
 *     // Inicializar el módulo
 *     nsdsp_math_init();
 *
 *     // Multiplicación de matrices
 *     float datos_m1[6] = {1, 2, 3, 4, 5, 6};  // Matriz 2x3
 *     float datos_m2[6] = {7, 8, 9, 10, 11, 12};  // Matriz 3x2
 *     float datos_m3[4];  // Matriz resultado 2x2
 *
 *     MATRIZ m1 = {2, 3, datos_m1};
 *     MATRIZ m2 = {3, 2, datos_m2};
 *     MATRIZ m3 = {2, 2, datos_m3};
 *
 *     int resultado = nsdsp_math_api.product(&m1, &m2, &m3);
 *
 *     // Suma de matrices
 *     float datos_a[9] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
 *     float datos_b[9] = {9, 8, 7, 6, 5, 4, 3, 2, 1};
 *     float datos_c[9];
 *
 *     MATRIZ ma = {3, 3, datos_a};
 *     MATRIZ mb = {3, 3, datos_b};
 *     MATRIZ mc = {3, 3, datos_c};
 *
 *     // Suma: ma + mb
 *     resultado = nsdsp_math_api.suma(&ma, &mb, &mc, 1);
 *
 *     // Resta: ma - mb
 *     resultado = nsdsp_math_api.suma(&ma, &mb, &mc, -1);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \section funciones_math Descripción de funciones
 *
 * \subsection nsdsp_math_init_func nsdsp_math_init
 * Inicializa la estructura de punteros a funciones nsdsp_math_api.
 * Esta función debe ser llamada antes de usar cualquier servicio del módulo.
 * Asigna los punteros a las funciones matriz_producto y matriz_suma en los campos
 * product y suma de la API respectivamente.
 *
 * \subsection matriz_producto_func matriz_producto
 * Realiza el producto de dos matrices M1 y M2, almacenando el resultado en M3.
 *
 * La función implementa el producto matricial estándar:
 * \f[
 * M3_{ij} = \sum_{k=1}^{n} M1_{ik} \cdot M2_{kj}
 * \f]
 *
 * donde M1 es de dimensión a×b, M2 es de dimensión b×c, y M3 es de dimensión a×c.
 *
 * \dot
 * digraph matriz_producto_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="matriz_producto(PM1, PM2, PM3)", fillcolor=lightgreen];
 *   VALIDATE [label="Validar punteros\nno NULL", shape=diamond, fillcolor=lightyellow];
 *   CHECK_DIM [label="Verificar dimensiones:\ncol(M1)==fil(M2)\nfil(M1)==fil(M3)\ncol(M2)==col(M3)", shape=diamond, fillcolor=lightcyan];
 *   CLEAR_M3 [label="Limpiar M3\na ceros", fillcolor=lightpink];
 *   LOOP_F [label="Para cada fila f\nde M1", shape=diamond, fillcolor=lightblue];
 *   LOOP_C [label="Para cada columna c\nde M2", shape=diamond, fillcolor=lightblue];
 *   CALC_DOT [label="Calcular producto punto:\nM3[f,c] = M1[f,:] · M2[:,c]", fillcolor=lightgreen];
 *   RETURN_OK [label="return NSDSP_MATH_OK", fillcolor=lightgreen];
 *   RETURN_ERROR [label="return NSDSP_MATH_KO", fillcolor=lightcoral];
 *
 *   START -> VALIDATE;
 *   VALIDATE -> CHECK_DIM [label="OK"];
 *   VALIDATE -> CLEAR_M3 [label="NULL"];
 *   CHECK_DIM -> LOOP_F [label="OK"];
 *   CHECK_DIM -> CLEAR_M3 [label="Error"];
 *   CLEAR_M3 -> RETURN_ERROR;
 *   LOOP_F -> LOOP_C [label="f < filas(M1)"];
 *   LOOP_F -> RETURN_OK [label="f >= filas(M1)"];
 *   LOOP_C -> CALC_DOT [label="c < col(M2)"];
 *   LOOP_C -> LOOP_F [label="c >= col(M2)"];
 *   CALC_DOT -> LOOP_C;
 * }
 * \enddot
 *
 * \param PM1 Puntero a la primera matriz (a×b)
 * \param PM2 Puntero a la segunda matriz (b×c)
 * \param PM3 Puntero a la matriz resultado (a×c)
 * \return NSDSP_MATH_OK (0) si el producto se realizó correctamente, NSDSP_MATH_KO (-1) si hubo error
 *
 * \subsection matriz_suma_func matriz_suma
 * Realiza la suma o resta de dos matrices M1 y M2, almacenando el resultado en M3.
 *
 * La función implementa:
 * - Si signo >= 0: \f$ M3 = M1 + M2 \f$
 * - Si signo < 0: \f$ M3 = M1 - M2 \f$
 *
 * Donde cada elemento se calcula como:
 * \f[
 * M3_{ij} = M1_{ij} \pm M2_{ij}
 * \f]
 *
 * \dot
 * digraph matriz_suma_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="matriz_suma(PM1, PM2, PM3, signo)", fillcolor=lightgreen];
 *   VALIDATE [label="Validar punteros\nno NULL", shape=diamond, fillcolor=lightyellow];
 *   CHECK_DIM [label="Verificar dimensiones:\nfil(M1)==fil(M2)==fil(M3)\ncol(M1)==col(M2)==col(M3)", shape=diamond, fillcolor=lightcyan];
 *   CLEAR_M3 [label="Limpiar M3\na ceros", fillcolor=lightpink];
 *   CHECK_SIGN [label="¿signo >= 0?", shape=diamond, fillcolor=lightyellow];
 *   LOOP_SUMA [label="Para cada elemento:\nM3[i] = M1[i] + M2[i]", fillcolor=lightblue];
 *   LOOP_RESTA [label="Para cada elemento:\nM3[i] = M1[i] - M2[i]", fillcolor=lightblue];
 *   RETURN_OK [label="return NSDSP_MATH_OK", fillcolor=lightgreen];
 *   RETURN_ERROR [label="return NSDSP_MATH_KO", fillcolor=lightcoral];
 *
 *   START -> VALIDATE;
 *   VALIDATE -> CHECK_DIM [label="OK"];
 *   VALIDATE -> CLEAR_M3 [label="NULL"];
 *   CHECK_DIM -> CHECK_SIGN [label="OK"];
 *   CHECK_DIM -> CLEAR_M3 [label="Error"];
 *   CLEAR_M3 -> RETURN_ERROR;
 *   CHECK_SIGN -> LOOP_SUMA [label="Sí"];
 *   CHECK_SIGN -> LOOP_RESTA [label="No"];
 *   LOOP_SUMA -> RETURN_OK;
 *   LOOP_RESTA -> RETURN_OK;
 * }
 * \enddot
 *
 * \param PM1 Puntero a la primera matriz sumando
 * \param PM2 Puntero a la segunda matriz sumando
 * \param PM3 Puntero a la matriz resultado
 * \param signo Si >= 0 realiza suma, si < 0 realiza resta
 * \return NSDSP_MATH_OK (0) si la operación se realizó correctamente, NSDSP_MATH_KO (-1) si hubo error
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_math Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 10/09/2025 | Dr. Carlos Romero | 1 | Implementación inicial con multiplicación de matrices |
 * | 10/09/2025 | Dr. Carlos Romero | 2 | Añadida estructura API para acceso a funciones |
 * | 13/09/2025 | Dr. Carlos Romero | 3 | Añadida función de suma/resta de matrices |
 *
 * \copyright ZGR R&D AIE
 */

#include "nsdsp_math.h"
#include <stddef.h>

/* Declaración de funciones */
void nsdsp_math_init(void);
int matriz_producto(MATRIZ * PM1, MATRIZ * PM2, MATRIZ * PM3);
int matriz_suma(MATRIZ * PM1, MATRIZ * PM2, MATRIZ * PM3, int signo);

/* Definición de variables globales */
NSDSP_MATH_API nsdsp_math_api;

/* Definición de funciones */

void nsdsp_math_init(void)
{
    /* Inicializar punteros de la API */
    nsdsp_math_api.product = matriz_producto;
    nsdsp_math_api.suma = matriz_suma;
}

int matriz_producto(MATRIZ * PM1, MATRIZ * PM2, MATRIZ * PM3)
{
    unsigned int f, c, index;
    unsigned int filas_m1, columnas_m1;
    unsigned int filas_m2, columnas_m2;
    unsigned int filas_m3, columnas_m3;
    float * p_m1;
    float * p_m2;
    float * p_m3;
    float acumulador;
    unsigned int offset_fila_m1;
    unsigned int offset_elemento_m3;

    /* Validar punteros de entrada */
    if (PM1 == NULL || PM2 == NULL || PM3 == NULL)
    {
        /* Si PM3 es válido, llenar con ceros */
        if (PM3 != NULL && PM3->pmatriz != NULL)
        {
            p_m3 = PM3->pmatriz;
            for (index = 0; index < (PM3->filas * PM3->columnas); index++)
            {
                p_m3[index] = 0.0f;
            }
        }
        return NSDSP_MATH_KO;
    }

    /* Validar punteros a datos */
    if (PM1->pmatriz == NULL || PM2->pmatriz == NULL || PM3->pmatriz == NULL)
    {
        /* Llenar M3 con ceros si es posible */
        if (PM3->pmatriz != NULL)
        {
            p_m3 = PM3->pmatriz;
            for (index = 0; index < (PM3->filas * PM3->columnas); index++)
            {
                p_m3[index] = 0.0f;
            }
        }
        return NSDSP_MATH_KO;
    }

    /* Obtener dimensiones */
    filas_m1 = PM1->filas;
    columnas_m1 = PM1->columnas;
    filas_m2 = PM2->filas;
    columnas_m2 = PM2->columnas;
    filas_m3 = PM3->filas;
    columnas_m3 = PM3->columnas;

    /* Obtener punteros a datos */
    p_m1 = PM1->pmatriz;
    p_m2 = PM2->pmatriz;
    p_m3 = PM3->pmatriz;

    /* Verificar compatibilidad de dimensiones */
    /* M1(a×b) × M2(b×c) = M3(a×c) */
    if (columnas_m1 != filas_m2 || filas_m1 != filas_m3 || columnas_m2 != columnas_m3)
    {
        /* Dimensiones incompatibles, llenar M3 con ceros */
        for (index = 0; index < (filas_m3 * columnas_m3); index++)
        {
            p_m3[index] = 0.0f;
        }
        return NSDSP_MATH_KO;
    }

    /* Realizar multiplicación de matrices */
    /* Para cada fila de M1 */
    for (f = 0; f < filas_m1; f++)
    {
        /* Calcular offset de la fila actual en M1 */
        offset_fila_m1 = f * columnas_m1;

        /* Para cada columna de M2 */
        for (c = 0; c < columnas_m2; c++)
        {
            /* Inicializar acumulador para el elemento M3[f,c] */
            acumulador = 0.0f;

            /* Producto punto entre fila f de M1 y columna c de M2 */
            for (index = 0; index < columnas_m1; index++)
            {
                /* M3[f,c] += M1[f,index] * M2[index,c] */
                acumulador += p_m1[offset_fila_m1 + index] * p_m2[index * columnas_m2 + c];
            }

            /* Almacenar resultado en M3[f,c] */
            offset_elemento_m3 = f * columnas_m3 + c;
            p_m3[offset_elemento_m3] = acumulador;
        }
    }

    return NSDSP_MATH_OK;
}

int matriz_suma(MATRIZ * PM1, MATRIZ * PM2, MATRIZ * PM3, int signo)
{
    unsigned int index;
    unsigned int total_elementos;
    float * p_m1;
    float * p_m2;
    float * p_m3;

    /* Validar punteros de entrada */
    if (PM1 == NULL || PM2 == NULL || PM3 == NULL)
    {
        /* Si PM3 es válido, llenar con ceros */
        if (PM3 != NULL && PM3->pmatriz != NULL)
        {
            p_m3 = PM3->pmatriz;
            total_elementos = PM3->filas * PM3->columnas;
            for (index = 0; index < total_elementos; index++)
            {
                p_m3[index] = 0.0f;
            }
        }
        return NSDSP_MATH_KO;
    }

    /* Validar punteros a datos */
    if (PM1->pmatriz == NULL || PM2->pmatriz == NULL || PM3->pmatriz == NULL)
    {
        /* Llenar M3 con ceros si es posible */
        if (PM3->pmatriz != NULL)
        {
            p_m3 = PM3->pmatriz;
            total_elementos = PM3->filas * PM3->columnas;
            for (index = 0; index < total_elementos; index++)
            {
                p_m3[index] = 0.0f;
            }
        }
        return NSDSP_MATH_KO;
    }

    /* Verificar que todas las matrices tienen las mismas dimensiones */
    if (PM1->filas != PM2->filas || PM1->filas != PM3->filas ||
        PM1->columnas != PM2->columnas || PM1->columnas != PM3->columnas)
    {
        /* Dimensiones incompatibles, llenar M3 con ceros */
        p_m3 = PM3->pmatriz;
        total_elementos = PM3->filas * PM3->columnas;
        for (index = 0; index < total_elementos; index++)
        {
            p_m3[index] = 0.0f;
        }
        return NSDSP_MATH_KO;
    }

    /* Obtener punteros a datos */
    p_m1 = PM1->pmatriz;
    p_m2 = PM2->pmatriz;
    p_m3 = PM3->pmatriz;

    /* Calcular número total de elementos */
    total_elementos = PM1->filas * PM1->columnas;

    /* Realizar suma o resta según el signo */
    if (signo >= 0)
    {
        /* Suma: M3 = M1 + M2 */
        for (index = 0; index < total_elementos; index++)
        {
            p_m3[index] = p_m1[index] + p_m2[index];
        }
    }
    else
    {
        /* Resta: M3 = M1 - M2 */
        for (index = 0; index < total_elementos; index++)
        {
            p_m3[index] = p_m1[index] - p_m2[index];
        }
    }

    return NSDSP_MATH_OK;
}
