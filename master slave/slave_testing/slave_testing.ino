#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>

#define SDA 18
#define SCL 19
#define I2C_SLAVE_ADDR 0x04

int duty1;
int duty2;

void receiveEvent(int howMany);

void setup() {
  Serial.begin(115200);
  WireSlave.begin(SDA, SCL, I2C_SLAVE_ADDR);
  WireSlave.onReceive(receiveEvent);
}

void loop() {
  WireSlave.update();
  delay(1);
}

void receiveEvent(int howMany){
  duty1 = WireSlave.read();
  duty2 = WireSlave.read();
  
  Serial.println(duty1);
  Serial.println(duty2);
}
