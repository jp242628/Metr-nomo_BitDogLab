#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/i2c.h" // Para o barramento I2C
#include <stdio.h>
#include <string.h>       // Para manipulação de strings
#include "inc/ssd1306.h"  // Para o display OLED

// Definir os pinos
#define BUZZER_PIN 21          // Pino do buzzer (GPIO 21)
#define BUTTON_A_PIN 5         // Pino do Botão A (GPIO 5)
#define BUTTON_B_PIN 6         // Pino do Botão B (GPIO 6)
#define JOYSTICK_BUTTON 22     // Pino do Botão do Joystick (GPIO 22)
#define I2C_PORT i2c1          // Barramento I2C
#define I2C_SDA 14             // Pinos para o barramento I2C (SDA)
#define I2C_SCL 15             // Pinos para o barramento I2C (SCL)
#define ENDERECO 0x3C          // Endereço do display OLED

// Variáveis globais
int bpm = 60;                  // Batidas por minuto
int interval;                  // Intervalo de tempo em milissegundos
bool running = true;           // Estado do metrônomo (ligado/desligado)
ssd1306_t display;             // Variável global para o display

// Função para calcular o intervalo de tempo em milissegundos
void calcular_intervalo() {
    interval = 60000 / bpm;     // Converte BPM para milissegundos
}

// Função para limpar o display OLED
void ssd1306_clear(ssd1306_t *ssd) {
    ssd1306_fill(ssd, false);
    ssd1306_show(ssd);
}

// Função para exibir o BPM no display OLED
void exibir_bpm() {
    char buffer[16];
    snprintf(buffer, sizeof(buffer), "BPM %d", bpm);

    // Limpa o display
    ssd1306_clear(&display);

    // Desenha a borda
    ssd1306_rect(&display, 0, 0, WIDTH, HEIGHT, true, false); // Borda externa
    ssd1306_rect(&display, 2, 2, WIDTH - 4, HEIGHT - 4, true, false); // Borda interna

    // Exibe o texto "BPM %d" centralizado
    int text_width = strlen(buffer) * 8; // Largura aproximada do texto (8 pixels por caractere)
    int x_position = (WIDTH - text_width) / 2;
    int y_position = (HEIGHT - 8) / 2;   // Altura aproximada do texto (8 pixels)
    ssd1306_draw_string(&display, buffer, x_position, y_position);

    // Atualiza o display
    ssd1306_show(&display);
}

// Função para gerar um som personalizado
void tocar_padrao_per() {
    if (!running) return; // Não toca se o metrônomo estiver desligado

    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_wrap(slice_num, 12500); // Frequência base: 125 MHz / 12500 = 10 kHz
    pwm_set_enabled(slice_num, true);

    // Sequência de tons para o padrão personalizado
    int frequencias[] = {440, 523, 659, 784}; // Frequências das notas musicais (Lá, Dó, Mi, Sol)
    int duracao_som = 100;                   // Duração de cada toque em milissegundos
    int duracao_silencio = 50;               // Duração do silêncio entre os toques em milissegundos

    for (int i = 0; i < 4; i++) { // Repete 4 vezes para cada nota
        // Configura a frequência do som
        pwm_set_wrap(slice_num, 125000000 / frequencias[i]);
        pwm_set_gpio_level(BUZZER_PIN, 6250); // 50% de duty cycle (volume médio)

        sleep_ms(duracao_som); // Mantém o som por 100 ms

        // Desliga o som
        pwm_set_gpio_level(BUZZER_PIN, 0);

        sleep_ms(duracao_silencio); // Aguarda 50 ms de silêncio
    }
}

// Função para configurar os pinos e periféricos
void setup() {
    stdio_init_all();

    // Configuração dos botões
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN); // Ativa resistor pull-up

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN); // Ativa resistor pull-up

    gpio_init(JOYSTICK_BUTTON);
    gpio_set_dir(JOYSTICK_BUTTON, GPIO_IN);
    gpio_pull_up(JOYSTICK_BUTTON); // Ativa resistor pull-up

    // Configuração do buzzer
    gpio_set_function(BUZZER_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(BUZZER_PIN);
    pwm_set_wrap(slice_num, 12500); // Frequência base: 125 MHz / 12500 = 10 kHz
    pwm_set_enabled(slice_num, true);

    // Configuração do I2C para o display OLED
    i2c_init(I2C_PORT, 400 * 1000); // Inicializa o I2C a 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    ssd1306_init(&display, 128, 64, false, ENDERECO, I2C_PORT); // Inicializa o display OLED
    ssd1306_config(&display); // Configura o display OLED

    // Calcula o intervalo inicial
    calcular_intervalo();
    exibir_bpm();
}

// Função para verificar os botões
void verificar_botoes() {
    if (!gpio_get(BUTTON_A_PIN)) { // Botão A pressionado
        bpm += 5;
        if (bpm > 240) bpm = 240; // Limite superior
        calcular_intervalo();
        exibir_bpm();
        sleep_ms(200); // Debounce
    }

    if (!gpio_get(BUTTON_B_PIN)) { // Botão B pressionado
        bpm -= 5;
        if (bpm < 30) bpm = 30; // Limite inferior
        calcular_intervalo();
        exibir_bpm();
        sleep_ms(200); // Debounce
    }

    if (!gpio_get(JOYSTICK_BUTTON)) { // Botão do Joystick pressionado
        running = !running; // Alterna entre ligado e desligado
        if (!running) {
            ssd1306_clear(&display);
            ssd1306_draw_string(&display, "Desligando", 16, 28);
            ssd1306_show(&display);
            sleep_ms(1000); // Aguarda 1 segundo antes de desligar
        }
        exibir_bpm();
        sleep_ms(200); // Debounce
    }
}

int main() {
    setup(); // Configuração inicial

    while (1) {
        verificar_botoes(); // Verifica os botões
        if (running) {
            tocar_padrao_per(); // Toca o padrão
            sleep_ms(interval - 400); // Intervalo entre os padrões
        }
    }
}