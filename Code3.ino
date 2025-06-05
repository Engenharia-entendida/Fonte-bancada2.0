#include "EncoderTool.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PwmPin 6
#define CLK 2
#define DT 1
#define Button 3
#define Relay 7
#define VoltOutPin 26
#define pinACS712 27
//=====Variáveis======//
bool StatusButton = true;
long oldPosition = 0;
float PwmValue = 0;
float volts = 0;
float ADJCurrent = 0;
float VoltOut = 0;
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
EncoderTool::PolledEncoder encoder;

void setup() {
    Serial.begin (9600);
    Wire.begin();
    display.begin (0x3C); 
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(16, 0);
    display.print("ZPS V2.0");
    display.display();
    delay (2000);
}
void setup1(){
    encoder.begin(CLK,DT,Button);
    pinMode(Button, INPUT_PULLDOWN);
    pinMode(PwmPin, OUTPUT);
    pinMode(Relay, OUTPUT);
    digitalWrite (Relay,HIGH);
}

void loop() {
    for(int i=0;i<500;i++){
        VoltOut += analogRead(VoltOutPin);
        delayMicroseconds (400);
    }
    VoltOut=VoltOut/500;
    for (int j=0;j<500; j++){
        ADJCurrent+=analogRead(pinACS712);
        delayMicroseconds(400);
    }
    ADJCurrent = ADJCurrent/500;
ADJCurrent=(ADJCurrent-790)*(5.00-0.00)/(1550-790) +0.00;
if(ADJCurrent<0) ADJCurrent=0;
VoltOut = (VoltOut - 64.46) * (34.1 - 1.39)/(900.4 - 64.46) + 1.39;
volts=VoltOut;
//==============================Print==================================//
display.clearDisplay();
display.setCursor (0, 10);
display.print("ADJ:"+ String(volts, 2) + " V");
display.setCursor (0, 30);
display.print("ADJ:"+ String(ADJCurrent, 2) + " A");
display.setCursor(0, 50);
display.print("+5:"+ String(StatusButton) + " A");
display.setCursor (0, 70);
display.print("-5:"+ String(StatusButton) + " A");
display.setCursor (0,110);
if(StatusButton==0) display.print ("OUT: ON");
else display. print ("OUT: OFF"); display.display();
}
//==============================Loop1==================================//
void loop1(){
    encoder.tick();
    if (encoder.valueChanged()){
        PwmValue = encoder.getValue ();
        PwmValue = constrain(PwmValue, 0, 230);
    }
analogWrite(PwmPin, PwmValue);
    if(encoder.buttonChanged()){
        if (encoder.getButton()==HIGH) StatusButton=!StatusButton;
        digitalWrite(Relay, StatusButton);
    }
}