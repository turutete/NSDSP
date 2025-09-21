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
 * 4. Configurar vectores de entrada (x0) y salida (y0)
 * 5. Procesar datos con ann_api.iterate()
 *
 * Ejemplo de uso:
 * \code
 * #include "ann.h"
 *
 * int main(void) {
 *     // Inicializar el módulo
 *     Init_ANN();
 *     nsdsp_math_init();
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
 *     // Configurar entrada y salida
 *     float input[3] = {1.0f, 0.5f, -0.3f};
 *     float output[2];
 *     red.x0.pmatriz = input;
 *     red.y0.pmatriz = output;
 *
 *     // Procesar (forward pass)
 *     int result = ann_api.iterate(&red);
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
 * Asigna los punteros a las funciones get_ann e iterate_ann.
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
 * \subsection iterate_ann_func iterate_ann
 * Realiza una iteración completa de forward pass en la red neuronal.
 *
 * Implementa la fórmula:
 * \f[
 * y_0 = T(M_L \cdot (T(M_{L-1} \cdot (T(M_{L-2} \cdot (\ldots T(M_1 \cdot x_0 + b_1) + b_2) \ldots) + b_{L-1})) + b_L)
 * \f]
 *
 * donde T() es la función de activación (trigger) definida para la red.
 *
 * \dot
 * digraph iterate_flow {
 *   rankdir=TB;
 *   node [shape=box, style=filled];
 *
 *   START [label="iterate_ann(service)", fillcolor=lightgreen];
 *   VALIDATE [label="Validar service\ny punteros", shape=diamond, fillcolor=lightyellow];
 *   INIT [label="input = x0\nCrear buffers temporales", fillcolor=lightblue];
 *   LOOP [label="Para cada capa i", shape=diamond, fillcolor=lightcyan];
 *   MULT [label="temp = Mi * input + bi", fillcolor=lightpink];
 *   TRIGGER [label="output = T(temp)", fillcolor=lightyellow];
 *   UPDATE [label="input = output", fillcolor=lightgreen];
 *   COPY [label="Copiar resultado\na y0", fillcolor=lightblue];
 *   RETURN_OK [label="return ANN_OK", fillcolor=lightgreen];
 *   RETURN_ERROR [label="return ANN_KO", fillcolor=lightcoral];
 *
 *   START -> VALIDATE;
 *   VALIDATE -> INIT [label="OK"];
 *   VALIDATE -> RETURN_ERROR [label="Error"];
 *   INIT -> LOOP;
 *   LOOP -> MULT [label="i < levels"];
 *   LOOP -> COPY [label="i >= levels"];
 *   MULT -> TRIGGER -> UPDATE -> LOOP;
 *   COPY -> RETURN_OK;
 * }
 * \enddot
 *
 * \param service Puntero al servicio ANN a procesar
 * \return ANN_OK (0) si el procesamiento fue exitoso, ANN_KO (-1) si hubo error
 *
 * \subsection trigger_ann_func trigger_ann
 * Aplica la función de activación a un vector de entrada.
 *
 * Funciones de activación soportadas:
 * - **SIGMOID**: \f$ f(x) = \frac{1}{1 + e^{-x}} \f$
 * - **TANH**: \f$ f(x) = \tanh(x) \f$
 * - **RELU**: \f$ f(x) = \max(0, x) \f$
 * - **LEAK**: \f$ f(x) = \max(x, \alpha \cdot x) \f$ donde \f$\alpha = 0.01\f$
 * - **SOFT**: \f$ f(x) = \log(1 + e^x) \f$ (Softplus)
 * - **STEP**: \f$ f(x) = \begin{cases} 1 & \text{si } x \geq 0 \\ 0 & \text{si } x < 0 \end{cases} \f$
 *
 * \param input Matriz de entrada (Nx1)
 * \param output Matriz de salida (Nx1)
 * \param trigger Tipo de función de activación
 * \return ANN_OK si éxito, ANN_KO si error
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
 *     a1 [label="T()", fillcolor=lightyellow];
 *   }
 *
 *   subgraph cluster_layer2 {
 *     label="Capa 2";
 *     w2 [label="W2\n(Pesos)", fillcolor=lightblue];
 *     b2 [label="b2\n(Bias)", fillcolor=lightblue];
 *     a2 [label="T()", fillcolor=lightyellow];
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
 * | 15/09/2025 | Dr. Carlos Romero | 2 | Añadidas funciones iterate_ann y trigger_ann |
 * | 16/09/2025 | Dr. Carlos Romero | 3 | Implementación completa de funciones trigger |
 *
 * \copyright ZGR R&D AIE
 */

#include "ann.h"
#include "nsdsp_math.h"
#include <stddef.h>
#include <string.h>
#include <math.h>

/* Declaración de funciones */
void Init_ANN(void);
ANN_SERVICE get_ann(unsigned int levels, ANN_TRIGGER trigger, MATRIZ *pesos, MATRIZ *bias);
int iterate_ann(ANN_SERVICE *service);
int trigger_ann(MATRIZ *input, MATRIZ *output, ANN_TRIGGER trigger);

/* Definición de variables globales */
ANN_API ann_api;

/* Buffer estático para objetos LAYER */
static LAYER layer_buffer[LMAX];

/* Buffers estáticos para cálculos intermedios */
#define MAX_NEURONS 100  /* Máximo número de neuronas por capa */
static float temp_buffer1[MAX_NEURONS];
static float temp_buffer2[MAX_NEURONS];

/* Definición de funciones */

void Init_ANN(void)
{
    /* Inicializar punteros de la API */
    ann_api.get_ann = get_ann;
    ann_api.iterate = iterate_ann;
    ann_api.trigger = trigger_ann;
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

int iterate_ann(ANN_SERVICE *service)
{
    unsigned int j;
    unsigned int current_level;
    MATRIZ input, output, temp;
    float *current_input, *current_output, *swap_ptr;
    int result;
    unsigned int num_elements;

    /* Validar parámetros */
    if (service == NULL)
    {
        return ANN_KO;
    }

    if (service->net.levels == 0 || service->net.levels > LMAX)
    {
        return ANN_KO;
    }

    if (service->x0.pmatriz == NULL || service->y0.pmatriz == NULL)
    {
        return ANN_KO;
    }

    /* Inicializar punteros a buffers temporales */
    current_input = temp_buffer1;
    current_output = temp_buffer2;

    /* Copiar entrada inicial x0 al buffer de trabajo */
    num_elements = service->x0.filas;
    if (num_elements > MAX_NEURONS)
    {
        return ANN_KO;
    }

    for (j = 0; j < num_elements; j++)
    {
        current_input[j] = service->x0.pmatriz[j];
    }

    /* Configurar matriz de entrada inicial */
    input.filas = service->x0.filas;
    input.columnas = 1;
    input.pmatriz = current_input;

    /* Procesar cada capa de la red */
    for (current_level = 0; current_level < service->net.levels; current_level++)
    {
        /* Verificar que la capa existe */
        if (service->net.layers[current_level] == NULL)
        {
            return ANN_KO;
        }

        /* Obtener pesos y bias de la capa actual */
        MATRIZ *pesos = service->net.layers[current_level]->pesos;
        MATRIZ *bias = service->net.layers[current_level]->bias;

        /* Verificar punteros */
        if (pesos == NULL || bias == NULL)
        {
            return ANN_KO;
        }

        /* Configurar matriz temporal para el resultado */
        temp.filas = pesos->filas;
        temp.columnas = 1;
        temp.pmatriz = current_output;

        /* Verificar que no excedemos el buffer */
        if (temp.filas > MAX_NEURONS)
        {
            return ANN_KO;
        }

        /* Calcular M*x + b */
        /* Primero: M*x */
        result = nsdsp_math_api.product(pesos, &input, &temp);
        if (result != NSDSP_MATH_OK)
        {
            return ANN_KO;
        }

        /* Segundo: resultado + b (suma elemento a elemento) */
        for (j = 0; j < temp.filas; j++)
        {
            temp.pmatriz[j] += bias->pmatriz[j];
        }

        /* Configurar matriz de salida para la función de activación */
        output.filas = temp.filas;
        output.columnas = 1;
        output.pmatriz = current_output;

        /* Aplicar función de activación T(resultado) */
        result = trigger_ann(&temp, &output, service->trigger);
        if (result != ANN_OK)
        {
            return ANN_KO;
        }

        /* Preparar para la siguiente iteración */
        /* La salida actual se convierte en la entrada de la siguiente capa */
        input.filas = output.filas;
        input.columnas = 1;

        /* Intercambiar buffers para la siguiente iteración */
        swap_ptr = current_input;
        current_input = current_output;
        current_output = swap_ptr;
        input.pmatriz = current_input;
    }

    /* Copiar resultado final a y0 */
    num_elements = service->y0.filas;
    for (j = 0; j < num_elements; j++)
    {
        service->y0.pmatriz[j] = current_input[j];
    }

    return ANN_OK;
}

int trigger_ann(MATRIZ *input, MATRIZ *output, ANN_TRIGGER trigger)
{
    unsigned int i;
    unsigned int num_elements;
    float value;
    float exp_value;
    float alpha_x;

    /* Validar parámetros */
    if (input == NULL || output == NULL)
    {
        return ANN_KO;
    }

    if (input->pmatriz == NULL || output->pmatriz == NULL)
    {
        return ANN_KO;
    }

    /* Verificar que las dimensiones son compatibles */
    if (input->filas != output->filas || input->columnas != 1 || output->columnas != 1)
    {
        return ANN_KO;
    }

    num_elements = input->filas;

    /* Aplicar función de activación según el tipo */
    switch (trigger)
    {
        case SIGMOID:
            /* f(x) = 1 / (1 + exp(-x)) */
            for (i = 0; i < num_elements; i++)
            {
                value = input->pmatriz[i];
                exp_value = expf(-value);
                output->pmatriz[i] = 1.0f / (1.0f + exp_value);
            }
            break;

        case TANH:
            /* f(x) = tanh(x) */
            for (i = 0; i < num_elements; i++)
            {
                value = input->pmatriz[i];
                output->pmatriz[i] = tanhf(value);
            }
            break;

        case RELU:
            /* f(x) = max(0, x) */
            for (i = 0; i < num_elements; i++)
            {
                value = input->pmatriz[i];
                output->pmatriz[i] = (value > 0.0f) ? value : 0.0f;
            }
            break;

        case LEAK:
            /* f(x) = max(x, alpha*x) donde alpha = 0.01 */
            for (i = 0; i < num_elements; i++)
            {
                value = input->pmatriz[i];
                alpha_x = ALPHA * value;
                output->pmatriz[i] = (value > alpha_x) ? value : alpha_x;
            }
            break;

        case SOFT:
            /* f(x) = log(1 + exp(x)) - Softplus */
            for (i = 0; i < num_elements; i++)
            {
                value = input->pmatriz[i];
                /* Para evitar overflow, usar la identidad:
                 * log(1 + exp(x)) = log(1 + exp(x)) si x < 0
                 *                  = x + log(1 + exp(-x)) si x >= 0
                 */
                if (value < 0.0f)
                {
                    exp_value = expf(value);
                    output->pmatriz[i] = logf(1.0f + exp_value);
                }
                else
                {
                    exp_value = expf(-value);
                    output->pmatriz[i] = value + logf(1.0f + exp_value);
                }
            }
            break;

        case STEP:
            /* f(x) = 1 si x >= 0, 0 si x < 0 */
            for (i = 0; i < num_elements; i++)
            {
                value = input->pmatriz[i];
                output->pmatriz[i] = (value >= 0.0f) ? 1.0f : 0.0f;
            }
            break;

        default:
            /* Tipo de activación no reconocido */
            return ANN_KO;
    }

    return ANN_OK;
}
