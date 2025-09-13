/** \page nsdsp NSDSP
 * \brief Módulo principal de la librería NSDSP
 *
 * Librería de Procesamiento Digital de Señales No Supervisado (Non-Stationary Digital Signal Processing).
 * Este módulo es el punto de entrada principal que inicializa todos los recursos disponibles
 * de la librería y proporciona la interfaz unificada para acceder a todos los servicios.
 *
 * \section uso_nsdsp Uso del módulo
 *
 * Para utilizar la librería NSDSP:
 * 1. Incluir el archivo de cabecera NSDSP.h en la aplicación
 * 2. Llamar a Init_NSDSP() al inicio del programa
 * 3. Utilizar los recursos disponibles a través de las estructuras de API:
 *    - pse para servicios de momentos estadísticos
 *    - dwt_api para servicios de transformada wavelet
 *    - fir_api para servicios de filtrado FIR
 * 4. Acceder a los resultados mediante nsdsp_statistical_objects[]
 *
 * Ejemplo de uso:
 * \code
 * #include "nsdsp.h"
 *
 * int main(void) {
 *     Init_NSDSP();
 *
 *     // Usar servicios de momentos
 *     RT_MOMENTOS_SERVICE service = pse.suscribe_rt_momentos();
 *
 *     // Usar servicios de transformada wavelet
 *     DWT_OBJECT dwt_obj;
 *     dwt_api.get_dwt(&dwt_obj);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \section funciones_nsdsp Descripción de funciones
 *
 * \subsection init_nsdsp Init_NSDSP
 * Función principal de inicialización de la librería. Esta función:
 * - Llama a Init_RT_Momentos() para inicializar el módulo de cálculo de momentos
 * - Llama a Init_Fir() para inicializar el módulo de filtrado FIR
 * - Llama a Init_DWT() para inicializar el módulo de transformada wavelet
 *
 * - Prepara todos los recursos para su uso
 *
 * \dot
 * digraph Init_NSDSP_Flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Init_NSDSP()", fillcolor=lightgreen];
 *   INIT_RT [label="Init_RT_Momentos()", fillcolor=lightyellow];
 *   INIT_FIR [label="Init_Fir()", fillcolor=lightyellow];
 *   INIT_DWT [label="Init_DWT()", fillcolor=lightyellow];
 *   END [label="Fin", fillcolor=lightgreen];
 *
 *   START -> INIT_RT -> INIT_FIR -> INIT_DWT -> END;
 * }
 * \enddot
 *
 * \section arquitectura_nsdsp Arquitectura del sistema
 *
 * La librería NSDSP sigue una arquitectura modular:
 *
 * \dot
 * digraph NSDSP_Architecture {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   APP [label="Aplicación Usuario", fillcolor=lightgreen];
 *   NSDSP [label="NSDSP.h/NSDSP.c", fillcolor=lightblue];
 *   STAT [label="nsdsp_statistical.h", fillcolor=lightyellow];
 *   RT [label="rt_momentos.h/rt_momentos.c", fillcolor=lightyellow];
 *   LAG [label="lagrange_halfband.h/lagrange_halfband.c", fillcolor=lightyellow];
 *   FIR [label="fir_filter.h/fir_filter.c", fillcolor=lightyellow];
 *   DWT [label="dwt.h/dwt.c", fillcolor=lightyellow];
 *
 *   subgraph cluster_lib {
 *     label="Librería NSDSP";
 *     style=filled;
 *     color=lightgrey;
 *     NSDSP; STAT; RT; LAG; FIR; DWT;
 *   }
 *
 *   APP -> NSDSP [label="include/llamadas"];
 *   NSDSP -> STAT [label="include"];
 *   NSDSP -> RT [label="include"];
 *   NSDSP -> LAG [label="include"];
 *   NSDSP -> FIR [label="include"];
 *   NSDSP -> DWT [label="include"];
 *   RT -> STAT [label="actualiza"];
 *   DWT -> LAG [label="usa"];
 *   DWT -> FIR [label="usa"];
 * }
 * \enddot
 *
 * *** Subpáginas ***
 *
 * \subpage rt_momentos
 * \subpage lagrange_halfband
 * \subpage fir_filter
 * \subpage wavelet_transform
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_nsdsp Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 12/07/2025 | Dr. Carlos Romero | 1 | Primera versión |
 * | 03/08/2025 | Dr. Carlos Romero | 2 | Actualización documentación Doxygen según estándar |
 * | 14/08/2025 | Dr. Carlos Romero | 3 | Integración módulo de interpolación/decimación |
 * | 14/08/2025 | Dr. Carlos Romero | 4 | Cambio a módulo de descomposición wavelet únicamente |
 * | 28/08/2025 | Dr. Carlos Romero | 5 | Integración de DWT y FIR_FILTER, eliminación wavelet_decim |
 * | 13/09/2025 | Dr. Carlos Romero | 6 | Se añade inicialización de la librería nsdsp_math |
 *
 * \copyright ZGR R&D AIE
 */

#include "nsdsp.h"

/* Declaración de funciones */
void Init_NSDSP(void);

/* Definición de funciones */

void Init_NSDSP(void)
{
    /* Inicializar el módulo RT_Momentos */
    Init_RT_Momentos();

    /* Inicializar el módulo FIR Filter */
    Init_Fir();

    /* Inicializar el módulo DWT */
    Init_DWT();

    /* Inicializar el módulo NSDSP Math */
    nsdsp_math_init();
}
