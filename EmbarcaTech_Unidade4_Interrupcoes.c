#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define NUM_PIXELS 25
#define TIME 200

// Configurações dos pinos
const uint led_R = 13; // Red=> GPIO11
const uint led_B = 12; // Blue => GPIO12
const uint led_G = 11; // Green => GPIO13
const uint WS2812_PIN = 7; // WS2812 => GPIO7
const uint button_a = 5; // Botão A = 5
const uint button_b = 6; // Botão B = 6 
const uint luminosity_R = 0; // Luminosidade máxima do LED vermelho
const uint luminosity_G = 0; // Luminosidade máxima do LED verde
const uint luminosity_B = 200; // Luminosidade máxima do LED azul

static volatile uint number_ws2812 = 0; // Variável para armazenar o número a ser exibido
static volatile uint32_t last_time = 0; // Armazena o tempo do último evento (em microssegundos)

// Prototipação da função de interrupção
static void gpio_irq_handler(uint gpio, uint32_t events);

// Buffers para armazenar quais LEDs estão ligados matriz 5x5
bool number_0[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool number_1[NUM_PIXELS] = {
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0
};

bool number_2[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0
};

bool number_3[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool number_4[NUM_PIXELS] = {
    0, 1, 0, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0
};

bool number_5[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool number_6[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 0, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool number_7[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0,
    0, 0, 0, 1, 0
};

bool number_8[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0
};

bool number_9[NUM_PIXELS] = {
    0, 1, 1, 1, 0,
    0, 1, 0, 1, 0,
    0, 1, 1, 1, 0,
    0, 0, 0, 1, 0,
    0, 1, 1, 1, 0
};

// Função para converter a posição do matriz para uma posição do vetor.
int getIndex(int x, int y) {
    // Se a linha for par (0, 2, 4), percorremos da esquerda para a direita.
    // Se a linha for ímpar (1, 3), percorremos da direita para a esquerda.
    if (y % 2 == 0) {
        return 24-(y * 5 + x); // Linha par (esquerda para direita).
    } else {
        return 24-(y * 5 + (4 - x)); // Linha ímpar (direita para esquerda).
    }
}

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) | ((uint32_t)(g) << 16) | (uint32_t)(b);
}


void set_ledS(bool number[NUM_PIXELS], uint8_t r, uint8_t g, uint8_t b)
{
    // Define a cor com base nos parâmetros fornecidos
    uint32_t color = urgb_u32(luminosity_R, luminosity_G, luminosity_B);
    
    // Define todos os LEDs com a cor especificada
    for (int i = 0; i < NUM_PIXELS; i++)
    {   
        int x = i % 5;
        int y = i / 5;
        int index = getIndex(x, y);

        if (number[index])
        {
            put_pixel(color); // Liga o LED com um no buffer
        }
        else
        {
            put_pixel(0);  // Desliga os LEDs com zero no buffer
        }
    }
}

// Função para exibir o número no display
void display_number(uint number)
{
    switch (number)
    {
    case 0:
        set_ledS(number_0, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 1:
        set_ledS(number_1, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 2:
        set_ledS(number_2, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 3:
        set_ledS(number_3, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 4:
        set_ledS(number_4, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 5:
        set_ledS(number_5, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 6:
        set_ledS(number_6, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 7:
        set_ledS(number_7, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 8:
        set_ledS(number_8, luminosity_R, luminosity_G, luminosity_B);
        break;
    case 9:
        set_ledS(number_9, luminosity_R, luminosity_G, luminosity_B);
        break;
    default:
        break;
    }
}

int main()
{
    // Inicializações
    stdio_init_all();

    // Inicializa os LEDs como saída 
    gpio_init(led_R);
    gpio_set_dir(led_R, GPIO_OUT);
    gpio_init(led_B);
    gpio_set_dir(led_B, GPIO_OUT);
    gpio_init(led_G);
    gpio_set_dir(led_G, GPIO_OUT);

    // Inicializa os botões como entrada
    gpio_init(button_a);
    gpio_set_dir(button_a, GPIO_IN);
    gpio_pull_up(button_a);
    gpio_init(button_b);
    gpio_set_dir(button_b, GPIO_IN);
    gpio_pull_up(button_b);

    //Configurações da PIO
    PIO pio = pio0; 
    uint offset = pio_add_program(pio, &pio_matrix_program);
    uint sm = pio_claim_unused_sm(pio, true);
    pio_matrix_program_init(pio, sm, offset, WS2812_PIN);

    // Configuração da interrupção com callback
    gpio_set_irq_enabled_with_callback(button_a, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(button_b, GPIO_IRQ_EDGE_FALL, true, &gpio_irq_handler);

    // Loop principal
    while (true)
    {
        // LED vermelho pisca 5 vezes por segundo
        gpio_put(led_R, 1);
        sleep_ms(TIME);
        gpio_put(led_R, 0);
        sleep_ms(TIME);
    }
}

// Função de interrupção com debouncing
void gpio_irq_handler(uint gpio, uint32_t events)
{
    // Obtém o tempo atual em microssegundos
    uint32_t current_time = to_us_since_boot(get_absolute_time());

    // Verifica se passou tempo suficiente desde o último evento
    if (current_time - last_time > 200000) // 200 ms de debouncing
    {
        last_time = current_time; // Atualiza o tempo do último evento

        if(gpio == button_a)
        {
            number_ws2812++; // Incrementa o número a ser exibido
            if (number_ws2812 > 9) // Se o número for maior que 9, reinicia
            {
                number_ws2812 = 0;
            }
        }
        else if(gpio == button_b)
        {
            number_ws2812--; // Decrementa o número a ser exibido
            if (number_ws2812 < 0) // Se o número for menor que 0, reinicia
            {
                number_ws2812 = 9;
            }
        }

        // Exibe o número no display
        display_number(number_ws2812);
    }
}
