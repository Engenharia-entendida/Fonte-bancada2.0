#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PwmPin 14
const byte rows = 4;                           // Número de linhas do teclado
const byte cols = 3;                           // Número de colunas do teclado
byte rowPins[rows] = {26, 27, 28, 29};         // Pinos das linhas conectados ao teclado
byte colPins[cols] = {6, 7, 8};                // Pinos das colunas conectados ao teclado
char keyList[rows][cols] = {
  {'1', '2', '3'},          // Primeira linha: 1, 2, 3
  {'4', '5', '6'},          // Segunda linha: 4, 5, 6
  {'7', '8', '9'},          // Terceira linha: 7, 8, 9
  {'*', '0', '#'}           // Quarta linha: *, 0, #
};

String inputValue = "0";                                                         // Armazena os valores digitados
int pwmValue = 0;                                                                // Valor PWM (0-255)
float volts = 0;                                                                 // Variável para Volts
float lastvolts = -1;                                                            // Variável para atualização de volts
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);   // Cria o objeto do display

void setup() {
    Serial.begin(9600);
    Wire.begin();                         // Inicia o I2C
    display.begin(0x3C);                  // Inicia o display
    display.clearDisplay();               // Limpa o display
    display.setTextSize(2);               // Define o tamanho do texto
    display.setTextColor(SH110X_WHITE);   // Define a cor do display
    display.setCursor(16, 0);             // Seta o cursor
    display.print("ZPS V2.0");            // Print do modelo
    display.display();                    // Imprime o modelo
    delay(2000);                          // Delay inicial
    pinMode(PwmPin, OUTPUT);              // Define o pino 14 como saída para PWM
                       
    //===================Configura Pinos do Teclado================================//
    pinMode(26, OUTPUT);              // Define o pino da linha como saída
    pinMode(27, OUTPUT);              // Define o pino da linha como saída
    pinMode(28, OUTPUT);              // Define o pino da linha como saída
    pinMode(29, OUTPUT);              // Define o pino da linha como saída
    pinMode(6, INPUT_PULLUP);         // Define o pino da coluna como entrada com pull-up
    pinMode(7, INPUT_PULLUP);         // Define o pino da coluna como entrada com pull-up
    pinMode(8, INPUT_PULLUP);         // Define o pino da coluna como entrada com pull-up
    display.clearDisplay();           // Limpa o display
    display.setCursor(0, 10);         // Seta o cursor
    display.print("Volts");           // Envia as informações   
    display.setCursor(0, 40);         // Seta o cursor
    display.print("3V ~ 34V");        // Envia as informações    
    display.display();                // Imprime o valor 
    analogWrite(PwmPin, 0);           // Aplica o valor PWM no pino 14
  } 
     
void loop() {
  char key = readKeypad();                                            // Lê a tecla pressionada no teclado
  if (key != '\0') {                                                  // Se uma tecla foi pressionada
    delay(250);                                                       // Delay para evitar múltiplas leituras de uma tecla pressionada
    if (key == '#') {                                                 // Se a tecla # for pressionada, aplica o valor PWM
      if (inputValue != "") {                                         // Se houver algo digitado
        inputValue.replace('*', '.');                                 // Substitui * por . (vírgula para ponto decimal)
        pwmValue = map(inputValue.toFloat(), 3.12, 33.9, 0, 255);     // Mapeia o valor digitado para PWM (0-255)
        pwmValue = constrain(pwmValue, 0, 255);                       // Constrói o valor PWM dentro do intervalo de 0 a 255
        analogWrite(PwmPin, pwmValue);                                // Aplica o valor PWM no pino 14
      }
      inputValue = "";                                                // Limpa o valor digitado após pressionar #
    } else if (key == '*') {                                          // Se a tecla * for pressionada, insere ponto como vírgula
      inputValue += '.';                                              // Adiciona ponto no lugar de vírgula
      display.clearDisplay();                                         // Limpa o display
      display.setCursor(0, 10);                                       // Seta o cursor
      display.print("ADJ:"+ inputValue  + "V");                       // Envia as informações    
      display.display();                                              // Exibe o valor digitado até o momento
      delay(50);
    } else {                                                          // Se for um número pressionado, adiciona ao valor digitado
      inputValue += key;                                              // Adiciona a tecla ao valor digitado
      display.clearDisplay();                                         // Limpa o display
      display.setCursor(0, 10);                                       // Seta o cursor
      display.print("ADJ:"+ inputValue  + "V");                       // Envia as informações    
      display.display();                                              // Exibe o valor digitado até o momento
      delay(50);
    }
  }
}
char readKeypad() {                                                   // Função para ler o teclado matricial
  delay(100);                                                         // Pequeno delay para estabilizar os sinais
  for (byte i = 0; i < rows; i++) {                                   // Percorre as linhas
    digitalWrite(rowPins[i], LOW);                                    // Ativa a linha atual (coloca em estado baixo)
    for (byte j = 0; j < cols; j++) {                                 // Percorre as colunas
      if (digitalRead(colPins[j]) == LOW) {                           // Se a tecla na linha i e coluna j for pressionada
          digitalWrite(rowPins[i], HIGH);                             // Restaura a linha para o estado alto
          return keyList[i][j];                                       // Retorna a tecla pressionada
      }
    }
    digitalWrite(rowPins[i], HIGH);                                   // Restaura a linha para o estado alto
  }
  return '\0';                                                        // Se nenhuma tecla foi pressionada, retorna nulo
}
