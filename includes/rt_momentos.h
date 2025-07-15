#ifndef RT_MOMENTOS_H_INCLUDED
#define RT_MOMENTOS_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef DEBUG

#endif // DEBUG

#define MAX_RT_MOMENTOS 4       // Número máximo de servicios concurrentes
#define N_MA            64      // Número de muestras de la media móvil

// Definiciones propias de la librería

#define RT_MOMENTOS_OK  0
#define RT_MOMENTOS_KO  -1
#define NONE            -1
#define INV_N_MA        (1.0f/(float)N_MA)  // Corregido para evitar división entera

typedef enum
    {
      FREE,
      ASIGNED
    } estado;

typedef struct
{
    unsigned int index_w;                   // índice de escritura
    float buffer_z[N_MA];                   // Definición de tipo buffer para los buffers Z de los filtros
} BUFFER_Z;

typedef int RT_MOMENTOS_SERVICE;


 typedef struct
 {
    BUFFER_Z mu_z;                          // Buffer Z MA mu
    BUFFER_Z sigma2_z;                      // Buffer Z MA sigma2
    BUFFER_Z a_z;                           // Buffer Z MA A
    BUFFER_Z c_z;                           // Buffer Z MA C
 } BUFFER_FIR;

typedef struct
{
    estado status;                          // Estado del servicio (FREE, ASIGNED)
    float mu;                               // Valor medio
    float var2;                             // Varianza
    float A;                                // Asimetría
    float C;                                // Curtosis
    BUFFER_FIR z_buffers;                   // Buffers de filtros FIR
 } RT_MOMENTOS;



typedef struct
{
    RT_MOMENTOS_SERVICE (* suscribe_rt_momentos)(void);
    int (* unsuscribe_rt_momentos)(RT_MOMENTOS_SERVICE);
    int (* compute_rt_momentos)(RT_MOMENTOS_SERVICE,float);
} SSP;


extern SSP pse;                                 // pse es la estructura de métodos de la librería
extern void Init_RT_Momentos(void);             // Declaración del método de inicialización de la librería
extern RT_MOMENTOS servicios_rt_momentos[MAX_RT_MOMENTOS];  // Array de servicios para acceso externo

#endif // RT_MOMENTOS_H_INCLUDED