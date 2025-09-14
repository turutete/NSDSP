#ifndef NSDSP_H_INCLUDED
#define NSDSP_H_INCLUDED

// Inclusión de recursos disponibles
#include "rt_momentos.h"
#include "nsdsp_statistical.h"
#include "lagrange_halfband.h"
#include "fir_filter.h"
#include "dwt.h"
#include "nsdsp_math.h"
#include "ann.h"

// Función de inicialización principal
extern void Init_NSDSP(void);

#ifdef DEBUG
// Inclusión de headers de test
#include "test_rt_momentos.h"
#include "test_lagrange_halfband.h"
#include "test_fir_filter.h"
#include "test_dwt.h"
#include "test_nsdsp_math.h"
#include "test_ann.h"
#endif

#endif // NSDSP_H_INCLUDED
