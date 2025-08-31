#ifndef LAGRANGE_HALFBAND_H_INCLUDED
#define LAGRANGE_HALFBAND_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Definiciones propias de la librería */
#define LAGRANGE_OK     0
#define LAGRANGE_KO     -1

/* Declaración de funciones públicas */
extern int lagrange_halfband(int m, float *h0);

#ifdef DEBUG
/* Declaración de funciones de test */
extern int Run_All_Lagrange_Tests(void);
#endif

#endif /* LAGRANGE_HALFBAND_H_INCLUDED */