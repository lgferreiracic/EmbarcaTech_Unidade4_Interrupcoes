#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define NUM_PIXELS 25
#define TIME 200

// Configurações dos pinos
const uint led_R = 11; // Red=> GPIO11
const uint led_B = 12; // Blue => GPIO12
const uint led_G = 13; // Green => GPIO13
const uint WS2812_PIN = 7; // WS2812 => GPIO7
const uint button_a = 5; // Botão A = 5
const uint button_b = 6; // Botão B = 6 
const uint luminosity_R = 0; // Luminosidade máxima do LED vermelho
const uint luminosity_G = 0; // Luminosidade máxima do LED verde
const uint luminosity_B = 200; // Luminosidade máxima do LED azul

int number_ws2812 = 0; // Variável para armazenar o número a ser exibido

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
