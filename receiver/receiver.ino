#include <Servo.h>
#include <stdio.h>
#include <string.h>
#include "Arduino.h"
#include <SoftwareSerial.h>;

const int RS_RO = 10;
const int RS_DI = 11;
const int RS_DE_RE = 12;
const int String_Count = 16;

String data[String_Count];

String msg = "";
bool inMsg = false;

SoftwareSerial RS_SLAVE(RS_RO, RS_DI); //RX, TX

float LeftJoystickY = 0.0;
float LeftJoystickX = 0.0;
float RightJoystickY = 0.0;
float RightJoystickX = 0.0;
float LeftTrigger = 0.0;
float RightTrigger = 0.0;
bool LeftBumper = false;
bool RightBumper = false;
bool A = false;
bool X = false;
bool Y = false;
bool B = false;
bool DpadY = false;
bool DpadX = false;

void setup() {
  //https://learn.adafruit.com/16-channel-pwm-servo-driver/hooking-it-up
  pwm.setPWMFreq(400);

  Serial.begin(9600);
  RS_SLAVE.begin(9600);
  pinMode(RS_DE_RE, OUTPUT);
  digitalWrite(RS_DE_RE, LOW);
  Serial.flush();
  delay(1000);
  Serial.println("READY");
}

void loop() {
  if(RS_SLAVE.available()){
    char c = RS_SLAVE.read();
    if(c == '<'){
      inMsg = true; 
    } else if(c == '>'){
      //Serial.println(msg);
      splitString(msg, '!', data);
      Serial.print("[");
      for (int i = 0; i < String_Count; i++) {
        Serial.print(data[i]);
        if(i!=String_Count-1){
          Serial.print(", ");
        }
        assignValues();
        setServosAndMotors();
      }
      Serial.println("]");
      msg = "";
      inMsg = false;
    } else if(inMsg){
      msg+=c;
    }
  }
}

void assignValues(){
  LeftJoystickX = (data[0].toInt()/1000);
  LeftJoystickY = (data[1].toInt()/1000);
  RightJoystickX = (data[2].toInt()/1000);
  RightJoystickY = (data[3].toInt()/1000);
  LeftTrigger = (data[4].toInt()/100);
  RightTrigger = (data[5].toInt()/100);

  if((sizeof(data) / sizeof(data[0])>6)){
    for (int i = 6; i < (sizeof(data) / sizeof(data[0])); i++) {
      if(data[i]=="a"){
        A = true;
      } else if(data[i]=="b"){
        B = true;
      } else if(data[i]=="x"){
        X = true;
      } else if(data[i]=="y"){
        Y = true;
      } else if(data[i]=="j"){
        LeftBumber = true;
      } else if(data[i]=="k"){
        RightBumber = true;
      } else if(data[i]=="u"){
        Up = true;
      } else if(data[i]=="d"){
        Down = true;
      } else if(data[i]=="l"){
        Left = true;
      } else if(data[i]=="r"){
        Right = true;
      }
    }
  }
}

void setServosAndMotors(){

}

void splitString(String str, char delimiter, String *result) {
  int startIndex = 0;
  int endIndex = 0;
  int partIndex = 0;

  while (endIndex >= 0 && partIndex < String_Count) {
    endIndex = str.indexOf(delimiter, startIndex);
    
    if (endIndex == -1) {  // If no more delimiters, grab the rest of the string
      result[partIndex] = str.substring(startIndex);
    } else {
      result[partIndex] = str.substring(startIndex, endIndex);
      startIndex = endIndex + 1;
    }
    
    partIndex++;
  }
}

void setServoPos(float speed, int channel) {
  float normalizedPos = (speed + 1.0) / 2.0;
  float percent = (normalizedPos + 0.40) * 0.80;
  int ticks = (int)(percent * 4095);
  pwm.setPWM(channel, ticks, 4095-ticks);
}

void setMotorSpeed(float speed, int channel) {
  float normalizedSpeed = (speed + 1.0) / 2.0;
  float percent = (normalizedSpeed + 0.44) * 0.76;
  int ticks = (int)(percent * 4095);
  pwm.setPWM(channel, ticks, 4095-ticks);
}