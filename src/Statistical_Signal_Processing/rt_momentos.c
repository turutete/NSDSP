/** \page rt_momentos PROCESAMIENTO ESTADÍSTICO
 * \brief Módulo de cálculo de momentos estadísticos en tiempo real
 *
 * Este módulo implementa el cálculo en tiempo real de los 4 primeros momentos estadísticos
 * de una señal utilizando filtros de media móvil (MA) con buffers circulares. Los momentos
 * calculados son: Media (M1), Varianza (M2), Asimetría (M3) y Curtosis (M4).
 *
 * \section uso_rt Uso del módulo
 *
 * Para utilizar este módulo:
 * 1. Inicializar con Init_RT_Momentos() (llamado automáticamente por Init_NSDSP())
 * 2. Suscribir un servicio: service = pse.suscribe_rt_momentos()
 * 3. Procesar muestras: pse.compute_rt_momentos(service, muestra)
 * 4. Leer resultados desde nsdsp_statistical_objects[service]
 * 5. Liberar servicio: pse.unsuscribe_rt_momentos(service)
 *
 * Ejemplo:
 * \code
 * RT_MOMENTOS_SERVICE service = pse.suscribe_rt_momentos();
 * for (int i = 0; i < 1000; i++) {
 *     float muestra = obtener_muestra();
 *     pse.compute_rt_momentos(service, muestra);
 * }
 * printf("Media: %f\n", nsdsp_statistical_objects[service].media);
 * pse.unsuscribe_rt_momentos(service);
 * \endcode
 *
 * \section funciones_rt Descripción de funciones
 *
 * \subsection init_rt Init_RT_Momentos
 * Inicializa la estructura de punteros a funciones pse (Public Service Endpoints).
 * Esta función debe ser llamada antes de usar cualquier servicio del módulo.
 * Asigna los punteros a las funciones Suscribe, Unsuscribe y Compute.
 *
 * \subsection suscribe_rt Suscribe_RT_Momentos
 * Busca un servicio libre en el array de servicios y lo asigna al usuario.
 *
 * \dot
 * digraph suscribe_state {
 *   rankdir=LR;
 *   node [shape=box];
 *
 *   START [label="Inicio"];
 *   SEARCH [label="Buscar servicio\nlibre", style=filled, fillcolor=lightyellow];
 *   FOUND [label="Servicio\nencontrado", shape=diamond, style=filled, fillcolor=lightblue];
 *   ASSIGN [label="Marcar ASIGNED\nInicializar buffers", style=filled, fillcolor=lightgreen];
 *   RETURN_ID [label="Retornar ID"];
 *   RETURN_NONE [label="Retornar NONE"];
 *
 *   START -> SEARCH;
 *   SEARCH -> FOUND;
 *   FOUND -> ASSIGN [label="Sí"];
 *   FOUND -> RETURN_NONE [label="No"];
 *   ASSIGN -> RETURN_ID;
 * }
 * \enddot
 *
 * \return Identificador del servicio (0 a MAX_RT_MOMENTOS-1) o NONE si no hay disponibles
 *
 * \subsection unsuscribe_rt Unsuscribe_RT_Momentos
 * Libera un servicio previamente asignado y resetea toda su memoria.
 * \param id_service Identificador del servicio a liberar
 * \return RT_MOMENTOS_OK si se liberó correctamente, RT_MOMENTOS_KO si error
 *
 * \subsection compute_rt Compute_RT_Momentos
 * Calcula los 4 momentos estadísticos para una nueva muestra de señal.
 *
 * Arquitectura del procesamiento:
 * \dot
 * digraph compute_architecture {
 *   rankdir=LR;
 *   node [shape=box, style=rounded];
 *
 *   xn [label="x(n)", shape=plaintext];
 *   MA1 [label="MA", style=filled, fillcolor=lightpink];
 *   MA2 [label="MA", style=filled, fillcolor=lightpink];
 *   MA3 [label="MA", style=filled, fillcolor=lightpink];
 *   MA4 [label="MA", style=filled, fillcolor=lightpink];
 *
 *   sub [label="-", shape=circle];
 *   pow2 [label="()²", shape=box, style=filled, fillcolor=lightblue];
 *   pow3 [label="()³", shape=box, style=filled, fillcolor=lightgreen];
 *   pow4 [label="()⁴", shape=box, style=filled, fillcolor=lightgreen];
 *
 *   div1 [label="÷", shape=box, style=filled, fillcolor=lightyellow];
 *   div2 [label="÷", shape=box, style=filled, fillcolor=lightyellow];
 *   sqrt [label="SQRT", shape=box, style=filled, fillcolor=lightyellow];
 *
 *   M1 [label="M1(n)", shape=plaintext];
 *   M2 [label="M2(n)", shape=plaintext];
 *   M3 [label="M3(n)", shape=plaintext];
 *   M4 [label="M4(n)", shape=plaintext];
 *
 *   xn -> MA1 -> M1;
 *   xn -> sub;
 *   MA1 -> sub [style=dashed];
 *
 *   sub -> pow2 -> MA2 -> M2;
 *   sub -> pow3 -> div1;
 *   sub -> pow4 -> div2;
 *
 *   MA2 -> sqrt -> div1;
 *   MA3 -> M3;
 *   div1 -> MA3;
 *
 *   MA2 -> div2;
 *   MA4 -> M4;
 *   div2 -> MA4;
 * }
 * \enddot
 *
 * Las fórmulas implementadas son:
 * - Media: \f$ M_1 = \frac{1}{N}\sum_{i=0}^{N-1} x[n-i] \f$
 * - Varianza: \f$ M_2 = \frac{1}{N}\sum_{i=0}^{N-1} (x[n-i] - M_1)^2 \f$
 * - Asimetría: \f$ M_3 = \frac{\frac{1}{N}\sum_{i=0}^{N-1} (x[n-i] - M_1)^3}{\sqrt{M_2}^3} \f$
 * - Curtosis: \f$ M_4 = \frac{\frac{1}{N}\sum_{i=0}^{N-1} (x[n-i] - M_1)^4}{M_2^2} \f$
 *
 * \param id_service Identificador del servicio
 * \param xn Muestra actual de la señal x(n)
 * \return RT_MOMENTOS_OK si cálculo correcto, RT_MOMENTOS_KO si división por cero
 *
 * \subsection ma_filter_rt MA_Filter
 * Implementa un filtro de media móvil con buffer circular de N_MA muestras.
 *
 * \dot
 * digraph ma_filter {
 *   rankdir=LR;
 *   node [shape=box];
 *
 *   INPUT [label="x(n)"];
 *   WRITE [label="Escribir en\nbuffer[index_w]", style=filled, fillcolor=lightblue];
 *   SUM [label="Sumar todos\nlos elementos", style=filled, fillcolor=lightyellow];
 *   UPDATE [label="index_w =\n(index_w+1)%N_MA", style=filled, fillcolor=lightgreen];
 *   OUTPUT [label="suma/N_MA"];
 *
 *   INPUT -> WRITE -> SUM -> UPDATE -> OUTPUT;
 * }
 * \enddot
 *
 * \param pz Puntero al buffer circular
 * \param xn Nueva muestra a procesar
 * \return Valor promedio de las últimas N_MA muestras
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_rt Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 22/06/2025 | Dr. Carlos Romero | 1 | Primera edición |
 * | 12/07/2025 | Dr. Carlos Romero | 2 | Implementación completa de los 4 momentos |
 * | 03/08/2025 | Dr. Carlos Romero | 3 | Actualización documentación Doxygen según estándar |
 *
 * \copyright ZGR R&D AIE
 */

#include "nsdsp_statistical.h"
#include "rt_momentos.h"

// Declaración de funciones
void Init_RT_Momentos(void);
RT_MOMENTOS_SERVICE Suscribe_RT_Momentos(void);
int Unsuscribe_RT_Momentos(RT_MOMENTOS_SERVICE);
int Compute_RT_Momentos(RT_MOMENTOS_SERVICE, float);
float MA_Filter(BUFFER_Z *, float);

// Declaración externa para la vista simplificada
statistical_object nsdsp_statistical_objects[MAX_RT_MOMENTOS];

// Atributos
RT_MOMENTOS servicios_rt_momentos[MAX_RT_MOMENTOS] = {0};  // Buffer de objetos RT_MOMENTOS
SSP pse;

// Definición de funciones

void Init_RT_Momentos(void)
{
    pse.suscribe_rt_momentos = Suscribe_RT_Momentos;
    pse.unsuscribe_rt_momentos = Unsuscribe_RT_Momentos;
    pse.compute_rt_momentos = Compute_RT_Momentos;
}

RT_MOMENTOS_SERVICE Suscribe_RT_Momentos(void)
{
    static RT_MOMENTOS_SERVICE service = 0;
    RT_MOMENTOS_SERVICE result;
    unsigned int checked, estado;

    estado = 0;
    checked = 0;

    while (estado == 0)
    {
        if (servicios_rt_momentos[service].status == FREE)
        {
            result = service;
            servicios_rt_momentos[service].status = ASIGNED;

            // Inicializar los índices de escritura de los buffers
            servicios_rt_momentos[service].z_buffers.mu_z.index_w = 0;
            servicios_rt_momentos[service].z_buffers.sigma2_z.index_w = 0;
            servicios_rt_momentos[service].z_buffers.a_z.index_w = 0;
            servicios_rt_momentos[service].z_buffers.c_z.index_w = 0;

            // Inicializar los buffers a cero
            for (int i = 0; i < N_MA; i++)
            {
                servicios_rt_momentos[service].z_buffers.mu_z.buffer_z[i] = 0.0f;
                servicios_rt_momentos[service].z_buffers.sigma2_z.buffer_z[i] = 0.0f;
                servicios_rt_momentos[service].z_buffers.a_z.buffer_z[i] = 0.0f;
                servicios_rt_momentos[service].z_buffers.c_z.buffer_z[i] = 0.0f;
            }

            // Inicializar los momentos
            servicios_rt_momentos[service].mu = 0.0f;
            servicios_rt_momentos[service].var2 = 0.0f;
            servicios_rt_momentos[service].A = 0.0f;
            servicios_rt_momentos[service].C = 0.0f;

            service++;
            estado = 1;
        }
        else
        {
            service++;
        }

        if (service == MAX_RT_MOMENTOS)
            service = 0;

        checked++;
        if (checked == MAX_RT_MOMENTOS && estado == 0)
            estado = 2;
    }

    if (estado != 1)
        result = (RT_MOMENTOS_SERVICE)(NONE);

    return (result);
}

int Unsuscribe_RT_Momentos(RT_MOMENTOS_SERVICE id_service)
{
    int result;

    result = RT_MOMENTOS_KO;

    if (id_service >= 0 && id_service < MAX_RT_MOMENTOS &&
        servicios_rt_momentos[id_service].status == ASIGNED)
    {
        servicios_rt_momentos[id_service] = (RT_MOMENTOS){0};
        result = RT_MOMENTOS_OK;
    }
    return (result);
}

int Compute_RT_Momentos(RT_MOMENTOS_SERVICE id_service, float xn)
{
    int result;
    float mu_out;
    float diff;
    float diff2, diff3, diff4;
    float sigma2_out;
    float asimetria_input;
    float curtosis_input;
    float sqrt_sigma2;
    float sigma2_cubed;
    float sigma2_squared;

    result = RT_MOMENTOS_KO;

    if (id_service >= 0 && id_service < MAX_RT_MOMENTOS &&
        servicios_rt_momentos[id_service].status == ASIGNED)
    {
        result = RT_MOMENTOS_OK;

        // M1: Media móvil de x(n)
        mu_out = MA_Filter(&servicios_rt_momentos[id_service].z_buffers.mu_z, xn);
        servicios_rt_momentos[id_service].mu = mu_out;

        // Actualizar vista simplificada
        nsdsp_statistical_objects[id_service].media = mu_out;

        // Calcular (x(n) - M1)
        diff = xn - mu_out;

        // Calcular potencias de la diferencia
        diff2 = diff * diff;
        diff3 = diff2 * diff;
        diff4 = diff2 * diff2;

        // M2: Varianza = MA((x(n) - M1)²)
        sigma2_out = MA_Filter(&servicios_rt_momentos[id_service].z_buffers.sigma2_z, diff2);
        servicios_rt_momentos[id_service].var2 = sigma2_out;

        // Actualizar vista simplificada
        nsdsp_statistical_objects[id_service].varianza = sigma2_out;

        // M3: Asimetría = MA((x(n) - M1)³ / sqrt(M2)³)
        // Protección contra división por cero
        if (sigma2_out > 0.0f)
        {
            sqrt_sigma2 = sqrtf(sigma2_out);
            sigma2_cubed = sqrt_sigma2 * sqrt_sigma2 * sqrt_sigma2;

            if (sigma2_cubed > 0.0f)
            {
                asimetria_input = diff3 / sigma2_cubed;
                servicios_rt_momentos[id_service].A = MA_Filter(&servicios_rt_momentos[id_service].z_buffers.a_z, asimetria_input);
                nsdsp_statistical_objects[id_service].asimetria = servicios_rt_momentos[id_service].A;
            }
            else
            {
                servicios_rt_momentos[id_service].A = 0.0f;
                nsdsp_statistical_objects[id_service].asimetria = 0.0f;
                result = RT_MOMENTOS_KO;
            }
        }
        else
        {
            servicios_rt_momentos[id_service].A = 0.0f;
            nsdsp_statistical_objects[id_service].asimetria = 0.0f;
            result = RT_MOMENTOS_KO;
        }

        // M4: Curtosis = MA((x(n) - M1)⁴ / M2²)
        // Protección contra división por cero
        if (sigma2_out > 0.0f)
        {
            sigma2_squared = sigma2_out * sigma2_out;
            curtosis_input = diff4 / sigma2_squared;
            servicios_rt_momentos[id_service].C = MA_Filter(&servicios_rt_momentos[id_service].z_buffers.c_z, curtosis_input);
            nsdsp_statistical_objects[id_service].curtosis = servicios_rt_momentos[id_service].C;
        }
        else
        {
            servicios_rt_momentos[id_service].C = 0.0f;
            nsdsp_statistical_objects[id_service].curtosis = 0.0f;
            result = RT_MOMENTOS_KO;
        }
    }

    return (result);
}

float MA_Filter(BUFFER_Z *pz, float xn)
{
    float suma;
    unsigned int i;
    unsigned int index_w;

    // Obtener el índice de escritura actual
    index_w = pz->index_w;

    // Escribir el nuevo valor en el buffer circular
    pz->buffer_z[index_w] = xn;

    // Calcular la suma de todos los elementos del buffer
    suma = 0.0f;
    for (i = 0; i < N_MA; i++)
    {
        suma += pz->buffer_z[i];
    }

    // Actualizar el índice de escritura (buffer circular)
    pz->index_w = (index_w + 1) % N_MA;

    // Retornar el promedio
    return (suma * INV_N_MA);
}
