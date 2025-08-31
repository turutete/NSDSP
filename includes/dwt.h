#ifndef DWT_H_INCLUDED
#define DWT_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "lagrange_halfband.h"
#include "fir_filter.h"

/* Definiciones propias del módulo */
#define LAGRANGE_M      3           /* Número de coeficientes distintos entre sí, no nulos, y distintos al central 1/2. El orden
                                    del filtro es 4*M-2 */
#define WAVELET_LEVELS  2           /* Número de niveles de descomposición Wavelet. El número de salidas es LEVEL+1 */

#if (WAVELET_LEVELS>15)             /* Máximo numero permitido para poder usar UNSIGNED INT */
#undef WAVELET_LEVELS
#define WAVELET_LEVELS  15
#endif


#if (LAGRANGE_M>10)
#undef  LAGRANGE_M
#define LAGRANGE_M      10          /* Valor máximo 39 coeficientes por filtro FIR */
#endif

#if (WAVELET_LEVELS>8)
#undef  WAVELET_LEVELS
#define WAVELET_LEVELS  8           /* Valor máximo de niveles de descomposición */
#endif



// Indicar en la línea inferior el tipo de Wavelet: LAGRANGE, DB4, DB8
#define LAGRANGE

#ifdef LAGRANGE
#define BUFFER_SIZE     (4*LAGRANGE_M-1)
#endif // LAGRANGE

#ifdef DB4
#define BUFFER_SIZE     4
#endif // DB4

#ifdef DB8
#define BUFFER_SIZE     8
#endif // DB8



// Declaración de objetos

typedef struct
{
    float lp_z[BUFFER_SIZE];
    float hp_z[BUFFER_SIZE];
} LPHP_Z;


typedef struct
{
    LPHP_Z lphp_z[WAVELET_LEVELS];
    float lp_coef[BUFFER_SIZE];
    float hp_coef[BUFFER_SIZE];
    float yltemp[WAVELET_LEVELS];
    float yhtemp[WAVELET_LEVELS];
    float yout[WAVELET_LEVELS+1];
    FIR_FILTER_OBJECT filtrolp[WAVELET_LEVELS];
    FIR_FILTER_OBJECT filtrohp[WAVELET_LEVELS];
    unsigned int decimator[WAVELET_LEVELS];         // decimator=0 se activa la salida de los filtros LP y HP
    unsigned int enabler[WAVELET_LEVELS];           // enabler=0 se activa el filtrado LP y HP del nivel
} DWT_OBJECT;


typedef struct
{
    void (* get_dwt)(DWT_OBJECT *);
    void (* dwt)(float xin,DWT_OBJECT * dwt_object);

} DWT_API;


// Métodos Públicos
extern void Init_DWT (void);
extern DWT_API dwt_api;

#endif // DWT_H_INCLUDED
