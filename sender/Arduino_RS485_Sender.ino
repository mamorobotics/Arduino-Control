#include <SoftwareSerial.h>
#include <SPI.h>
#include <RH_RF95.h>

const int baudrate = 9600;

const int RS_RO = 5;
const int RS_DI = 3;
const int RS_DE_RE = 4;

SoftwareSerial RS_Master(RS_RO, RS_DI); // RX, TX

// Define pins for the RFM96W
#define RFM95_CS 10   // Chip select
#define RFM95_RST 9   // Reset
#define RFM95_INT 2   // Interrupt

// Define frequency (433 MHz)
#define RF95_FREQ 433.0

// Create an instance of the driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

bool firstController = true;

void setup() {
  Serial.begin(9600);
  RS_Master.begin(9600);
  pinMode(RS_DE_RE, OUTPUT);

  digitalWrite(RS_DE_RE, HIGH);
  Serial.println("Master ready");

    // Reset the module
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);

  // Initialize the radio
  if (!rf95.init()) {
    Serial.println("RFM96W LoRa radio init failed");
    while (1);
  }
  Serial.println("RFM96W LoRa radio init OK!");

  // Set frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    Serial.println("Failed to set frequency");
    while (1);
  }
  Serial.print("Frequency set to: ");
  Serial.println(RF95_FREQ);

  // Set receiver gain
  rf95.setTxPower(13, false); // Not used in receiving but good practice
  rf95.setSpreadingFactor(7); // Optional: configure spreading factor

  Serial.println("~");
}

void loop() {
  // put your main code here, to run repeatedly:

  if (Serial.available()) {
    RS_Master.write(Serial.read());
    //Serial.write('~');

    // if(firstController){
    //   uint8_t data[] = "S";
    //   rf95.send(data, sizeof(data));
    //   rf95.waitPacketSent();
    //   firstController = false;
    //   delay(1000);
    // }

    // if (rf95.available()) {
    //   // Buffer to hold received data
    //   uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    //   uint8_t len = sizeof(buf);

    //   // Check if data was received
    //   if (rf95.recv(buf, &len)) {
    //     //Serial.println("Message received!");
    //     //Serial.print("RSSI: ");
    //     //Serial.println(rf95.lastRssi(), DEC);

    //     // Print the received message
    //     //Serial.print("Message: ");
    //     Serial.write(buf, sizeof(buf));
    //     Serial.write('~');
    //     // for (uint8_t i = 0; i < len; i++) {
    //     //   Serial.print((char)buf[i]);
    //     // }
    //     //Serial.println();
    //   } else {
    //     Serial.println("Receive failed");
    //   }
    // }
    // else{
    //   Serial.write('~');
    // }
  }
  //delay(50);
}
