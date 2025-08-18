/** \page wavelet_decim DESCOMPOSICIÓN WAVELET
 * \brief Módulo de decimación y descomposición wavelet multinivel
 *
 * Este módulo implementa la descomposición wavelet mediante bancos de filtros
 * con decimación por factor 2. Soporta descomposición multinivel usando
 * diferentes tipos de filtros: Lagrange halfband, Daubechies 4 y Daubechies 8.
 * Solo realiza la descomposición (análisis), no la reconstrucción.
 *
 * \section teoria_wavelet Teoría de la descomposición wavelet
 *
 * La descomposición wavelet descompone una señal en coeficientes de aproximación
 * (paso bajo) y detalle (paso alto) en múltiples niveles:
 *
 * \dot
 * digraph wavelet_decomp {
 *   rankdir=LR;
 *   node [shape=box];
 *
 *   X [label="x[n]"];
 *   H0_1 [label="H₀(z)", style=filled, fillcolor=lightblue];
 *   H1_1 [label="H₁(z)", style=filled, fillcolor=lightpink];
 *   D2_1 [label="↓2"];
 *   D2_2 [label="↓2"];
 *   A1 [label="A₁", shape=ellipse];
 *   D1 [label="D₁", shape=ellipse];
 *
 *   H0_2 [label="H₀(z)", style=filled, fillcolor=lightblue];
 *   H1_2 [label="H₁(z)", style=filled, fillcolor=lightpink];
 *   D2_3 [label="↓2"];
 *   D2_4 [label="↓2"];
 *   A2 [label="A₂", shape=ellipse];
 *   D2 [label="D₂", shape=ellipse];
 *
 *   X -> H0_1 -> D2_1 -> A1;
 *   X -> H1_1 -> D2_2 -> D1;
 *   A1 -> H0_2 -> D2_3 -> A2;
 *   A1 -> H1_2 -> D2_4 -> D2;
 * }
 * \enddot
 *
 * Donde:
 * - H₀(z): Filtro paso bajo (scaling function)
 * - H₁(z): Filtro paso alto (wavelet function)
 * - A_i: Coeficientes de aproximación nivel i
 * - D_i: Coeficientes de detalle nivel i
 * - ↓2: Decimación por factor 2
 *
 * \section tipos_filtros_wavelet Tipos de filtros soportados
 *
 * - **WAVELET_LAGRANGE**: Filtros de media banda de Lagrange
 * - **WAVELET_DB4**: Daubechies 4 (4 coeficientes)
 * - **WAVELET_DB8**: Daubechies 8 (8 coeficientes)
 *
 * \section uso_wavelet_decim Uso del módulo
 *
 * Ejemplo de descomposición de 3 niveles:
 * \code
 * // Inicializar módulo
 * Init_Wavelet_Decim();
 *
 * // Suscribir servicio para 3 niveles con Daubechies 4
 * WAVELET_HANDLE handle = wavelet_api.suscribe_wavelet_decimator(WAVELET_DB4, 0, 3);
 *
 * // Procesar muestras
 * WAVELET_OUTPUT output;
 * for (int i = 0; i < num_samples; i++) {
 *     wavelet_api.process_wavelet_decomposition(handle, input[i], &output);
 *     if (output.ready) {
 *         printf("A3: %f, D3: %f, D2: %f, D1: %f\n",
 *                output.approximation,
 *                output.detail[2], output.detail[1], output.detail[0]);
 *     }
 * }
 *
 * // Liberar servicio
 * wavelet_api.unsuscribe_wavelet_decimator(handle);
 * \endcode
 *
 * \section funciones_wavelet Descripción de funciones principales
 *
 * \subsection suscribe_wavelet Suscribe_Wavelet_Decimator
 * Reserva un servicio de descomposición wavelet multinivel.
 *
 * \param type Tipo de filtro (WAVELET_LAGRANGE, WAVELET_DB4, WAVELET_DB8)
 * \param m Parámetro m para Lagrange (1-16), ignorado para Db4/Db8
 * \param levels Número de niveles de descomposición (1-8)
 * \return Handle del servicio o -1 si error
 *
 * \subsection process_wavelet Process_Wavelet_Decomposition
 * Procesa una muestra y realiza la descomposición wavelet multinivel.
 *
 * El algoritmo realiza:
 * 1. Filtrado paso bajo y alto en el primer nivel
 * 2. Decimación por 2 de ambas salidas
 * 3. El resultado paso bajo se propaga al siguiente nivel
 * 4. El proceso se repite para todos los niveles
 *
 * \param handle Identificador del servicio
 * \param input Muestra de entrada
 * \param output Estructura con coeficientes de aproximación y detalle
 * \return WAVELET_DECIM_OK si éxito, WAVELET_DECIM_KO si error
 *
 * \author Dr. Carlos Romero
 *
 * \section historial_wavelet Historial de cambios
 * | Fecha | Autor | Versión | Descripción |
 * |:-----:|:-----:|:-------:|:------------|
 * | 14/08/2025 | Dr. Carlos Romero | 1 | Implementación inicial de descomposición wavelet |
 * | 17/08/2025 | Dr. Carlos Romero | 2 | Eliminación de código de test unitario |
 *
 * \copyright ZGR R&D AIE
 */

#include "wavelet_decim.h"


/* Declaración de funciones */
void Init_Wavelet_Decim(void);
WAVELET_HANDLE Suscribe_Wavelet_Decimator(wavelet_filter_type type, int m, int levels);
int Unsuscribe_Wavelet_Decimator(WAVELET_HANDLE handle);
int Process_Wavelet_Decomposition(WAVELET_HANDLE handle, float input, WAVELET_OUTPUT *output);
int Get_Wavelet_Info(WAVELET_HANDLE handle, wavelet_filter_type *type, int *levels, int *num_coeffs);
int Reset_Wavelet_Buffers(WAVELET_HANDLE handle);
void Get_Wavelet_Daubechies4_Coeffs(float *h0, float *h1);
void Get_Wavelet_Daubechies8_Coeffs(float *h0, float *h1);
static int Initialize_Wavelet_Coeffs(WAVELET_DECIM_SERVICE *service, wavelet_filter_type type, int m);
static float Apply_Wavelet_Filter(WAVELET_BUFFER *buffer, float *coeffs, int num_coeffs);
static void Write_To_Wavelet_Buffer(WAVELET_BUFFER *buffer, float value);

/* Variables globales */
WAVELET_DECIM_SERVICE wavelet_services[MAX_WAVELET_SERVICES];
WAVELET_DECIM_API wavelet_api;

/* Coeficientes Daubechies 4 */
static const float WAVELET_DB4_H0[4] = {
    0.48296291314469025f,   /* (1 + sqrt(3))/(4*sqrt(2)) */
    0.83651630373746899f,   /* (3 + sqrt(3))/(4*sqrt(2)) */
    0.22414386804185735f,   /* (3 - sqrt(3))/(4*sqrt(2)) */
    -0.12940952255092145f   /* (1 - sqrt(3))/(4*sqrt(2)) */
};

/* Coeficientes Daubechies 8 */
static const float WAVELET_DB8_H0[8] = {
    0.23037781330885523f,
    0.71484657055254153f,
    0.63088076792959036f,
    -0.02798376941698385f,
    -0.18703481171888114f,
    0.03084138183598697f,
    0.03288301166698295f,
    -0.01059740178500278f
};

/* Definición de funciones */

void Init_Wavelet_Decim(void)
{
    int i, j, k;

    /* Inicializar API pública */
    wavelet_api.suscribe_wavelet_decimator = Suscribe_Wavelet_Decimator;
    wavelet_api.unsuscribe_wavelet_decimator = Unsuscribe_Wavelet_Decimator;
    wavelet_api.process_wavelet_decomposition = Process_Wavelet_Decomposition;
    wavelet_api.get_wavelet_info = Get_Wavelet_Info;
    wavelet_api.reset_wavelet_buffers = Reset_Wavelet_Buffers;

    /* Inicializar todos los servicios */
    for (i = 0; i < MAX_WAVELET_SERVICES; i++)
    {
        wavelet_services[i].status = WD_FREE;
        wavelet_services[i].filter = WAVELET_LAGRANGE;
        wavelet_services[i].m = 0;
        wavelet_services[i].filter_order = 0;
        wavelet_services[i].num_coeffs = 0;
        wavelet_services[i].num_levels = 0;

        /* Limpiar coeficientes */
        for (j = 0; j < MAX_FILTER_ORDER; j++)
        {
            wavelet_services[i].h0[j] = 0.0f;
            wavelet_services[i].h1[j] = 0.0f;
        }

        /* Limpiar niveles de descomposición */
        for (j = 0; j < MAX_DECOMP_LEVELS; j++)
        {
            wavelet_services[i].levels[j].sample_counter = 0;
            wavelet_services[i].levels[j].last_lowpass = 0.0f;
            wavelet_services[i].levels[j].last_highpass = 0.0f;
            wavelet_services[i].levels[j].lowpass_buffer.write_index = 0;
            wavelet_services[i].levels[j].lowpass_buffer.buffer_size = 0;
            wavelet_services[i].levels[j].highpass_buffer.write_index = 0;
            wavelet_services[i].levels[j].highpass_buffer.buffer_size = 0;

            for (k = 0; k < MAX_FILTER_ORDER; k++)
            {
                wavelet_services[i].levels[j].lowpass_buffer.buffer[k] = 0.0f;
                wavelet_services[i].levels[j].highpass_buffer.buffer[k] = 0.0f;
            }
        }
    }
}

WAVELET_HANDLE Suscribe_Wavelet_Decimator(wavelet_filter_type type, int m, int levels)
{
    int i, j;
    WAVELET_HANDLE handle;
    int result;

    /* Validar parámetros */
    if (type != WAVELET_LAGRANGE && type != WAVELET_DB4 && type != WAVELET_DB8)
    {
        return -1;
    }

    if (type == WAVELET_LAGRANGE && (m < 1 || m > 16))
    {
        return -1;
    }

    if (levels < 1 || levels > MAX_DECOMP_LEVELS)
    {
        return -1;
    }

    /* Buscar servicio libre */
    handle = -1;
    for (i = 0; i < MAX_WAVELET_SERVICES; i++)
    {
        if (wavelet_services[i].status == WD_FREE)
        {
            handle = i;
            break;
        }
    }

    if (handle == -1)
    {
        return -1;
    }

    /* Configurar servicio */
    wavelet_services[handle].status = WD_ASIGNED;
    wavelet_services[handle].filter = type;
    wavelet_services[handle].m = m;
    wavelet_services[handle].num_levels = levels;

    /* Inicializar coeficientes */
    result = Initialize_Wavelet_Coeffs(&wavelet_services[handle], type, m);
    if (result != WAVELET_DECIM_OK)
    {
        wavelet_services[handle].status = WD_FREE;
        return -1;
    }

    /* Configurar buffers para cada nivel */
    for (i = 0; i < levels; i++)
    {
        wavelet_services[handle].levels[i].lowpass_buffer.buffer_size =
            wavelet_services[handle].num_coeffs;
        wavelet_services[handle].levels[i].highpass_buffer.buffer_size =
            wavelet_services[handle].num_coeffs;
        wavelet_services[handle].levels[i].lowpass_buffer.write_index = 0;
        wavelet_services[handle].levels[i].highpass_buffer.write_index = 0;
        wavelet_services[handle].levels[i].sample_counter = 0;
        wavelet_services[handle].levels[i].last_lowpass = 0.0f;
        wavelet_services[handle].levels[i].last_highpass = 0.0f;

        /* Limpiar buffers */
        for (j = 0; j < MAX_FILTER_ORDER; j++)
        {
            wavelet_services[handle].levels[i].lowpass_buffer.buffer[j] = 0.0f;
            wavelet_services[handle].levels[i].highpass_buffer.buffer[j] = 0.0f;
        }
    }

    return handle;
}

int Unsuscribe_Wavelet_Decimator(WAVELET_HANDLE handle)
{
    int i, j;

    /* Validar handle */
    if (handle < 0 || handle >= MAX_WAVELET_SERVICES)
    {
        return WAVELET_DECIM_KO;
    }

    if (wavelet_services[handle].status != WD_ASIGNED)
    {
        return WAVELET_DECIM_KO;
    }

    /* Liberar servicio */
    wavelet_services[handle].status = WD_FREE;
    wavelet_services[handle].filter = WAVELET_LAGRANGE;
    wavelet_services[handle].m = 0;
    wavelet_services[handle].filter_order = 0;
    wavelet_services[handle].num_coeffs = 0;
    wavelet_services[handle].num_levels = 0;

    /* Limpiar arrays y buffers */
    for (i = 0; i < MAX_FILTER_ORDER; i++)
    {
        wavelet_services[handle].h0[i] = 0.0f;
        wavelet_services[handle].h1[i] = 0.0f;
    }

    for (i = 0; i < MAX_DECOMP_LEVELS; i++)
    {
        wavelet_services[handle].levels[i].sample_counter = 0;
        wavelet_services[handle].levels[i].last_lowpass = 0.0f;
        wavelet_services[handle].levels[i].last_highpass = 0.0f;

        for (j = 0; j < MAX_FILTER_ORDER; j++)
        {
            wavelet_services[handle].levels[i].lowpass_buffer.buffer[j] = 0.0f;
            wavelet_services[handle].levels[i].highpass_buffer.buffer[j] = 0.0f;
        }
    }

    return WAVELET_DECIM_OK;
}

int Process_Wavelet_Decomposition(WAVELET_HANDLE handle, float input, WAVELET_OUTPUT *output)
{
    int level;
    float current_input;
    float lowpass_out, highpass_out;

    /* Validar parámetros */
    if (handle < 0 || handle >= MAX_WAVELET_SERVICES || output == NULL)
    {
        return WAVELET_DECIM_KO;
    }

    if (wavelet_services[handle].status != WD_ASIGNED)
    {
        return WAVELET_DECIM_KO;
    }

    /* Inicializar estructura de salida */
    output->ready = 0;
    output->approximation = 0.0f;
    for (level = 0; level < MAX_DECOMP_LEVELS; level++)
    {
        output->detail[level] = 0.0f;
        output->detail_ready[level] = 0;
    }

    /* Procesar primer nivel con la entrada original */
    current_input = input;

    /* Para cada nivel de descomposición */
    for (level = 0; level < wavelet_services[handle].num_levels; level++)
    {
        /* Aplicar filtros paso bajo y paso alto */
        if (level == 0)
        {
            /* Primer nivel: procesar entrada original */
            Write_To_Wavelet_Buffer(&wavelet_services[handle].levels[level].lowpass_buffer,
                                   current_input);
            Write_To_Wavelet_Buffer(&wavelet_services[handle].levels[level].highpass_buffer,
                                   current_input);
        }
        else
        {
            /* Niveles siguientes: procesar solo si el nivel anterior produjo salida */
            if (wavelet_services[handle].levels[level-1].sample_counter == 1)
            {
                current_input = wavelet_services[handle].levels[level-1].last_lowpass;
                Write_To_Wavelet_Buffer(&wavelet_services[handle].levels[level].lowpass_buffer,
                                       current_input);
                Write_To_Wavelet_Buffer(&wavelet_services[handle].levels[level].highpass_buffer,
                                       current_input);
            }
            else
            {
                /* No hay nueva entrada para este nivel */
                break;
            }
        }

        /* Aplicar filtros */
        lowpass_out = Apply_Wavelet_Filter(&wavelet_services[handle].levels[level].lowpass_buffer,
                                          wavelet_services[handle].h0,
                                          wavelet_services[handle].num_coeffs);

        highpass_out = Apply_Wavelet_Filter(&wavelet_services[handle].levels[level].highpass_buffer,
                                           wavelet_services[handle].h1,
                                           wavelet_services[handle].num_coeffs);

        /* Decimación: tomar 1 de cada 2 muestras */
        if (wavelet_services[handle].levels[level].sample_counter == 0)
        {
            /* Guardar valores decimados */
            wavelet_services[handle].levels[level].last_lowpass = lowpass_out;
            wavelet_services[handle].levels[level].last_highpass = highpass_out;

            /* Marcar detalle como disponible */
            output->detail[level] = highpass_out;
            output->detail_ready[level] = 1;

            /* Si es el último nivel, la aproximación está lista */
            if (level == wavelet_services[handle].num_levels - 1)
            {
                output->approximation = lowpass_out;
                output->ready = 1;
            }
        }

        /* Alternar contador de decimación */
        wavelet_services[handle].levels[level].sample_counter =
            (wavelet_services[handle].levels[level].sample_counter + 1) % 2;
    }

    return WAVELET_DECIM_OK;
}

int Get_Wavelet_Info(WAVELET_HANDLE handle, wavelet_filter_type *type, int *levels, int *num_coeffs)
{
    /* Validar handle */
    if (handle < 0 || handle >= MAX_WAVELET_SERVICES)
    {
        return WAVELET_DECIM_KO;
    }

    if (wavelet_services[handle].status != WD_ASIGNED)
    {
        return WAVELET_DECIM_KO;
    }

    /* Retornar información */
    if (type != NULL)
    {
        *type = wavelet_services[handle].filter;
    }

    if (levels != NULL)
    {
        *levels = wavelet_services[handle].num_levels;
    }

    if (num_coeffs != NULL)
    {
        *num_coeffs = wavelet_services[handle].num_coeffs;
    }

    return WAVELET_DECIM_OK;
}

int Reset_Wavelet_Buffers(WAVELET_HANDLE handle)
{
    int i, j;

    /* Validar handle */
    if (handle < 0 || handle >= MAX_WAVELET_SERVICES)
    {
        return WAVELET_DECIM_KO;
    }

    if (wavelet_services[handle].status != WD_ASIGNED)
    {
        return WAVELET_DECIM_KO;
    }

    /* Resetear todos los buffers y contadores */
    for (i = 0; i < wavelet_services[handle].num_levels; i++)
    {
        wavelet_services[handle].levels[i].sample_counter = 0;
        wavelet_services[handle].levels[i].last_lowpass = 0.0f;
        wavelet_services[handle].levels[i].last_highpass = 0.0f;
        wavelet_services[handle].levels[i].lowpass_buffer.write_index = 0;
        wavelet_services[handle].levels[i].highpass_buffer.write_index = 0;

        for (j = 0; j < MAX_FILTER_ORDER; j++)
        {
            wavelet_services[handle].levels[i].lowpass_buffer.buffer[j] = 0.0f;
            wavelet_services[handle].levels[i].highpass_buffer.buffer[j] = 0.0f;
        }
    }

    return WAVELET_DECIM_OK;
}

static int Initialize_Wavelet_Coeffs(WAVELET_DECIM_SERVICE *service, wavelet_filter_type type, int m)
{
    int i, result;
    int L,signo;

    switch (type)
    {
        case WAVELET_LAGRANGE:
            /* Generar coeficientes Lagrange para h0 */
            service->filter_order = 4 * m - 2;
            service->num_coeffs = 4 * m - 1;

            result = lagrange_halfband(m, service->h0);
            if (result != LAGRANGE_OK)
            {
                return WAVELET_DECIM_KO;
            }

            /* Para Lagrange halfband, h1 es el filtro complementario */
            /* h1[n] = (-1)^(N-1-n) * h0[N-1-n] */
            L=service->num_coeffs;
            if (L%2==0)
            {
                signo=-1;
            }
            else
            {
                signo=1;
            }

            for (i = 0; i < service->num_coeffs; i++)
            {
                service->h1[i] = signo* service->h0[L-1-i];
                if (signo==1)
                    signo=-1;
                else
                    signo=1;
            }
            break;

        case WAVELET_DB4:
            service->filter_order = 3;
            service->num_coeffs = 4;
            Get_Wavelet_Daubechies4_Coeffs(service->h0, service->h1);
            break;

        case WAVELET_DB8:
            service->filter_order = 7;
            service->num_coeffs = 8;
            Get_Wavelet_Daubechies8_Coeffs(service->h0, service->h1);
            break;

        default:
            return WAVELET_DECIM_KO;
    }

    return WAVELET_DECIM_OK;
}

void Get_Wavelet_Daubechies4_Coeffs(float *h0, float *h1)
{
    int i;

    /* Copiar coeficientes lowpass (h0) */
    for (i = 0; i < 4; i++)
    {
        h0[i] = WAVELET_DB4_H0[i];
    }

    /* Generar coeficientes highpass (h1) */
    /* h1[n] = (-1)^n * h0[N-1-n] */
    for (i = 0; i < 4; i++)
    {
        h1[i] = (i % 2 == 0 ? 1.0f : -1.0f) * h0[3 - i];
    }
}

void Get_Wavelet_Daubechies8_Coeffs(float *h0, float *h1)
{
    int i;

    /* Copiar coeficientes lowpass (h0) */
    for (i = 0; i < 8; i++)
    {
        h0[i] = WAVELET_DB8_H0[i];
    }

    /* Generar coeficientes highpass (h1) */
    /* h1[n] = (-1)^n * h0[N-1-n] */
    for (i = 0; i < 8; i++)
    {
        h1[i] = (i % 2 == 0 ? 1.0f : -1.0f) * h0[7 - i];
    }
}

static void Write_To_Wavelet_Buffer(WAVELET_BUFFER *buffer, float value)
{
    buffer->buffer[buffer->write_index] = value;
    buffer->write_index = (buffer->write_index + 1) % buffer->buffer_size;
}

static float Apply_Wavelet_Filter(WAVELET_BUFFER *buffer, float *coeffs, int num_coeffs)
{
    float sum;
    int i, buf_idx;

    sum = 0.0f;

    /* Convolución con buffer circular */
    for (i = 0; i < num_coeffs; i++)
    {
        buf_idx = (buffer->write_index - 1 - i + buffer->buffer_size) % buffer->buffer_size;
        sum += coeffs[i] * buffer->buffer[buf_idx];
    }

    return sum;
}

