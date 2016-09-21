#include "MultiOutSerial.h"

uint8_t txs[4] = {2, 3, 4, 5};
uint8_t data[4] = {'2', '3', '4', '5'};

MultiOutSerial ser(txs, false);

void setup() {
  // put your setup code here, to run once:
  ser.begin(100000, 2, 1);
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("loop");
  delay(1000);
  ser.write(txs);
}
