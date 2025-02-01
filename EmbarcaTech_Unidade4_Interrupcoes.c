#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "WS2812.pio.h"

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
const uint luminosity_G = 200; // Luminosidade máxima do LED verde
const uint luminosity_B = 0; // Luminosidade máxima do LED azul

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


    // Loop principal
    while (true)
    {
        

    }
}
