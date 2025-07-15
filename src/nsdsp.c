/** \page page1 NSDSP.c
 *
 * \brief Implementación principal de la librería NSDSP
 *
 * Librería de Procesamiento Digital de Señales No Supervisado.
 * Este es el módulo principal que inicializa todos los recursos disponibles.
 *
 * \section arquitectura Arquitectura general
 *
 * \dot
 * digraph NSDSP {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   APP [label="Aplicación", fillcolor=lightgreen];
 *   NSDSP [label="NSDSP.h", fillcolor=lightblue];
 *   STAT [label="nsdsp_statistical.h", fillcolor=lightyellow];
 *   RT [label="rt_momentos.h", fillcolor=lightyellow];
 *
 *   APP -> NSDSP [label="include"];
 *   NSDSP -> STAT [label="include"];
 *   NSDSP -> RT [label="include"];
 * }
 * \enddot
 *
 * Cada recurso sigue el patrón estándar:
 * - Init_[recurso]() - Inicialización del recurso
 * - Suscribe_[recurso]() - Obtener una instancia del recurso
 * - Compute_[recurso]() - Procesar datos con el recurso
 * - Unsuscribe_[recurso]() - Liberar la instancia del recurso
 *
 * \section recursos Recursos disponibles
 * - RT_Momentos: Cálculo de momentos estadísticos en tiempo real
 *
 * \section uso Uso básico
 * La aplicación debe:
 * 1. Incluir NSDSP.h
 * 2. Llamar a Init_NSDSP() al inicio
 * 3. Usar los recursos según necesite a través de pse
 * 4. Acceder a los resultados a través de nsdsp_statistical_objects[]
 *
 * \section implementacion Detalles de implementación
 *
 * La función Init_NSDSP() realiza:
 *
 * \dot
 * digraph Implementation {
 *   rankdir=LR;
 *   node [shape=box];
 *
 *   INIT [label="Init_NSDSP()", style=filled, fillcolor=lightgreen];
 *   RT_INIT [label="Init_RT_Momentos()", style=filled, fillcolor=lightyellow];
 *   STAT_INIT [label="Inicializar\nnsdsp_statistical_objects[]", style=filled, fillcolor=lightyellow];
 *
 *   INIT -> RT_INIT;
 *   INIT -> STAT_INIT;
 * }
 * \enddot
 *
 * \author Dr. Carlos Romero
 *
 * \section historial Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 12/07/2025 | Dr. Carlos Romero | 1 | Primera versión |
 *
 * \copyright ZGR R&D AIE
 */

#include "nsdsp.h"

// Vista simplificada de los datos estadísticos
statistical_object nsdsp_statistical_objects[MAX_RT_MOMENTOS];

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
