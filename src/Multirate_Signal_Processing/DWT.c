/** \page   wavelet_transform   Transformada Wavelet Discreta (DWT)
 * \brief Implementación de la Transformada Wavelet Discreta mediante el algoritmo de Mallat
 *
 * Este fichero implementa el servicio de transformada Wavelet Discreta, mediante el algoritmo de Mallat.
 * El servicio permite crear múltiples objetos DWT concurrentes, cada uno con su propio conjunto de
 * buffers y filtros. Todos los servicios usan el mismo tipo de filtro Wavelet, que se selecciona en
 * tiempo de compilación mediante directivas de preprocesador.
 *
 * \section teoria_dwt Teoría de la Transformada Wavelet
 *
 * La transformada wavelet discreta descompone una señal en coeficientes de aproximación (paso bajo)
 * y detalle (paso alto) mediante bancos de filtros con decimación por factor 2:
 *
 * \dot
 * digraph dwt_theory {
 *   rankdir=LR;
 *   node [shape=box];
 *
 *   xn [label="x[n]", shape=plaintext];
 *   h0 [label="h₀[n]\n(Paso Bajo)", style=filled, fillcolor=lightblue];
 *   h1 [label="h₁[n]\n(Paso Alto)", style=filled, fillcolor=lightpink];
 *   down2_1 [label="↓2", shape=circle, style=filled, fillcolor=lightyellow];
 *   down2_2 [label="↓2", shape=circle, style=filled, fillcolor=lightyellow];
 *   approx [label="Aproximación\n(LP)", shape=ellipse, style=filled, fillcolor=lightblue];
 *   detail [label="Detalle\n(HP)", shape=ellipse, style=filled, fillcolor=lightpink];
 *
 *   xn -> h0 -> down2_1 -> approx;
 *   xn -> h1 -> down2_2 -> detail;
 * }
 * \enddot
 *
 * \section algoritmo_mallat Algoritmo de Mallat
 *
 * El algoritmo de Mallat implementa la descomposición multinivel:
 *
 * \f[
 * \text{Aproximación: } A_j[k] = \sum_n h_0[n-2k] A_{j-1}[n]
 * \f]
 * \f[
 * \text{Detalle: } D_j[k] = \sum_n h_1[n-2k] A_{j-1}[n]
 * \f]
 *
 * donde \f$j\f$ es el nivel de descomposiciÃ³n y \f$A_0 = x[n]\f$ es la seÃ±al original.
 *
 * \dot
 * digraph mallat_algorithm {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Inicio Algoritmo\nde Mallat", fillcolor=lightgreen];
 *   INPUT [label="Entrada: x[n]\n(Nivel 0)", fillcolor=lightyellow];
 *   LEVEL_LOOP [label="Para cada nivel j", shape=diamond, fillcolor=lightcyan];
 *   FILTER_H0 [label="Filtrar con h0[n]\n(Paso Bajo)", fillcolor=lightblue];
 *   FILTER_H1 [label="Filtrar con h1[n]\n(Paso Alto)", fillcolor=lightpink];
 *   DECIMATE [label="Decimar por 2\n(↓2)", fillcolor=lightyellow];
 *   STORE_DETAIL [label="Almacenar\nDetalle D_j", fillcolor=lightpink];
 *   NEXT_LEVEL [label="Aproximación A_j\npara siguiente nivel", fillcolor=lightblue];
 *   CHECK_LEVELS [label="¿Más niveles?", shape=diamond, fillcolor=lightcyan];
 *   FINAL_APPROX [label="Almacenar\nAproximación Final", fillcolor=lightblue];
 *   END [label="Fin", fillcolor=lightgreen];
 *
 *   START -> INPUT -> LEVEL_LOOP;
 *   LEVEL_LOOP -> FILTER_H0 [label="Sí"];
 *   LEVEL_LOOP -> FINAL_APPROX [label="No"];
 *   INPUT -> FILTER_H1;
 *   FILTER_H0 -> DECIMATE;
 *   FILTER_H1 -> DECIMATE;
 *   DECIMATE -> STORE_DETAIL;
 *   DECIMATE -> NEXT_LEVEL;
 *   STORE_DETAIL -> CHECK_LEVELS;
 *   NEXT_LEVEL -> CHECK_LEVELS;
 *   CHECK_LEVELS -> LEVEL_LOOP [label="Sí"];
 *   CHECK_LEVELS -> FINAL_APPROX [label="No"];
 *   FINAL_APPROX -> END;
 * }
 * \enddot
 *
 * \section tipos_filtros_dwt Tipos de Filtros Soportados
 *
 * La librería soporta distintos tipos de filtros Wavelet, seleccionables en tiempo de compilación:
 *
 * - **LAGRANGE**: Filtros de media banda de Lagrange con parámetro M configurable
 * - **DB4**: Coeficientes FIR de la wavelet Daubechies 4
 * - **DB8**: Coeficientes FIR de la wavelet Daubechies 8
 *
 * La selección se realiza mediante directivas `#define` en `dwt.h`:
 * \code
 * #define LAGRANGE    // Para usar filtros Lagrange
 * // o
 * #define DB4         // Para usar Daubechies 4
 * // o
 * #define DB8         // Para usar Daubechies 8
 * \endcode
 *
 * \section arquitectura_dwt Arquitectura del Sistema DWT
 *
 * \dot
 * digraph dwt_multilevel {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   INPUT [label="x[n]", shape=plaintext, fillcolor=white];
 *
 *   subgraph cluster_level1 {
 *     label="Nivel 1";
 *     style=filled;
 *     color=lightgrey;
 *
 *     LP1 [label="LP₁", fillcolor=lightblue];
 *     HP1 [label="HP₁", fillcolor=lightpink];
 *     D1 [label="↓2", shape=circle, fillcolor=lightyellow];
 *     D2 [label="↓2", shape=circle, fillcolor=lightyellow];
 *     OUT1 [label="D₁[k]", shape=plaintext, fillcolor=white];
 *     A1 [label="A₁[k]", shape=plaintext, fillcolor=white];
 *   }
 *
 *   subgraph cluster_level2 {
 *     label="Nivel 2";
 *     style=filled;
 *     color=lightgrey;
 *
 *     LP2 [label="LP₂", fillcolor=lightblue];
 *     HP2 [label="HP₂", fillcolor=lightpink];
 *     D3 [label="↓2", shape=circle, fillcolor=lightyellow];
 *     D4 [label="↓2", shape=circle, fillcolor=lightyellow];
 *     OUT2 [label="D₂[k]", shape=plaintext, fillcolor=white];
 *     A2 [label="A₂[k]", shape=plaintext, fillcolor=white];
 *   }
 *
 *   INPUT -> LP1;
 *   INPUT -> HP1;
 *   LP1 -> D1 -> A1;
 *   HP1 -> D2 -> OUT1;
 *   A1 -> LP2;
 *   A1 -> HP2;
 *   LP2 -> D3 -> A2;
 *   HP2 -> D4 -> OUT2;
 * }
 * \enddot
 *
 * \section uso_dwt Uso del Módulo
 *
 * Para utilizar los servicios DWT, se implementa una API con las siguientes funciones:
 *
 * \code
 * #include "dwt.h"
 *
 * int main(void) {
 *     // Inicializar módulos
 *     Init_Fir();
 *     Init_DWT();
 *
 *     // Crear objeto DWT
 *     DWT_OBJECT dwt_obj;
 *     dwt_api.get_dwt(&dwt_obj);
 *
 *     // Procesar muestras
 *     for (int i = 0; i < 1000; i++) {
 *         float muestra = obtener_muestra();
 *         dwt_api.dwt(muestra, &dwt_obj);
 *
 *         // Leer salidas
 *         printf("Aproximación: %f\n", dwt_obj.yout[WAVELET_LEVELS]);
 *         for (int j = 0; j < WAVELET_LEVELS; j++) {
 *             printf("Detalle[%d]: %f\n", j, dwt_obj.yout[j]);
 *         }
 *     }
 *
 *     return 0;
 * }
 * \endcode
 *
 * \section funciones_dwt Descripción de Funciones
 *
 * \subsection init_dwt_func Init_DWT
 * Inicializa la estructura de punteros a funciones dwt_api.
 * Asigna los punteros a las funciones Get_DWT y Dwt.
 *
 * \subsection get_dwt_func Get_DWT
 * Inicializa completamente un objeto DWT_OBJECT.
 *
 * \dot
 * digraph get_dwt_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Get_DWT(pdwt)", fillcolor=lightgreen];
 *   INIT_FIR [label="Init_Fir()", fillcolor=lightyellow];
 *   GENERATE_COEFS [label="Generar coeficientes\nLP y HP según tipo", fillcolor=lightblue];
 *   CLEAR_BUFFERS [label="Limpiar buffers Z\ny salidas", fillcolor=lightblue];
 *   INIT_FILTERS [label="Inicializar objetos\nFIR para cada nivel", fillcolor=lightcyan];
 *   INIT_COUNTERS [label="Inicializar\ncontadores decimación", fillcolor=lightcyan];
 *   END [label="Fin", fillcolor=lightgreen];
 *
 *   START -> INIT_FIR -> GENERATE_COEFS -> CLEAR_BUFFERS -> INIT_FILTERS -> INIT_COUNTERS -> END;
 * }
 * \enddot
 *
 * El proceso de inicialización incluye:
 * 1. Inicialización del módulo FIR_FILTER
 * 2. Generación de coeficientes según el tipo de wavelet seleccionado
 * 3. Limpieza de todos los buffers de retardo
 * 4. Inicialización de objetos FIR_FILTER para cada nivel
 * 5. Configuración de contadores de decimación
 *
 * \param pdwt Puntero al objeto DWT_OBJECT a inicializar
 *
 * \subsection dwt_func Dwt
 * Ejecuta una iteración de la transformada DWT multinivel.
 *
 * \dot
 * digraph dwt_process_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Dwt(xin, dwt_object)", fillcolor=lightgreen];
 *   LEVEL_LOOP [label="Para cada nivel i", shape=diamond, fillcolor=lightyellow];
 *   CHECK_ENABLE [label="¿enabler[i] == 0?", shape=diamond, fillcolor=lightcyan];
 *   SELECT_INPUT [label="Seleccionar entrada:\ni==0 ? xin : yltemp[i-1]", fillcolor=lightblue];
 *   FILTER_HP [label="Filtrar con HP[i]", fillcolor=lightpink];
 *   FILTER_LP [label="Filtrar con LP[i]", fillcolor=lightblue];
 *   UPDATE_ENABLE [label="enabler[i] = (1<<i)-1", fillcolor=lightyellow];
 *   CHECK_DECIM [label="¿decimator[i] == 0?", shape=diamond, fillcolor=lightcyan];
 *   STORE_OUTPUTS [label="Almacenar salidas\nyout[i] = yhtemp", fillcolor=lightgreen];
 *   UPDATE_DECIM [label="decimator[i] = (1<<(i+1))-1", fillcolor=lightyellow];
 *   DECREMENT [label="enabler[i]--", fillcolor=lightyellow];
 *   NEXT_LEVEL [label="i++", fillcolor=lightyellow];
 *   END [label="Fin", fillcolor=lightgreen];
 *
 *   START -> LEVEL_LOOP;
 *   LEVEL_LOOP -> CHECK_ENABLE [label="i < WAVELET_LEVELS"];
 *   LEVEL_LOOP -> END [label="i >= WAVELET_LEVELS"];
 *   CHECK_ENABLE -> SELECT_INPUT [label="Sí"];
 *   CHECK_ENABLE -> DECREMENT [label="No"];
 *   SELECT_INPUT -> FILTER_HP;
 *   SELECT_INPUT -> FILTER_LP;
 *   FILTER_HP -> UPDATE_ENABLE;
 *   FILTER_LP -> UPDATE_ENABLE;
 *   UPDATE_ENABLE -> CHECK_DECIM;
 *   CHECK_DECIM -> STORE_OUTPUTS [label="Sí"];
 *   CHECK_DECIM -> DECREMENT [label="No"];
 *   STORE_OUTPUTS -> UPDATE_DECIM;
 *   UPDATE_DECIM -> DECREMENT;
 *   DECREMENT -> NEXT_LEVEL;
 *   NEXT_LEVEL -> LEVEL_LOOP;
 * }
 * \enddot
 *
 * El algoritmo implementa:
 * 1. Control de habilitación por niveles (enabler)
 * 2. Selección de entrada (señal original o aproximación del nivel anterior)
 * 3. Filtrado paralelo paso bajo y paso alto
 * 4. Decimación controlada (decimator)
 * 5. Almacenamiento de salidas cuando corresponde
 *
 * \param xin Muestra de entrada x(n)
 * \param dwt_object Puntero al objeto DWT_OBJECT
 *
 * \section objetos_dwt Estructuras de Datos
 *
 * \subsection dwt_object_struct DWT_OBJECT
 * Contiene todos los elementos necesarios para la descomposición wavelet:
 * - **lphp_z**: Arrays de buffers Z para filtros LP y HP por nivel
 * - **lp_coef, hp_coef**: Coeficientes de los filtros paso bajo y paso alto
 * - **yltemp, yhtemp**: Salidas temporales de filtros LP y HP
 * - **yout**: Vector de salidas (detalles + aproximación final)
 * - **filtrolp, filtrohp**: Objetos FIR_FILTER para cada nivel
 * - **decimator, enabler**: Contadores de control de decimación
 *
 * \section configuracion_dwt Configuración del Sistema
 *
 * Las siguientes constantes configuran el comportamiento:
 * - **LAGRANGE_M**: Parámetro M para filtros Lagrange (3 por defecto)
 * - **WAVELET_LEVELS**: Número de niveles de descomposición (2 por defecto)
 * - **BUFFER_SIZE**: Tamaño de buffers, depende del tipo de filtro seleccionado
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_dwt Historial de Cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 18/08/2025 | Dr. Carlos Romero | 1 | Primera edición |
 * | 28/08/2025 | Dr. Carlos Romero | 2 | Documentación Doxygen completa con Graphviz |
 *
 * \copyright  ZGR R&D AIE
 */

 #include "dwt.h"

/* Definición de variables */
#ifdef  DB4
/* Coeficientes Daubechies 4 */
static const float WAVELET_DB4_H0[4] = {
    0.48296291314469025f,   /* (1 + sqrt(3))/(4*sqrt(2)) */
    0.83651630373746899f,   /* (3 + sqrt(3))/(4*sqrt(2)) */
    0.22414386804185735f,   /* (3 - sqrt(3))/(4*sqrt(2)) */
    -0.12940952255092145f   /* (1 - sqrt(3))/(4*sqrt(2)) */
};
#endif /* DB4 */

#ifdef  DB8
/* Coeficientes Daubechies 8 */
static const float WAVELET_DB8_H0[8] = {
   5.441584220000000e-02,
   3.128715909000000e-01,
   6.756307363000000e-01,
   5.853546837000000e-01,
   -1.582910530000000e-02,
   -2.840155430000000e-01,
   4.724846000000000e-04,
   1.287474266000000e-01,
};
#endif /* DB8 */

/* Definición de Variables Globales */
DWT_API dwt_api;

/* Declaración de métodos */
void Init_DWT(void);
void Get_DWT(DWT_OBJECT *);
void Dwt(float,DWT_OBJECT *);

/* Definición de métodos */

void Init_DWT(void)
{
    dwt_api.get_dwt=Get_DWT;
    dwt_api.dwt=Dwt;
}

void Get_DWT(DWT_OBJECT * pdwt)
{
    unsigned int i,j;
    int signo;

    /* Inicializar FIR Filter API */
    Init_Fir();

#ifdef  LAGRANGE

    i=BUFFER_SIZE;

    if ((i&0x0001)==0)
    {
        signo=-1;
    }
    else
    {
        signo=1;
    }

    lagrange_halfband(LAGRANGE_M, pdwt->lp_coef);

    for (i=0;i<BUFFER_SIZE;i++)
    {
        pdwt->hp_coef[i]=signo*(pdwt->lp_coef[BUFFER_SIZE-1-i]);
        signo*=-1;
    }

#endif /* LAGRANGE */

#ifdef  DB4
    signo=-1;
    for(i=0;i<BUFFER_SIZE;i++)
    {
        pdwt->lp_coef[i]=WAVELET_DB4_H0[i];
        pdwt->hp_coef[i]=signo*WAVELET_DB4_H0[BUFFER_SIZE-1-i];
        signo*=-1;
    }
#endif /* DB4 */

#ifdef  DB8
    signo=-1;
    for(i=0;i<BUFFER_SIZE;i++)
    {
        pdwt->lp_coef[i]=WAVELET_DB8_H0[i];
        pdwt->hp_coef[i]=signo*WAVELET_DB8_H0[BUFFER_SIZE-1-i];
        signo*=-1;
    }
#endif /* DB8 */

    /* Limpia buffer de retrasos de los filtros LP y HP, e inicializa coeficientes de los filtros */
    for (i=0;i<WAVELET_LEVELS;i++)
    {
        for (j=0;j<BUFFER_SIZE;j++)
        {
            pdwt->lphp_z[i].lp_z[j]=0.0f;
            pdwt->lphp_z[i].hp_z[j]=0.0f;
        }
    }

    /* Limpia salidas filtros temporales */
    for(i=0;i<WAVELET_LEVELS;i++)
    {
        pdwt->yltemp[i]=0.0f;
        pdwt->yhtemp[i]=0.0f;
    }

    /* Inicializa las salidas */
    for (i=0;i<(WAVELET_LEVELS+1);i++ )
    {
        pdwt->yout[i]=0.0f;
    }

    /* Inicializa Objetos FIR FILTER */
    for (i=0;i<WAVELET_LEVELS;i++)
    {
        pdwt->filtrolp[i] = fir_api.get_fir(BUFFER_SIZE, pdwt->lp_coef, pdwt->lphp_z[i].lp_z);
        pdwt->filtrohp[i] = fir_api.get_fir(BUFFER_SIZE, pdwt->hp_coef, pdwt->lphp_z[i].hp_z);
    }

    for (i=0;i<WAVELET_LEVELS;i++)
    {
        pdwt->decimator[i]=0;
        pdwt->enabler[i]=0;
    }
}

void Dwt(float xin, DWT_OBJECT * dwt_object)
{
    unsigned int i;
    float xinput;
    float yhtemp,yltemp;


    for (i=0;i<WAVELET_LEVELS;i++)
    {
        if (dwt_object->enabler[i]==0)
        {
            if (i==0)
            {
                xinput=xin;
            }
            else
            {
                xinput=dwt_object->yltemp[i-1];
            }

            yhtemp = fir_api.fir_filter(xinput, &dwt_object->filtrohp[i]);
            yltemp = fir_api.fir_filter(xinput, &dwt_object->filtrolp[i]);


            dwt_object->enabler[i]=(1<<i);                   /* 2^i -1. El filtrado del nivel i se hace 1 muestra de cada 2^i de la señal
                                                                    de entrada */

            if (dwt_object->decimator[i]==0)
            {
                dwt_object->yhtemp[i]=yhtemp;
                dwt_object->yltemp[i]=yltemp;
                dwt_object->decimator[i]=(1<<(i+1));          /* 2^(i+1)-1. La salida de los filtros LP HP del nivel i salen al
                                                                    siguiente nivel cada 2^(i+1) muestras de la señal de entrada */
                dwt_object->yout[i]=yhtemp;
                if (i==(WAVELET_LEVELS-1))
                {
                    dwt_object->yout[i+1]=yltemp;
                }
            }
        }
        dwt_object->enabler[i]-=1;
        dwt_object->decimator[i]-=1;

    }
}
