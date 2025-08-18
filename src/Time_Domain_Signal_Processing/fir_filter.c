/** \page   fir_filter   FIR Filter
 *
 *
 *
 *  Este módulo de SW implementa un método general para realizar el filtrado de una nueva muestra de una señal
 *  de entrada xn mediante un filtro FIR dado también como parámetro de entrada.
 *
 *  El módulo implemeta una API con 2 funciones:
 *
 * Get_Fir: Es un método que crea e inicializa un servicio de filtrado FIR.
 *
 * ***Prototipo: *** FIR_FILTER_OBJECT Get_Fir(unsigned int ncoef, float pcoef, float pz)
 *
 * \param[in]   ncoef es el número de coeficientes del filtr FIR
 * \param[in]   pcoef es un puntero a un vector de coeficientes del filtro FIR.Debe tener ncoef elementos.
 * \param[in]    pz es un puntero al buffer de retrasos del filtro fir. Debe tener longitud ncoef
 * \return      La función devuelve un objeto FIR_FILTER_OBJECT con el puntero de escritura inicializado, y el
 *              buffer Z del filtro limpiado.
 *
 * ***Prototipo:*** float y=fir_filter(float xn,FIR_FILTER_OBJECT * pfiltro)
 *
 * \param[in] 	xn	es la muestra x(n) de la secuencia de entrada
 * \param[in]   pfiltro es un puntero a un objeto FIR_OBJECT
 * \return		Retorna el FLOAT32 y, que es el resultado del filtrado de la muestra xn
 *
 *
 * *** FIR_FILTER_OBJECT ***
 *
 *  Este objeto está definido en *_fir_filter.h_*.
 *
 *  El objeto FIR_FILTER_OBJECT contiene:
 *
 * * ncoeficientes: Es un UINT16 que define el número de coeficientes del filtro
 * * p_write: Puntero de escritura en el buffer circular de retrasos Z del filtro
 * * pcoeficientes: Es un puntero al buffer FLOAT32 con los coeficientes del filtro
 * * pzbuffer: Es un puntero al buffer circular de retrasos Z del filtro
 *
 * *** Manejo de Excepciones ***
 *
 * Cualquier excepción en la ejecución de la operación de filtrado resultará en una salida y=0.
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_rt Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 18/08/2025 | Dr. Carlos Romero | 1 | Primera edición |
 *
 * \copyright  ZIGOR R&D AIE
 *
 */

 #include "fir_filter.h"

 // Declaración de funciones
 void Init_Fir(void);
 FIR_FILTER_OBJECT Get_Fir(unsigned int, float *, float *);
 float Fir_Filter (float, FIR_FILTER_OBJECT *);


 // Definición de Variables globales

 FIR_FILTER_API fir_api;


 // Definición de funciones

 void Init_Fir(void)
 {
     fir_api.fir_filter=Fir_Filter;
     fir_api.get_fir=Get_Fir;
 }

 FIR_FILTER_OBJECT Get_Fir(unsigned int ncoef, float * pcoef, float * pz)
 {
     FIR_FILTER_OBJECT objeto;
     unsigned int index;
     float * pw;

     pw=pz;
     for (index=0;index<ncoef;index++)
        *(pw++)=0;
     objeto.ncoef=ncoef;
     objeto.pcoef=pcoef;
     objeto.pz=pz;
     return objeto;
 }

 float fir_filter(float xn, FIR_FILTER_OBJECT * pfir)
 {
     unsigned int index, N;
     float * pmax;
     float * pinit;
     float y;

     if (pfir==NULL)
     {
         return 0.0f;
     }

     y=0.0f;
     N=pfir->ncoef+1;
     if (N>MAX_FIR_LENGTH)
     {
         return 0.0f;
     }

     pmax=(pfir->pz)+(pfir->ncoef);
     pinit=pfir->p_write;
     *(pfir->p_write++)=xn;
     if (pfir->p_write==pmax)
     {
         pfir->p_write=pfir->pz;
     }
     if (pinit==pmax)
     {
         pinit=pfir->pz;
     }




     for (index=0;index<N;index++)
     {
         y+=*(pfir->pcoef++)**(pinit++);
         if (pinit==pmax)
         {
             pinit=pfir->pz;
         }

     }
     return y;

 }
