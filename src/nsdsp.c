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
 * 3. Utilizar los recursos disponibles a través de la estructura pse
 * 4. Acceder a los resultados mediante nsdsp_statistical_objects[]
 *
 * Ejemplo de uso:
 * \code
 * #include "nsdsp.h"
 *
 * int main(void) {
 *     Init_NSDSP();
 *     RT_MOMENTOS_SERVICE service = pse.suscribe_rt_momentos();
 *     // Usar el servicio...
 *     return 0;
 * }
 * \endcode
 *
 * \section funciones_nsdsp Descripción de funciones
 *
 * \subsection init_nsdsp Init_NSDSP
 * Función principal de inicialización de la librería. Esta función:
 * - Llama a Init_RT_Momentos() para inicializar el módulo de cálculo de momentos
 * - Inicializa el array nsdsp_statistical_objects[] con valores por defecto (0.0f)
 * - Prepara todos los recursos para su uso
 *
 * \dot
 * digraph Init_NSDSP_Flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="Init_NSDSP()", fillcolor=lightgreen];
 *   INIT_RT [label="Init_RT_Momentos()", fillcolor=lightyellow];
 *   INIT_STAT [label="Inicializar\nnsdsp_statistical_objects[]", fillcolor=lightyellow];
 *   END [label="Fin", fillcolor=lightgreen];
 *
 *   START -> INIT_RT -> INIT_STAT -> END;
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
 *
 *   subgraph cluster_lib {
 *     label="Librería NSDSP";
 *     style=filled;
 *     color=lightgrey;
 *     NSDSP; STAT; RT;
 *   }
 *
 *   APP -> NSDSP [label="include/llamadas"];
 *   NSDSP -> STAT [label="include"];
 *   NSDSP -> RT [label="include"];
 *   RT -> STAT [label="actualiza"];
 * }
 * \enddot
 *
 * *** Subpáginas ***
 *
 * \subpage rt_momentos
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_nsdsp Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 12/07/2025 | Dr. Carlos Romero | 1 | Primera versión |
 * | 03/08/2025 | Dr. Carlos Romero | 2 | Actualización documentación Doxygen según estándar |
 *
 * \copyright ZGR R&D AIE
 */

#include "nsdsp.h"

void Init_NSDSP(void);


void Init_NSDSP(void)
{
    int i;

    // Inicializar el módulo RT_Momentos
    Init_RT_Momentos();

    // Inicializar la vista de datos estadísticos
    for (i = 0; i < MAX_RT_MOMENTOS; i++)
    {
        nsdsp_statistical_objects[i].media = 0.0f;
        nsdsp_statistical_objects[i].varianza = 0.0f;
        nsdsp_statistical_objects[i].asimetria = 0.0f;
        nsdsp_statistical_objects[i].curtosis = 0.0f;
    }
}
