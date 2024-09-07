#include <SoftwareSerial.h>

const int baudrate = 9600;

const int RS_RO = 10;
const int RS_DI = 11;
const int RS_DE_RE = 12;

SoftwareSerial RS_Master(RS_RO, RS_DI); // RX, TX


void setup() {
  // put your setup code here, to run once:

Serial.begin(9600);
RS_Master.begin(9600);
pinMode(RS_DE_RE, OUTPUT);

digitalWrite(RS_DE_RE, HIGH);
Serial.println("Master ready");

}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available()) {
    RS_Master.write(Serial.read());
  }

}
