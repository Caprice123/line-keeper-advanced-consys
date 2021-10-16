#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>

#define SDA 18
#define SCL 19
#define I2C_SLAVE_ADDR 0x04

int duty1;
int duty2;




#include <Robojax_L298N_DC_motor.h>
// motor 1 settings
#define CHA 0
#define ENA 13 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 12
#define IN2 14
// motor 1 -> 52 mulai jalan
// motor 2 settings
#define IN3 27
#define IN4 26
#define ENB 25 // this pin must be PWM enabled pin if Arduino board is used
#define CHB 1

const int CCW = 2; // do not change
const int CW  = 1; // do not change

#define motor1 1 // do not change
#define motor2 2 // do not change

Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB, true);


void receiveEvent(int howMany);

void setup() {
  Serial.begin(115200);
  WireSlave.begin(SDA, SCL, I2C_SLAVE_ADDR);
  robot.begin();
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

  robot.rotate(motor1, duty1, CW);
  robot.rotate(motor2, duty2, CW);
}
