#ifndef FIR_FILTER_H_INCLUDED
#define FIR_FILTER_H_INCLUDED

#include    <stddef.h>

#define MAX_FIR_LENGTH  128

typedef struct
    {
        unsigned int ncoef;
        float * p_write;
        float * pcoef;
        float * pz;
    } FIR_FILTER_OBJECT;

typedef struct
    {
        FIR_FILTER_OBJECT (* get_fir)(unsigned int ncoef, float * pcoef, float * pz);
        float (* fir_filter) (float xin, FIR_FILTER_OBJECT * pfir );
    } FIR_FILTER_API;


// API pública del módulo fir_filter.c

extern void Init_Fir(void);
extern FIR_FILTER_API fir_api;


#endif // FIR_FILTER_H_INCLUDED
