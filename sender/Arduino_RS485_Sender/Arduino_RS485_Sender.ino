#include <SoftwareSerial.h>
#include <SPI.h>
#include <RH_RF95.h>
#include <avr/wdt.h>

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

uint8_t bufs[4][RH_RF95_MAX_MESSAGE_LEN];
uint8_t lens[4];

int lastReceivedIndex = 0;
int lastSentIndex = 0;

void setup() {
  MCUSR = 0;
  wdt_disable();

  Serial.begin(9600);
  RS_Master.begin(9600);
  pinMode(RS_DE_RE, OUTPUT);

  digitalWrite(RS_DE_RE, HIGH);
  //Serial.println("Master ready");

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
    //Serial.println("RFM96W LoRa radio init failed");
    while (1);
  }
  //Serial.println("RFM96W LoRa radio init OK!");

  // Set frequency
  if (!rf95.setFrequency(RF95_FREQ)) {
    //Serial.println("Failed to set frequency");
    while (1);
  }
  //Serial.print("Frequency set to: ");
  //Serial.println(RF95_FREQ);

  // Set receiver gain
  rf95.setTxPower(13, false); // Not used in receiving but good practice
  rf95.setSpreadingFactor(7); // Optional: configure spreading factor

  Serial.println("~");
}

void loop() {
  //Serial.print("RSSI: ");
  //Serial.println(rf95.lastRssi(), DEC);
  rf95.setModeRx();
  //Serial.write('~');
  if (Serial.available()) {
    char mChar = Serial.read();

    if(mChar == '|'){
      //Serial.write('R');
      wdt_enable(WDTO_15MS); // Set watchdog timeout to 15ms
      while (1);
    }
    RS_Master.write(mChar);
    if(mChar == '>'){
      for(uint8_t i=0; i<lens[lastSentIndex]; i++){
        Serial.write((char)bufs[lastSentIndex][i]);
      }
      lastSentIndex++;
      lastSentIndex = lastSentIndex == 3 ? 0 : lastSentIndex;
      Serial.write('~');
    }
  }

  if (rf95.available()) {
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf95.recv(buf, &len)) {
      Serial.write(buf, len);
    }
  }
}
