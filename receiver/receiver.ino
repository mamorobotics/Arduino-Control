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
int serMinPWM = 184;
int serMaxPWM = 430;

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVO_FREQ 400 //50  // Analog servos run at ~50 Hz

void setup() {
  Serial.begin(9600);
  Serial.println("BOOT");
  RS_SLAVE.begin(9600);
  pinMode(RS_DE_RE, OUTPUT);
  digitalWrite(RS_DE_RE, LOW);
  Serial.flush();
  delay(1000);
  Serial.println("READY");

  pwm.begin();
  // pwm.setOscillatorFrequency(27000000);  // Set the PWM frequency
  pwm.setPWMFreq(SERVO_FREQ);

  setServosToZero();
}

void loop() {
  if (RS_SLAVE.available()) {
    char c = RS_SLAVE.read();
    if (c == '<') {
      inMsg = true;
    } else if (c == '>') {
      //Serial.println(msg);
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
  float fl = constrain((LeftJoystickY + LeftJoystickX), -1.0, 1.0);
  float fr = constrain((LeftJoystickY - LeftJoystickX), -1.0, 1.0);
  float ul = RightJoystickY;
  float ur = RightJoystickY;
  //fl=1.0;

  // positionToPulseMotor(ur);
  // positionToPulseMotor(ul);
  // positionToPulseMotor(fr);
  // positionToPulseMotor(fl);
  pwm.writeMicroseconds(0, positionToPulseMotor(fl));
  pwm.writeMicroseconds(1, positionToPulseMotor(ur));
  pwm.writeMicroseconds(2, positionToPulseMotor(ul));
  pwm.writeMicroseconds(3, positionToPulseMotor(fr));
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

uint16_t positionToPulseMotor(float position) {
  position = constrain(position, -1.0, 1.0);
  Serial.print(position);
  Serial.print(" ");
  float normalized = (position + 1.0) / 2.0;
  uint16_t micro = motMinPWM + (uint16_t)(normalized * (motMaxPWM - motMinPWM));
  Serial.println(micro);
  return micro;
}

void setServosToZero() {
  pwm.writeMicroseconds(0, positionToPulseMotor(0));
  pwm.writeMicroseconds(1, positionToPulseMotor(0));
  pwm.writeMicroseconds(2, positionToPulseMotor(0));
  pwm.writeMicroseconds(3, positionToPulseMotor(0));
  delay(100);  // Give servos time to settle
}