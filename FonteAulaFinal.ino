#include "EncoderTool.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "Adafruit_ADS1X15.h"
#include <PID_v1.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 128
#define PwmPin 6
#define CLK 2
#define CLK2 12
#define DT 1
#define DT2 11
#define Button 3
#define Button2 13
#define Relay 7
#define VoltOutPin 26
#define pinACS712 27
//==============================Variables==================================//
double Setpoint, Input, Output;
double Kp = 0.04, Ki = 2.5, Kd = 0.06; // ajuste fino pode ser feito depois
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

bool StatusButton = true;
bool Pisca=true;
bool Toggle = true;
long oldPosition = 0;
float ADJCurrent = 0;
float ReadCurrent = 0.000;
float SetCurrent = 0.000;
float ReadCurrentADJ = 0.00;
float VoltOut = 0;
volatile int16_t adc0=0;
volatile int16_t adc1=0;
Adafruit_SH1107 display = Adafruit_SH1107(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);
EncoderTool::PolledEncoder encoder;
EncoderTool::PolledEncoder encoder2;
Adafruit_ADS1115 ads1115;
//================================Setup====================================//
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
    ads1115.begin();
    delay (2000);
}
//================================Setup1===================================//
void setup1(){
    encoder.begin(CLK,DT,Button);
    encoder2.begin(CLK2,DT2,Button2);
    pinMode(Button, INPUT_PULLUP);
    pinMode(Button2, INPUT_PULLUP);
    pinMode(PwmPin, OUTPUT);
    
    myPID.SetOutputLimits(0, 255);pinMode(Relay, OUTPUT);
    digitalWrite (Relay,HIGH);
    myPID.SetMode(AUTOMATIC);
} 
//=================================Loop====================================//
void loop() {
adc1 = ads1115.readADC_SingleEnded(1);
adc0 = ads1115.readADC_SingleEnded(0);
ADJCurrent = adc1;
ADJCurrent=(ADJCurrent-12800) * (2.00-0.00)/(15900-12800) + 0.00; 
if(ADJCurrent<0 || StatusButton==true) ADJCurrent=0;
ReadCurrentADJ=ADJCurrent;
display.clearDisplay();
if(Toggle==true){
    if(Pisca==true){
        display.setCursor (0, 10);
        display.print("SET:"+ String(ReadCurrent, 2) + "A");
    }
    else {
        display.setCursor (0, 10);
        display.print("");
    }
}
else{
    display.setCursor (0, 10);
    display.print("SET:"+ String(ReadCurrent, 2) + "A");
}
VoltOut=(Setpoint-0)*(34.2-2.2)/(255-0)+2.2;
display.setCursor (0, 30);
display.print("ADJ:"+ String(VoltOut, 2) + "V");
display.setCursor(0, 50);
display.print("ADJ:"+ String(ADJCurrent, 2) + "A");
display.setCursor(0, 70);
display.print("+5:"+ String(StatusButton) + "A");
display.setCursor (0, 90);
display.print("-5:"+ String(StatusButton) + "A");
display.setCursor (0,110);
if(StatusButton==0) display.print ("OUT: ON");
else display.print ("OUT: OFF");
display.display();
Pisca=!Pisca;
}
//=================================Loop====================================//
void loop1(){   
encoder.tick();
    if (encoder.valueChanged()){
        Setpoint = encoder.getValue();
        Setpoint = constrain(Setpoint, 0, 255);
    }
if(StatusButton==false){
    Input=adc0;
    Input=(Input-800)*(255-0)/(12500-800)+0;
    myPID.Compute();
    analogWrite(PwmPin, (int)Output);
    
}
if(encoder.buttonChanged()){
    if (encoder.getButton()==LOW) StatusButton=!StatusButton;
        digitalWrite(Relay, StatusButton);
}
encoder2.tick();
    if (encoder2.valueChanged()){
        ReadCurrent = encoder2.getValue();
        ReadCurrent = constrain(ReadCurrent, 0, 250);
        ReadCurrent = (ReadCurrent - 0) * (2.00 - 0.0)/(250 - 0) + 0;
    }
     if(encoder2.buttonChanged()){
        if (encoder2.getButton()==LOW){ SetCurrent = ReadCurrent;
        Toggle=!Toggle;
        }
    }
    if(ReadCurrentADJ>SetCurrent){ 
        digitalWrite(Relay,HIGH);
        StatusButton=true;
    }
}
