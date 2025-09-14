/** \page ann REDES NEURONALES ARTIFICIALES
 * \brief Módulo de redes neuronales artificiales para la librería NSDSP
 *
 * Este módulo implementa redes neuronales artificiales feedforward con diferentes
 * funciones de activación. El módulo está diseñado para trabajar con memoria estática
 * y ser eficiente en sistemas embebidos. Utiliza las operaciones matriciales del
 * módulo nsdsp_math para los cálculos de propagación.
 *
 * \section uso_ann Uso del módulo
 *
 * Para utilizar este módulo:
 * 1. Inicializar con Init_ANN() (llamado automáticamente por Init_NSDSP())
 * 2. Crear las matrices de pesos y bias para cada capa
 * 3. Obtener un servicio ANN con ann_api.get_ann()
 * 4. Procesar datos a través de la red
 *
 * Ejemplo de uso:
 * \code
 * #include "ann.h"
 *
 * int main(void) {
 *     // Inicializar el módulo
 *     Init_ANN();
 *
 *     // Crear matrices de pesos y bias para 2 capas
 *     // Capa 1: 3 entradas, 4 neuronas
 *     float w1_data[12] = {...};  // 4x3
 *     float b1_data[4] = {...};   // 4x1
 *     MATRIZ pesos[2] = {{4, 3, w1_data}, {2, 4, w2_data}};
 *     MATRIZ bias[2] = {{4, 1, b1_data}, {2, 1, b2_data}};
 *
 *     // Crear red neuronal
 *     ANN_SERVICE red = ann_api.get_ann(2, RELU, pesos, bias);
 *
 *     return 0;
 * }
 * \endcode
 *
 * \section funciones_ann Descripción de funciones
 *
 * \subsection init_ann_func Init_ANN
 * Inicializa la estructura de punteros a funciones ann_api.
 * Esta función debe ser llamada antes de usar cualquier servicio del módulo.
 *
 * \subsection get_ann_func get_ann
 * Crea y configura un servicio de red neuronal artificial.
 *
 * \dot
 * digraph get_ann_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="get_ann(levels, trigger, pesos, bias)", fillcolor=lightgreen];
 *   VALIDATE [label="Validar\nlevels <= LMAX\npunteros no NULL", shape=diamond, fillcolor=lightyellow];
 *   INIT_SERVICE [label="Inicializar\nANN_SERVICE", fillcolor=lightblue];
 *   SET_TRIGGER [label="service.trigger = trigger", fillcolor=lightcyan];
 *   SET_LEVELS [label="service.net.levels = levels", fillcolor=lightcyan];
 *   LOOP [label="Para cada capa i", shape=diamond, fillcolor=lightpink];
 *   ASSIGN_LAYER [label="layers[i]->pesos = &pesos[i]\nlayers[i]->bias = &bias[i]", fillcolor=lightgreen];
 *   INIT_X0 [label="Inicializar x0\ncon dimensiones de entrada", fillcolor=lightyellow];
 *   INIT_Y0 [label="Inicializar y0\ncon dimensiones de salida", fillcolor=lightyellow];
 *   RETURN [label="return service", fillcolor=lightgreen];
 *   ERROR [label="service con\ncampos NULL", fillcolor=lightcoral];
 *
 *   START -> VALIDATE;
 *   VALIDATE -> INIT_SERVICE [label="OK"];
 *   VALIDATE -> ERROR [label="Error"];
 *   INIT_SERVICE -> SET_TRIGGER -> SET_LEVELS -> LOOP;
 *   LOOP -> ASSIGN_LAYER [label="i < levels"];
 *   LOOP -> INIT_X0 [label="i >= levels"];
 *   ASSIGN_LAYER -> LOOP;
 *   INIT_X0 -> INIT_Y0 -> RETURN;
 *   ERROR -> RETURN;
 * }
 * \enddot
 *
 * \param levels Número de capas de la red (debe ser <= LMAX)
 * \param trigger Tipo de función de activación a usar
 * \param pesos Array de matrices de pesos para cada capa
 * \param bias Array de matrices de bias para cada capa
 * \return Objeto ANN_SERVICE configurado
 *
 * \section arquitectura_ann Arquitectura de la Red
 *
 * \dot
 * digraph ann_architecture {
 *   rankdir=LR;
 *   node [shape=box, style=filled];
 *
 *   subgraph cluster_input {
 *     label="Entrada";
 *     x0 [label="x0\n(Vector entrada)", fillcolor=lightgreen];
 *   }
 *
 *   subgraph cluster_layer1 {
 *     label="Capa 1";
 *     w1 [label="W1\n(Pesos)", fillcolor=lightblue];
 *     b1 [label="b1\n(Bias)", fillcolor=lightblue];
 *     a1 [label="Activación", fillcolor=lightyellow];
 *   }
 *
 *   subgraph cluster_layer2 {
 *     label="Capa 2";
 *     w2 [label="W2\n(Pesos)", fillcolor=lightblue];
 *     b2 [label="b2\n(Bias)", fillcolor=lightblue];
 *     a2 [label="Activación", fillcolor=lightyellow];
 *   }
 *
 *   subgraph cluster_output {
 *     label="Salida";
 *     y0 [label="y0\n(Vector salida)", fillcolor=lightgreen];
 *   }
 *
 *   x0 -> w1 -> a1 -> w2 -> a2 -> y0;
 *   b1 -> a1;
 *   b2 -> a2;
 * }
 * \enddot
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_ann Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 15/09/2025 | Dr. Carlos Romero | 1 | Implementación inicial con get_ann |
 *
 * \copyright ZGR R&D AIE
 */

#include "ann.h"
#include <stddef.h>

/* Declaración de funciones */
void Init_ANN(void);
ANN_SERVICE get_ann(unsigned int levels, ANN_TRIGGER trigger, MATRIZ *pesos, MATRIZ *bias);

/* Definición de variables globales */
ANN_API ann_api;

/* Buffer estático para objetos LAYER */
static LAYER layer_buffer[LMAX];

/* Definición de funciones */

void Init_ANN(void)
{
    /* Inicializar punteros de la API */
    ann_api.get_ann = get_ann;
}

ANN_SERVICE get_ann(unsigned int levels, ANN_TRIGGER trigger, MATRIZ *pesos, MATRIZ *bias)
{
    ANN_SERVICE service;
    unsigned int i;
    unsigned int input_size, output_size;

    /* Inicializar estructura a valores por defecto */
    service.trigger = trigger;
    service.net.levels = 0;
    service.x0.filas = 0;
    service.x0.columnas = 0;
    service.x0.pmatriz = NULL;
    service.y0.filas = 0;
    service.y0.columnas = 0;
    service.y0.pmatriz = NULL;

    /* Inicializar punteros de layers a NULL */
    for (i = 0; i < LMAX; i++)
    {
        service.net.layers[i] = NULL;
    }

    /* Validar parámetros de entrada */
    if (levels > LMAX || levels == 0)
    {
        /* Número de capas inválido */
        return service;
    }

    if (pesos == NULL || bias == NULL)
    {
        /* Punteros NULL */
        return service;
    }

    /* Configurar el servicio */
    service.net.levels = levels;

    /* Asignar las capas */
    for (i = 0; i < levels; i++)
    {
        /* Verificar que las matrices existen */
        if (pesos[i].pmatriz == NULL || bias[i].pmatriz == NULL)
        {
            /* Error: matriz inválida */
            service.net.levels = 0;
            return service;
        }

        /* Usar el buffer estático de layers */
        layer_buffer[i].pesos = &pesos[i];
        layer_buffer[i].bias = &bias[i];
        service.net.layers[i] = &layer_buffer[i];
    }

    /* Inicializar x0 con las dimensiones de entrada */
    /* El tamaño de entrada es el número de columnas de la primera capa de pesos */
    if (levels > 0 && pesos[0].columnas > 0)
    {
        input_size = pesos[0].columnas;
        service.x0.filas = input_size;
        service.x0.columnas = 1;  /* Vector columna */
        service.x0.pmatriz = NULL; /* Se asignará externamente */
    }

    /* Inicializar y0 con las dimensiones de salida */
    /* El tamaño de salida es el número de filas de la última capa de pesos */
    if (levels > 0 && pesos[levels-1].filas > 0)
    {
        output_size = pesos[levels-1].filas;
        service.y0.filas = output_size;
        service.y0.columnas = 1;  /* Vector columna */
        service.y0.pmatriz = NULL; /* Se asignará externamente */
    }

    return service;
}
