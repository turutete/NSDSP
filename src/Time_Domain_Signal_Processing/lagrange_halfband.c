/** \page lagrange_halfband FILTROS DE MEDIA BANDA LAGRANGE
 * \brief Módulo de generación de coeficientes para filtros de media banda de Lagrange
 *
 * Este módulo implementa la generación de coeficientes para filtros de media banda de Lagrange.
 * Los filtros de media banda son filtros digitales especiales que tienen la característica de que
 * la mitad de sus coeficientes son cero, lo que los hace computacionalmente eficientes para
 * aplicaciones de decimación e interpolación por 2.
 *
 * El filtro implementado tiene la forma:
 * \f[
 * H_0(z) = \frac{1}{2} + \sum_{n=1}^{m} h_m(2n-1) \cdot (z^{-(2n-1)} + z^{2n-1})
 * \f]
 *
 * donde los coeficientes \f$h_m(2n-1)\f$ están dados por:
 * \f[
 * h_m(2n-1) = \frac{(-1)^{n+m-1}}{(m-n)!(m-1+n)!(2n-1)} \cdot \prod_{k=1}^{2m}(m-k+\frac{1}{2})
 * \f]
 *
 * El orden del filtro resultante es 4m-2, con 4m-1 coeficientes totales.
 *
 * \section uso_lagrange Uso del módulo
 *
 * Para utilizar este módulo:
 * 1. Incluir el archivo de cabecera lagrange_halfband.h
 * 2. Llamar a lagrange_halfband() con el parámetro m deseado
 * 3. El array de coeficientes debe estar previamente allocado
 *
 * Ejemplo:
 * \code
 * #include "lagrange_halfband.h"
 * 
 * int main(void) {
 *     int m = 3;
 *     int orden = 4 * m - 1;  // 11 coeficientes
 *     float h0[11];
 *     
 *     int result = lagrange_halfband(m, h0);
 *     if (result == LAGRANGE_OK) {
 *         // Usar los coeficientes h0[0] a h0[10]
 *     }
 *     return 0;
 * }
 * \endcode
 *
 * \section funciones_lagrange Descripción de funciones
 *
 * \subsection lagrange_halfband_func lagrange_halfband
 * Genera los coeficientes del filtro de media banda de Lagrange de orden 4m-2.
 *
 * \dot
 * digraph lagrange_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="lagrange_halfband(m, h0)", fillcolor=lightgreen];
 *   VALIDATE [label="Validar m >= 1", shape=diamond, fillcolor=lightyellow];
 *   INIT [label="Inicializar array\nh0[2m] = 0.5", fillcolor=lightblue];
 *   LOOP [label="Para l=1 hasta m", shape=diamond, fillcolor=lightcyan];
 *   PRODUCT [label="Calcular productorio\nΠ(m-k+0.5)", fillcolor=lightpink];
 *   COEFF [label="Calcular hm usando\nfórmula de Lagrange", fillcolor=lightpink];
 *   ASSIGN [label="h0[2m±(2l-1)] = hm", fillcolor=lightgreen];
 *   NEXT [label="l++", fillcolor=lightcyan];
 *   RETURN_OK [label="return LAGRANGE_OK", fillcolor=lightgreen];
 *   RETURN_ERROR [label="return LAGRANGE_KO", fillcolor=lightcoral];
 *
 *   START -> VALIDATE;
 *   VALIDATE -> INIT [label="m >= 1"];
 *   VALIDATE -> RETURN_ERROR [label="m < 1"];
 *   INIT -> LOOP;
 *   LOOP -> PRODUCT [label="l <= m"];
 *   LOOP -> RETURN_OK [label="l > m"];
 *   PRODUCT -> COEFF;
 *   COEFF -> ASSIGN;
 *   ASSIGN -> NEXT;
 *   NEXT -> LOOP;
 * }
 * \enddot
 *
 * La función implementa el algoritmo:
 * 1. Inicializa todos los coeficientes a cero
 * 2. Establece el coeficiente central h0[2m] = 0.5
 * 3. Para cada l de 1 a m:
 *    - Calcula el productorio \f$\prod_{k=1}^{2m}(m-k+\frac{1}{2})\f$
 *    - Calcula el coeficiente usando la fórmula de Lagrange
 *    - Asigna simétricamente: h0[2m+(2l-1)] = h0[2m-(2l-1)] = hm
 *
 * \param m Parámetro del filtro (entero positivo >= 1)
 * \param h0 Array de salida para los coeficientes (debe tener tamaño 4m-1)
 * \return LAGRANGE_OK si éxito, LAGRANGE_KO si error en parámetros
 *
 * \subsection factorial_func factorial
 * Calcula el factorial de un número entero no negativo.
 * Implementación iterativa para evitar recursión y mejorar eficiencia.
 *
 * \param n Número entero no negativo
 * \return n! (factorial de n)
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_lagrange Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 04/08/2025 | Dr. Carlos Romero | 1 | Implementación inicial desde Matlab |
 *
 * \copyright ZGR R&D AIE
 */

#include <stdlib.h>
#include "lagrange_halfband.h"

/* Declaración de funciones */
int lagrange_halfband(int m, float *h0);
unsigned long factorial(int n);

/* Definición de funciones */

int lagrange_halfband(int m, float *h0)
{
    int orden;
    int l, k;
    float productorio;
    float hm;
    unsigned long fact_m_minus_l;
    unsigned long fact_m_plus_l_minus_1;
    int sign;
    int center_index;
    int pos_index, neg_index;

    /* Validar parámetros de entrada */
    if (m < 1 || h0 == NULL)
    {
        return LAGRANGE_KO;
    }

    orden = 4 * m - 1;
    center_index = 2 * m - 1;  /* Índice central (base 0) */

    /* Inicializar array a cero */
    for (k = 0; k < orden; k++)
    {
        h0[k] = 0.0f;
    }

    /* Establecer coeficiente central */
    h0[center_index] = 0.5f;

    /* Calcular coeficientes simétricos */
    for (l = 1; l <= m; l++)
    {
        /* Calcular productorio Π(m-k+1/2) para k=1 hasta 2m */
        productorio = 1.0f;
        for (k = 1; k <= 2 * m; k++)
        {
            productorio *= (float)(m - k) + 0.5f;
        }

        /* Calcular factoriales */
        fact_m_minus_l = factorial(m - l);
        fact_m_plus_l_minus_1 = factorial(m - 1 + l);

        /* Calcular signo (-1)^(l+m-1) */
        sign = ((l + m - 1) % 2 == 0) ? 1 : -1;

        /* Calcular coeficiente hm */
        hm = (float)sign * productorio / 
             ((float)fact_m_minus_l * (float)fact_m_plus_l_minus_1 * (float)(2 * l - 1));

        /* Asignar simétricamente */
        pos_index = center_index + (2 * l - 1);
        neg_index = center_index - (2 * l - 1);

        if (pos_index < orden)
        {
            h0[pos_index] = hm;
        }
        if (neg_index >= 0)
        {
            h0[neg_index] = hm;
        }
    }

    return LAGRANGE_OK;
}

unsigned long factorial(int n)
{
    unsigned long result;
    int i;

    if (n < 0)
    {
        return 0;  /* Factorial no definido para negativos */
    }

    result = 1;
    for (i = 2; i <= n; i++)
    {
        result *= (unsigned long)i;
    }

    return result;
}