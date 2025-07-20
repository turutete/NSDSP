#ifndef NSDSP_H_INCLUDED
#define NSDSP_H_INCLUDED

// Inclusión de recursos disponibles
#include "rt_momentos.h"
#include "nsdsp_statistical.h"

// Función de inicialización principal
extern void Init_NSDSP(void);

#ifdef DEBUG
// Declaración de funciones de test
extern int Run_All_RT_Momentos_Tests(void);
#endif

#endif // NSDSP_H_INCLUDED
