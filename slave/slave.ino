#include <Arduino.h>
#include <Wire.h>
#include <WireSlave.h>

#define SDA 21
#define SCL 22
#define I2C_SLAVE_ADDR 0x04

int duty1;
int duty2;


#include <Robojax_L298N_DC_motor.h>
// motor 1 settings
#define CHA 0
#define ENA 19 // this pin must be PWM enabled pin if Arduino board is used
#define IN1 18
#define IN2 5
// motor 1 -> 52 mulai jalan
// motor 2 settings
#define IN3 17
#define IN4 16
#define ENB 4


//#define ENA 13 // this pin must be PWM enabled pin if Arduino board is used
//#define IN1 12
//#define IN2 14
//// motor 1 -> 52 mulai jalan
//// motor 2 settings
//#define IN3 27
//#define IN4 26
//#define ENB 25 // this pin must be PWM enabled pin if Arduino board is used
#define CHB 1

const int CCW = 2; // do not change
const int CW  = 1; // do not change

#define motor1 1 // do not change
#define motor2 2 // do not change

int last_steering_angle = 0;
const int max_speed = 100;
const int default_speed_left = 55;
const int default_speed_right = 55;
const int threshold_degree = 10;
const int setpoint = 90;
int steering_angle;

unsigned long currentTime;
unsigned long previousTime;
double elapsedTime;
double cumError, rateError, lastError;
int deviation, error;

int left = default_speed_left;
int right = default_speed_right;

float kp = 0.625;
float ki = 0;
float kd = 0;


Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB, false);


void receiveEvent(int howMany);

void setup() {
  Serial.begin(115200);
  WireSlave.begin(SDA, SCL, I2C_SLAVE_ADDR);
  robot.begin();
  WireSlave.onReceive(receiveEvent);
  Serial.println("left:,right:");
//  for (int x = 20; x < 101; x++){
//    robot.rotate(motor1,x, CW);
//  robot.rotate(motor2, x, CW);
//  Serial.println(x);
//  delay(500);
//  
//  }
//  robot.rotate(motor1, 100, CW);
//  robot.rotate(motor2, 55, CW);
//  delay(1000);
//  robot.rotate(motor1, 0, CW);
//  robot.rotate(motor2, 0, CW);
}

void loop() {
  
  WireSlave.update();
  delay(1);
}



void receiveEvent(int howMany){
  currentTime = millis();
  elapsedTime = (double) currentTime - previousTime;
  
  steering_angle = WireSlave.read();
  steering_angle = (steering_angle > 180) ? last_steering_angle : steering_angle;

  deviation = setpoint - steering_angle;
  error = abs(deviation);

  cumError += deviation * elapsedTime;
  rateError = (deviation - lastError) / elapsedTime;


  double out = kp * error + ki * cumError + kd * rateError;

  if (deviation <= threshold_degree && deviation >= -threshold_degree){
      left = default_speed_left;
      right = default_speed_right;
  }
  else if (deviation < -threshold_degree){
      left = default_speed_left + out;
      right = default_speed_right;
  }
  else if (deviation > threshold_degree){
      left = default_speed_left;
      right = default_speed_right + out;
  }
  left = (left > max_speed) ? max_speed : left;
  left = (left < default_speed_left) ? default_speed_left : left;

  right = (right > max_speed) ? max_speed : right;
  right = (right < default_speed_right) ? default_speed_right : right;
  Serial.print(steering_angle);
  Serial.print(",");
  Serial.print(left);
  Serial.print(",");
  Serial.println(right);
//robot.rotate(motor1, left, CW);
//  robot.rotate(motor2, right, CW);

  robot.rotate(motor2, left, CW);
  robot.rotate(motor1, right, CW);
//  delay(100);
//  robot.rotate(motor2, 0, CW);
//  robot.rotate(motor1, 0, CW);

  lastError = deviation;
  previousTime = currentTime;
  last_steering_angle = steering_angle;
}
