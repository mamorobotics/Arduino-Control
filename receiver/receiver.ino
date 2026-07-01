#include <Servo.h>
#include <stdio.h>
#include <string.h>
#include <Adafruit_PWMServoDriver.h>
#include <Wire.h>
#include "Arduino.h"
#include <SoftwareSerial.h>

const int RS_RO = 8;
const int RS_DI = 12;
const int RS_DE_RE = 13;
const int String_Count = 16;

Servo foward_left;
Servo foward_right;
Servo up_left;
Servo up_right;

String data[String_Count];

String msg = "";
bool inMsg = false;

SoftwareSerial RS_SLAVE(RS_RO, RS_DI);  //RX, TX

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
bool Up = false;
bool Down = false;
bool Left = false;
bool Right = false;

int motMinPWM = 1220;
int motMaxPWM = 1780;
int serMinPWM = 553;
int serMaxPWM = 2520;
int pumpMinPWM = 1460;
int pumpMaxPWM = 1540;

int FLpin = 12;
int URpin = 14;
int ULpin = 11;
int FRpin = 15;
int ClawLpin = 8;
int ClawRpin = 9;
float MaxClaw = 0.85;
float MinClaw = 0.16;


int baudrate = 31250;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 50 //50  // Analog servos run at ~50 Hz

void setup() {
  Serial.begin(baudrate);
  Serial.println("BOOT");
  RS_SLAVE.begin(baudrate);
  pinMode(RS_DE_RE, OUTPUT);
  digitalWrite(RS_DE_RE, LOW);
  Serial.flush();
  delay(1000);
  Serial.println("READY");

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);  // Set the PWM frequency
  pwm.setPWMFreq(SERVO_FREQ);

  setServosToZero();
}

void loop() {
  if (RS_SLAVE.available()) {
    char c = RS_SLAVE.read();
    if (c == '<') {
      inMsg = true;
    } else if (c == '>') {
      Serial.println(msg);
      splitString(msg, '!', data);
      // Serial.print("[");
      // for (int i = 0; i < String_Count; i++) {
      //   Serial.print(data[i]);
      //   if(i!=String_Count-1){
      //     Serial.print(", ");
      //   }
      // }
      // Serial.println("]");
      assignValues();
      setServosAndMotors();
      msg = "";
      inMsg = false;
    } else if (inMsg) {
      msg += c;
    }
    
  }

  for (int i = 0; i < String_Count; i++) {
    data[i] = "";
  }
}

void assignValues() {
  LeftJoystickX = (data[0].toFloat() / 1000.0);
  LeftJoystickY = (data[1].toFloat() / 1000.0);
  RightJoystickX = (data[2].toFloat() / 1000.0);
  RightJoystickY = (data[3].toFloat() / 1000.0);
  LeftTrigger = (data[4].toFloat() / 100.0);
  RightTrigger = (data[5].toFloat() / 100.0);

  resetButtonBools();

  if ((sizeof(data) / sizeof(data[0]) > 6)) {
    for (int i = 6; i < (sizeof(data) / sizeof(data[0])); i++) {
      if (data[i] == "a") {
        A = true;
      } else if (data[i] == "b") {
        B = true;
      } else if (data[i] == "x") {
        X = true;
      } else if (data[i] == "y") {
        Y = true;
      } else if (data[i] == "j") {
        LeftBumper = true;
      } else if (data[i] == "k") {
        RightBumper = true;
      } else if (data[i] == "u") {
        Up = true;
      } else if (data[i] == "d") {
        Down = true;
      } else if (data[i] == "l") {
        Left = true;
      } else if (data[i] == "r") {
        Right = true;
      }
    }
  }
}

void resetButtonBools() {
  LeftBumper = false;
  RightBumper = false;
  A = false;
  X = false;
  Y = false;
  B = false;
  Up = false;
  Down = false;
  Left = false;
  Right = false;
}

void setServosAndMotors() {
  float fl = constrain((LeftJoystickY + RightJoystickX), -1.0, 1.0);
  float fr = constrain((LeftJoystickY - RightJoystickX), -1.0, 1.0);
  float ul = constrain((RightJoystickY + LeftBumper * 0.1), -1.0, 1.0);
  float ur = constrain((RightJoystickY + RightBumper * 0.1), -1.0, 1.0);
  float pump = A ? 1.0 : B ? -1.0 : 0.01;
  uint16_t testThing = X ? 1600 : 1500;
  float claw = RightTrigger ? MaxClaw : MinClaw;
  
  
  // float fl = A ? 0.2 : 0;
  // float fr = B ? 0.2 : 0;
  // float ul = X ? 0.2 : 0;
  // float ur = Y ? 0.2 : 0;
  

  

  pwm.writeMicroseconds(ClawLpin, positionToPulseServo(claw));
  pwm.writeMicroseconds(ClawRpin, positionToPulseServo(1.0 - claw));

  //pwm.writeMicroseconds(11, testThing);

  pwm.writeMicroseconds(FLpin, positionToPulse(fl, motMaxPWM, motMinPWM));
  pwm.writeMicroseconds(URpin, positionToPulse(ur, motMaxPWM, motMinPWM));
  pwm.writeMicroseconds(ULpin, positionToPulse(ul, motMaxPWM, motMinPWM));
  pwm.writeMicroseconds(FRpin, positionToPulse(fr, motMaxPWM, motMinPWM));

  
  //pwm.writeMicroseconds(4, positionToPulse(pump, motMaxPWM, motMinPWM));
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

uint16_t positionToPulseServo(float position) {
  position = constrain(position, 0.0, 1.0);
  return serMinPWM + (uint16_t)(position * (serMaxPWM - serMinPWM));
}

uint16_t positionToPulse(float position, float max, float min) {
  position = constrain(position, -1.0, 1.0);
  if (position <= 0.01 && position >= 0) {
    position = 0.01;
  }
  if (position >= -0.01 && position < 0) {
    position = -0.01;
  }
  Serial.print(position);
  Serial.print(" ");
  float normalized = (position + 1.0) / 2.0;
  uint16_t micro = min + (uint16_t)(normalized * (max - min));
  Serial.println(micro);
  return micro;
}

void setServosToZero() {
  pwm.writeMicroseconds(FLpin, positionToPulse(0, motMaxPWM, motMinPWM));
  pwm.writeMicroseconds(URpin, positionToPulse(0, motMaxPWM, motMinPWM));
  pwm.writeMicroseconds(ULpin, positionToPulse(0, motMaxPWM, motMinPWM));
  pwm.writeMicroseconds(FRpin, positionToPulse(0, motMaxPWM, motMinPWM));


  pwm.writeMicroseconds(ClawLpin, positionToPulseServo(0.5));
  pwm.writeMicroseconds(ClawRpin, positionToPulseServo(0.5));
  delay(4000);  // Give servos time to settle
  delay(1000);
}