#include <Servo.h>
#include <stdio.h>
#include <string.h>

int data[16];

Servo mU;
Servo mD;
Servo mL;
Servo mR;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mU.attach(0);
  mD.attach(1);
  mL.attach(2);
  mR.attach(3);
}

void loop() {
  int i=0;
  if(Serial.available() > 0){
    const char* dataS = Serial.read();

    char destination[100];
    strcpy(destination, dataS);

    char *token = strtok(destination, "!");

    while (token != NULL)
    {
        data[i] = token;
        token = strtok(NULL, "!");
        i++;
    }
    i=0;
  }
}
