/** \page   fir_filter   FIR Filter
 * \brief Módulo de filtrado FIR de propósito general
 *
 * Este módulo de SW implementa un método general para realizar el filtrado de una nueva muestra de una señal
 * de entrada xn mediante un filtro FIR dado también como parámetro de entrada. Utiliza buffers circulares
 * para la implementación eficiente de la línea de retardo, permitiendo crear múltiples instancias de
 * filtros FIR simultáneamente.
 *
 * \section teoria_fir Teoría del filtrado FIR
 *
 * Un filtro FIR (Finite Impulse Response) implementa la ecuación de diferencias:
 * \f[
 * y[n] = \sum_{k=0}^{N-1} h[k] \cdot x[n-k]
 * \f]
 *
 * donde:
 * - \f$y[n]\f$ es la salida del filtro en el instante n
 * - \f$h[k]\f$ son los coeficientes del filtro (respuesta impulsional)
 * - \f$x[n-k]\f$ son las muestras de entrada retardadas
 * - N es el número de coeficientes del filtro
 *
 * \section arquitectura_fir Arquitectura del filtro FIR
 *
 * \dot
 * digraph fir_architecture {
 *   rankdir=LR;
 *   node [shape=box];
 *
 *   xn [label="x[n]", shape=plaintext];
 *   z1 [label="z⁻¹", style=filled, fillcolor=lightblue];
 *   z2 [label="z⁻¹", style=filled, fillcolor=lightblue];
 *   z3 [label="z⁻¹", style=filled, fillcolor=lightblue];
 *   zn [label="z⁻¹", style=filled, fillcolor=lightblue];
 *
 *   h0 [label="h[0]", shape=circle, style=filled, fillcolor=lightgreen];
 *   h1 [label="h[1]", shape=circle, style=filled, fillcolor=lightgreen];
 *   h2 [label="h[2]", shape=circle, style=filled, fillcolor=lightgreen];
 *   h3 [label="h[3]", shape=circle, style=filled, fillcolor=lightgreen];
 *   hn [label="h[N-1]", shape=circle, style=filled, fillcolor=lightgreen];
 *
 *   sum [label="Σ", shape=circle, style=filled, fillcolor=lightyellow];
 *   yn [label="y[n]", shape=plaintext];
 *
 *   xn -> h0;
 *   xn -> z1 -> h1;
 *   z1 -> z2 -> h2;
 *   z2 -> z3 -> h3;
 *   z3 -> zn -> hn;
 *
 *   h0 -> sum;
 *   h1 -> sum;
 *   h2 -> sum;
 *   h3 -> sum;
 *   hn -> sum;
 *
 *   sum -> yn;
 * }
 * \enddot
 *
 * \section uso_fir Uso del módulo
 *
 * El módulo implementa una API con 2 funciones principales:
 *
 * Para utilizar este módulo:
 * 1. Inicializar con Init_Fir()
 * 2. Crear filtro con Get_Fir()
 * 3. Filtrar muestras con fir_filter()
 *
 * Ejemplo:
 * \code
 * #include "fir_filter.h"
 *
 * int main(void) {
 *     // Inicializar módulo
 *     Init_Fir();
 *
 *     // Definir filtro promediador
 *     float coefs[5] = {0.2f, 0.2f, 0.2f, 0.2f, 0.2f};
 *     float z_buffer[5];
 *
 *     // Crear objeto filtro
 *     FIR_FILTER_OBJECT filtro = fir_api.get_fir(5, coefs, z_buffer);
 *
 *     // Procesar muestras
 *     for (int i = 0; i < 100; i++) {
 *         float muestra = obtener_muestra();
 *         float salida = fir_api.fir_filter(muestra, &filtro);
 *         procesar_salida(salida);
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \section funciones_fir Descripción de funciones
 *
 * \subsection init_fir_func Init_Fir
 * Inicializa la estructura de punteros a funciones fir_api (Public Service Endpoints).
 * Esta función debe ser llamada antes de usar cualquier servicio del módulo.
 * Asigna los punteros a las funciones Get_Fir y fir_filter.
 *
 * \subsection get_fir_func Get_Fir
 * Crea e inicializa un servicio de filtrado FIR.
 *
 * \dot
 * digraph get_fir_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Get_Fir(ncoef, pcoef, pz)", fillcolor=lightgreen];
 *   CLEAR [label="Limpiar buffer Z\na cero", fillcolor=lightyellow];
 *   INIT [label="Inicializar objeto:\n- ncoef\n- pcoef\n- pz\n- p_write", fillcolor=lightblue];
 *   RETURN [label="return objeto", fillcolor=lightgreen];
 *
 *   START -> CLEAR -> INIT -> RETURN;
 * }
 * \enddot
 *
 * \param ncoef Número de coeficientes del filtro FIR
 * \param pcoef Puntero a un vector de coeficientes del filtro FIR. Debe tener ncoef elementos.
 * \param pz Puntero al buffer de retrasos del filtro fir. Debe tener longitud ncoef
 * \return La función devuelve un objeto FIR_FILTER_OBJECT con el puntero de escritura inicializado, y el buffer Z del filtro limpiado.
 *
 * \subsection fir_filter_func fir_filter
 * Realiza el filtrado FIR de una muestra utilizando buffer circular.
 *
 * \dot
 * digraph fir_filter_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="fir_filter(xn, pfiltro)", fillcolor=lightgreen];
 *   VALIDATE [label="Validar parámetros", shape=diamond, fillcolor=lightyellow];
 *   WRITE [label="Escribir xn en\nbuffer circular", fillcolor=lightblue];
 *   UPDATE_PTR [label="Actualizar puntero\nde escritura", fillcolor=lightblue];
 *   CONV [label="Realizar convolución\ncon buffer circular", fillcolor=lightpink];
 *   RETURN_Y [label="return y", fillcolor=lightgreen];
 *   RETURN_0 [label="return 0.0", fillcolor=lightcoral];
 *
 *   START -> VALIDATE;
 *   VALIDATE -> WRITE [label="OK"];
 *   VALIDATE -> RETURN_0 [label="Error"];
 *   WRITE -> UPDATE_PTR -> CONV -> RETURN_Y;
 * }
 * \enddot
 *
 * \section buffer_circular Funcionamiento del Buffer Circular
 *
 * \dot
 * digraph buffer_circular {
 *   rankdir=LR;
 *   node [shape=record, style=filled];
 *
 *   buffer [label="<0>x[n-4]|<1>x[n-3]|<2>x[n-2]|<3>x[n-1]|<4>x[n]", fillcolor=lightblue];
 *   p_write [label="p_write", shape=plaintext];
 *   p_read [label="p_read", shape=plaintext];
 *
 *   p_write -> buffer:4 [label="Escribir nueva\nmuestra aquí"];
 *   p_read -> buffer:4 [label="Leer desde aquí\npara convolución"];
 *
 *   wraparound [label="Al llegar al final,\nvolver al inicio\n(wraparound)", shape=box, style=filled, fillcolor=lightyellow];
 * }
 * \enddot
 *
 * El algoritmo implementa:
 * 1. Validación de parámetros de entrada
 * 2. Escritura de la nueva muestra en el buffer circular
 * 3. Actualización del puntero de escritura con wraparound
 * 4. Convolución usando la ecuación FIR con buffer circular
 * 5. Retorno del resultado filtrado
 *
 * \param xn Muestra x(n) de la secuencia de entrada
 * \param pfiltro Puntero a un objeto FIR_FILTER_OBJECT
 * \return Retorna el FLOAT32 y, que es el resultado del filtrado de la muestra xn
 *
 * \section objetos_fir FIR_FILTER_OBJECT
 *
 * Este objeto está definido en fir_filter.h y contiene:
 *
 * - **ncoef**: Número de coeficientes del filtro
 * - **p_write**: Puntero de escritura en el buffer circular de retrasos Z del filtro
 * - **pcoef**: Puntero al buffer FLOAT32 con los coeficientes del filtro
 * - **pz**: Puntero al buffer circular de retrasos Z del filtro
 *
 * \section excepciones_fir Manejo de Excepciones
 *
 * Cualquier excepción en la ejecución de la operación de filtrado resultará en una salida y=0.
 * Las condiciones de error incluyen:
 * - Puntero NULL al objeto filtro
 * - Número de coeficientes excesivo (> MAX_FIR_LENGTH)
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_fir_filter Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 18/08/2025 | Dr. Carlos Romero | 1 | Primera edición |
 * | 28/08/2025 | Dr. Carlos Romero | 2 | Documentación Doxygen completa con Graphviz |
 *
 * \copyright  ZGR R&D AIE
 */

 #include "fir_filter.h"
 #include <stdio.h>

 /* Declaración de funciones */
 void Init_Fir(void);
 FIR_FILTER_OBJECT Get_Fir(unsigned int, float *, float *);
 float fir_filter (float, FIR_FILTER_OBJECT *);

 /* Definición de Variables globales */
 FIR_FILTER_API fir_api;

 /* Definición de funciones */

 void Init_Fir(void)
 {
     fir_api.fir_filter=fir_filter;
     fir_api.get_fir=Get_Fir;
 }

 FIR_FILTER_OBJECT Get_Fir(unsigned int ncoef, float * pcoef, float * pz)
 {
     FIR_FILTER_OBJECT objeto;
     unsigned int index;
     float * pw;

     pw=pz;
     if (pw!=NULL)
     {
         for (index=0;index<ncoef;index++)
            *(pw++)=0;
     }
     objeto.ncoef=ncoef;
     objeto.pcoef=pcoef;
     objeto.pz=pz;
     objeto.p_write=pz;
     return objeto;
 }

 float fir_filter(float xn, FIR_FILTER_OBJECT * pfir)
 {
     unsigned int index, N;
     float * pmax;
     float * pmin;
     float * pinit;
     float y;
     float * pcoef_temp;

     if (pfir==NULL)
     {
         return 0.0f;
     }

     y=0.0f;
     N=pfir->ncoef;
     if (N>MAX_FIR_LENGTH)
     {
         return 0.0f;
     }

     pmin=pfir->pz;
     pmax=(pfir->pz)+(pfir->ncoef);
     pinit=pfir->p_write;
     *(pfir->p_write++)=xn;

     if (pfir->p_write==pmax)
     {
         pfir->p_write=pfir->pz;
     }


    pcoef_temp=pfir->pcoef;

     for (index=0;index<N;index++)
     {
         y+=(*(pcoef_temp++))*(*(pinit--));

         if (pinit<pmin)
         {
             pinit=pmax;
             pinit--;
         }
     }
     return y;
 }
