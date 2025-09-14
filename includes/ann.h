#ifndef ANN_H_INCLUDED
#define ANN_H_INCLUDED

#include "nsdsp_math.h"

/* Definiciones propias del módulo */
#define LMAX 4  /* Número máximo de capas */

/* Enumerado para tipos de función de activación */
typedef enum {
    SIGMOID,
    TANH,
    RELU,
    LEAK,
    SOFT,
    STEP
} ANN_TRIGGER;

/* Objeto LAYER - Capa de la red neuronal */
typedef struct {
    MATRIZ *pesos;
    MATRIZ *bias;
} LAYER;

/* Objeto NET - Estructura de la red */
typedef struct {
    unsigned int levels;
    LAYER *layers[LMAX];
} NET;

/* Objeto ANN_SERVICE - Servicio de red neuronal */
typedef struct {
    ANN_TRIGGER trigger;
    MATRIZ x0;  /* Vector de entrada */
    NET net;    /* Estructura de la red */
    MATRIZ y0;  /* Vector de salida */
} ANN_SERVICE;

/* Declaración de la API */
typedef struct {
    ANN_SERVICE (*get_ann)(unsigned int levels, ANN_TRIGGER trigger, MATRIZ *pesos, MATRIZ *bias);
} ANN_API;

/* API pública del módulo */
extern ANN_API ann_api;

/* Función de inicialización */
extern void Init_ANN(void);

#endif /* ANN_H_INCLUDED */
