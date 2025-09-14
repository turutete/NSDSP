#ifndef NSDSP_MATH_H_INCLUDED
#define NSDSP_MATH_H_INCLUDED

/* Definiciones propias del módulo */
#define NSDSP_MATH_OK  0
#define NSDSP_MATH_KO  -1

/* Declaración de objetos */
typedef struct
{
    unsigned int filas;
    unsigned int columnas;
    float * pmatriz;
} MATRIZ;

/* Declaración de la API */
typedef struct
{
    int (* product)(MATRIZ * PM1, MATRIZ * PM2, MATRIZ * PM3);
    int (* suma)(MATRIZ * PM1, MATRIZ * PM2, MATRIZ * PM3, int signo);
} NSDSP_MATH_API;

/* API pública del módulo */
extern NSDSP_MATH_API nsdsp_math_api;

/* Función de inicialización */
extern void nsdsp_math_init(void);

#endif /* NSDSP_MATH_H_INCLUDED */
