#include <Wire.h>
#include <Arduino.h>
#include <WirePacker.h>



byte duty1 = 80;
byte duty2 = 100;

#define SDA 14
#define SCL 15
#define I2C_SLAVE_ADDR 0x04

void setup() {
  Wire.begin(SDA, SCL);
  Serial.begin(115200);
}

void loop() {
  WirePacker packer;

  packer.write(duty1);
  packer.write(duty2);
  packer.end();
  
  Wire.beginTransmission(I2C_SLAVE_ADDR);
  while (packer.available()){
      Wire.write(packer.read());
  }
  Wire.endTransmission();
  Serial.println("Transmission done");
  delay(1000);
}
