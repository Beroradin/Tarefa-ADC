#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "hardware/timer.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "inc/ssd1306.h"
#include "inc/font.h"

// Definição dos pinos
#define EIXO_Y 26    // ADC0
#define EIXO_X 27    // ADC1
#define LED1_PIN 11  // LED controlado pelo botão do joystick
#define LED2_PIN 12  // LED controlado pelo eixo Y
#define LED3_PIN 13  // LED controlado pelo eixo X
#define I2C_SDA 14   // Pino SDA do i2c   
#define I2C_SCL 15   // Pino SCL do i2c
#define I2C_PORT i2c1    
#define BUTTON_PIN 5  // Botão
#define JOYSTICK_BUTTON 22 // Botão do joystick
#define PWM_WRAP    4095    // 12 bits de wrap (4096 valores)

// Constantes do display
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define SQUARE_SIZE 8

// Variáveis globais
volatile bool pwm_enabled = true;
volatile bool borda_style = false;
volatile bool green_value = false;
bool cor = true;
absolute_time_t last_interrupt_time = 0;


// Variáveis para posição do quadrado
int pos_x = (DISPLAY_WIDTH - SQUARE_SIZE) / 2;  // Centraliza horizontalmente
int pos_y = (DISPLAY_HEIGHT - SQUARE_SIZE) / 2; // Centraliza verticalmente
const int SPEED = 1;
const int MAX_X = DISPLAY_WIDTH - SQUARE_SIZE;  // Limite direito
const int MAX_Y = DISPLAY_HEIGHT - SQUARE_SIZE; // Limite inferior

// Protótipos de funções
void gpio_callback(uint gpio, uint32_t events);
void leitura_e_controle_joystick(uint slice1, uint slice2);

int main() {
    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400 * 5000);

    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    
    ssd1306_t ssd;
    ssd1306_init(&ssd, DISPLAY_WIDTH, DISPLAY_HEIGHT, false, 0x3c, I2C_PORT);
    ssd1306_config(&ssd);
    ssd1306_send_data(&ssd);

    // Limpa o display
    ssd1306_fill(&ssd, false);
    ssd1306_send_data(&ssd);

    // Inicializa o ADC
    adc_init();
    adc_gpio_init(EIXO_Y);
    adc_gpio_init(EIXO_X);

    // Configura os LEDs
    gpio_set_function(LED2_PIN, GPIO_FUNC_PWM);
    gpio_set_function(LED3_PIN, GPIO_FUNC_PWM);
    gpio_init(LED1_PIN);
    gpio_set_dir(LED1_PIN, GPIO_OUT);
    gpio_put(LED1_PIN, 0);

    // Configura PWM
    uint slice1 = pwm_gpio_to_slice_num(LED2_PIN);
    uint slice2 = pwm_gpio_to_slice_num(LED3_PIN);
    pwm_set_wrap(slice1, PWM_WRAP);
    pwm_set_wrap(slice2, PWM_WRAP);
    pwm_set_clkdiv(slice1, 2.0f);
    pwm_set_clkdiv(slice2, 2.0f);
    pwm_set_enabled(slice1, true);
    pwm_set_enabled(slice2, true);

    // Configura botões
    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON);
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(JOYSTICK_BUTTON, GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_callback(gpio_callback);
    irq_set_enabled(IO_IRQ_BANK0, true);

    while (1) {
        leitura_e_controle_joystick(slice1, slice2);
        cor = !cor;
        
        // Limpa e redesenha o display
        ssd1306_fill(&ssd, false);
        if (borda_style){
            ssd1306_rect(&ssd, 3, 3, 122, 58, cor, !cor);  // Borda
        }
        else{
            ssd1306_rect(&ssd, 3, 3, 122, 58, true, false);  // Borda
        }
        ssd1306_rect(&ssd, pos_y, pos_x, SQUARE_SIZE, SQUARE_SIZE, true, true);  // Quadrado
        ssd1306_send_data(&ssd);
    
        sleep_ms(30);
    }
}

// Funções de callback e controle permanecem as mesmas
void gpio_callback(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();
    int64_t diff = absolute_time_diff_us(last_interrupt_time, now);

    if (diff < 250000) return;
    last_interrupt_time = now;

    if (gpio == BUTTON_PIN) {
        pwm_enabled = !pwm_enabled;
    } else if (gpio == JOYSTICK_BUTTON) {
        green_value = !green_value;
        gpio_put(LED1_PIN, green_value);
        borda_style = !borda_style;

    }
}

void leitura_e_controle_joystick(uint slice1, uint slice2) {
    const uint16_t CENTER = 2047;
    const uint16_t DEADZONE = 170;

    adc_select_input(0);
    uint16_t y_value = adc_read();
    
    adc_select_input(1);
    uint16_t x_value = adc_read();

    int16_t y_diff = (int16_t)y_value - CENTER;
    int16_t x_diff = (int16_t)x_value - CENTER;

    // Corrigindo o movimento no eixo X (movimento horizontal)
    if (abs(x_diff) > DEADZONE) {
        pos_x += (x_diff > 0) ? SPEED : -SPEED;
        pos_x = (pos_x < 0) ? 0 : (pos_x > MAX_X) ? MAX_X : pos_x;
    }
    
    // Corrigindo o movimento no eixo Y (movimento vertical)
    if (abs(y_diff) > DEADZONE) {
        pos_y += (y_diff > 0) ? -SPEED : SPEED;  // Invertido aqui
        pos_y = (pos_y < 0) ? 0 : (pos_y > MAX_Y) ? MAX_Y : pos_y;
    }

    // Mantendo o PWM como estava
    uint16_t pwm_y = (abs(y_diff) <= DEADZONE) ? 0 : abs(y_diff) * 2;
    uint16_t pwm_x = (abs(x_diff) <= DEADZONE) ? 0 : abs(x_diff) * 2;

    if (pwm_enabled) {
        pwm_set_gpio_level(LED2_PIN, pwm_y);
        pwm_set_gpio_level(LED3_PIN, pwm_x);
    }
}