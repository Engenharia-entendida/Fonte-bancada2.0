#include "EncoderTool.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <ResponsiveAnalogRead.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PwmPin 14
#define CLK 2
#define DT 1
#define Button 3
//==============================Variáveis================================//
long oldPosition  = 0;                                                           //Variável para posição encoder
float PwmValue = 0;                                                              //Variável para PWM
float volts = 0;                                                                 //Variável para Volts
float lastvolts = -1;                                                            //Variável para atualização de volts
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);   //Cria o objeto do display
EncoderTool::PolledEncoder encoder;                                              //Criar o objeto enconder

//=================================Setup==================================//
void setup() {
  Serial.begin(9600);                     //Serial
  encoder.begin(CLK,DT,Button);           //Inicia encoder
  pinMode(PwmPin,OUTPUT);                 //Pino PWM como saída
  Wire.begin();                           //Inicia o I2C
  display.begin(0x3C);                    //Inicia o display
  display.clearDisplay();                 //Limpa o display
  display.setTextSize(2);                 //Define o tamanho do texto
  display.setTextColor(SH110X_WHITE);     //Define a cor do display
  display.setCursor(16, 0);               //Seta o cursor
  display.print("ZPS V2.0");              //Print do modelo
  display.display();                      //Imprime o modelo
  delay(2000);                            //Delay inicial
}
//==================================Loop==================================//
void loop() {
  encoder.tick();                                       //Leitura do encoder
  if (encoder.valueChanged()){                          //Se o valor mudar
      PwmValue = encoder.getValue();                    //Pega o novo valor e armazena em PwmValue
      if(PwmValue<0) PwmValue = 0;                      //Limita o PWM em 0
      if(PwmValue>255) PwmValue = 255;                  //Limita o PWM em 255
    }
  //if (encoder.buttonChanged())           

  analogWrite(PwmPin,PwmValue);                           //Escreve o valor de PwmValue no Pino PWM
  volts = ((PwmValue / 255.0) * (33.9 - 2.96)) + 2.96;    //Mapeia o valor do PWM para tensão 

//=================================Print==================================//
  if(volts != lastvolts){                               //Se houver uma mudança no último valor...
  display.clearDisplay();                               //Limpa o display
  display.setCursor(0, 10);                             //Seta o cursor
  display.print("ADJ:"+ String(volts, 2) + "V");        //Print do modelo   
  display.display();                                    //Imprime o modelo
  Serial.println(String(volts, 2) + " V");              //Imprime Volts com duas casas
  lastvolts = volts;                                    //Atualiza Volts
  }
}
