#include <Servo.h>
#include <stdio.h>
#include <string.h>
#include "Arduino.h"
#include <SoftwareSerial.h>;

const int RS_RO = 10;
const int RS_DI = 11;
const int RS_DE_RE = 12;

String data[16];

SoftwareSerial RS_SLAVE(RS_RO, RS_DI); //RX, TX

void setup() {
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
    Serial.println(RS_SLAVE.read());
    //splitString((String)RS_SLAVE.read(), '!', data);
    // for (int i = 0; i < 16; i++) {
    //   Serial.println(*ata[i]);
    // }
  }
}

void splitString(String str, char delimiter, String *result) {
  int startIndex = 0;
  int endIndex = 0;
  int partIndex = 0;

  while (endIndex >= 0 && partIndex < 3) {
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