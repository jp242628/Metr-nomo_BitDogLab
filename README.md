# Metrônomo_BitDogLab

Este projeto é um metrônomo digital utilizando a placa Raspberry Pi Pico, um display OLED SSD1306 e um buzzer. O metrônomo permite ajustar o BPM (batidas por minuto) e exibe o valor atual no display OLED. Além disso, ele emite sons no buzzer de acordo com o BPM configurado.

## Componentes Utilizados

- Raspberry Pi Pico
- Display OLED SSD1306
- Buzzer
- Botões (Botão A, Botão B e Joystick)

## Conexões

- **Buzzer**: GPIO 21
- **Botão A**: GPIO 5
- **Botão B**: GPIO 6
- **Joystick Button**: GPIO 22
- **I2C SDA**: GPIO 14
- **I2C SCL**: GPIO 15

## Funcionalidades

- Ajuste do BPM utilizando os botões A e B
- Exibição do BPM atual no display OLED
- Emissão de som no buzzer de acordo com o BPM configurado
- Alternância entre ligado e desligado utilizando o botão do joystick

## Compilação e Execução

Clone o repositório:

```sh
git clone https://github.com/jp242628/Metronomo_BitDogLab.git
cd Metronomo_BitDogLab
```

Configure o ambiente, compile e carregue o código no Raspberry Pi Pico utilizando o ambiente de desenvolvimento de sua preferência.

## Código Principal

O código principal está localizado no arquivo `u7project.c`. Ele contém a lógica para leitura dos botões, ajuste do BPM, controle do display OLED e emissão de som no buzzer.