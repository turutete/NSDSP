#ifndef NSDSP_STATISTICAL_H_INCLUDED
#define NSDSP_STATISTICAL_H_INCLUDED

typedef struct
{
    float media;                // Valor promedio (1º momento)
    float varianza;             // Valor de la varianza (2º momento)
    float asimetria;            // Valor de la asimetría (3º momento)
    float curtosis;             // Valor de la curtosis (4º momento)
} statistical_object;

// Vista simplificada de los objetos RT_MOMENTOS
// Permite acceso directo a los valores calculados
extern statistical_object nsdsp_statistical_objects[];

#endif // NSDSP_STATISTICAL_H_INCLUDED
