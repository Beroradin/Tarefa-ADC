# ✨ Tarefa08-Conversor ADC

<p align="center"> Repositório dedicado a Tarefa 08 do processo de capacitação do EmbarcaTech que envolve conversores ADC na placa Raspberry Pi Pico W por meio da Plataforma BitDogLab.</p>

## :clipboard: Apresentação da tarefa

Para consolidar a compreensão dos conceitos relacionados ao uso do periférico conversor ADC no microcontrolador RP2040 e explorar as funcionalidades da placa de desenvolvimento BitDogLab, esta atividade propõe a implementação de um projeto prático envolvendo a utilização do joystick (ligado ao canal 1 e 2 do ADC) para o controle do LED RGB como também de um quadrado 8x8 no display OLED. O projeto também prevê a utilização do botão A (GPIO 5) como também o botão do joystick (GPIO 22), um trava/destrava o valor PWM atual dos LEDs enquanto o outro controla o estado lógico do LED verde e da borda do display OLED.

## :dart: Objetivos

- Compreender o funcionamento e a aplicação de conversores analógico-digital em microcontroladores;

- Compreender o funcionamento e a aplicação de modulação por largura de pulso (PWM) em microcontroladores;

- Compreender o funcionamento e a aplicação de interrupções em microcontroladores;

- Implementar a técnica de debouncing via software para eliminar o efeito de bouncing em botões;

- Controlar o display OLED e o LED RGB por meio do valor ADC do Joystick;

- Manipular o display OLED por meio do botão do Joystick;

- Permitir habilitar/desabilitar o valor do LED RGB por meio do botão na GPIO 5.

## :books: Descrição do Projeto

Utiizou-se a placa BitDogLab (que possui o microcontrolador RP2040) para a exibição no display OLED ssd1306 um quadrado 8x8 que é controlado por meio do conversor ADC ligado ao joystick. O LED RGB também é controlado pelo ADC, mas passa pelo periférico PWM para seu eventual acionamento e controle efetivo. Os botões também controlam aspectos (flags) do projeto.

## :walking: Integrantes do Projeto

- Matheus Pereira Alves

## :bookmark_tabs: Funcionamento do Projeto

- O display OLED possui um quadrado 8x8 que é controlado pelo joystick (conversor analógico-digital nos eixos X e Y);
- O LED RGB (cores vermelhas e azuis) são controladas pela iteração ADC-PWM, tendo seus máximos nos extremos (valor wrap 0 - 4095);
- O LED RGB verde é ligado por meio do botão do joystick (GPIO 22), que também aciona uma borda variável;
- O PWM possui uma verificação de deadzone, que é uma área em qual o valor do pwm é setado para 0 e é próximo do valor adc de 2047;
- Houve a implementação de debounce e interrupções para os botões.

## :camera: GIF mostrando o funcionamento do programa na placa BitDogLab
<p align="center">
  <img src=".github/adc.gif" alt="GIF" width="523px" />
</p>

## :arrow_forward: Vídeo no youtube mostrando o funcionamento do programa na placa BitDogLab

<p align="center">
    <a href="https://youtube.com/shorts/d9alYMcvrqM">Clique aqui para acessar o vídeo</a>
</p>
