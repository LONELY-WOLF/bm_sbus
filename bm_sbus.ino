#include "MultiOutSerial.h"

uint8_t txs[4] = {2, 3, 4, 5};
uint8_t data[4] = {0x00, 0x82, 0x55, 0xAA};

MultiOutSerial ser(txs, false);

void setup() {
  // put your setup code here, to run once:
  ser.begin(100000, PARITY_NONE, 2);
  Serial.begin(100000);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println("loop");
  //Serial.write(0x55);
  //delay(1000);
  ser.write(data);
}
