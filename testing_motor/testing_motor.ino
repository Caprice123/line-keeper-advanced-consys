
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
String incomingbyte;
int duty;
// for single motor
//Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA, true);  

// for two motors without debug information // Watch video instruciton for this line: https://youtu.be/2JTMqURJTwg
Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA,  IN3, IN4, ENB, CHB, true);

// fore two motors with debut information
//Robojax_L298N_DC_motor robot(IN1, IN2, ENA, CHA, IN3, IN4, ENB, CHB, true);

void setup() {
  Serial.begin(115200);
  robot.begin();
  //L298N DC Motor by Robojax.com

}

void loop() {
  
// // robot.demo(1);
//  robot.rotate(motor1, 80, CW);//run motor1 at 80% speed in CW direction
//  robot.rotate(motor2, 70, CW);//run motor2 at 70% speed in CW direction
//  
//  delay(3000);
//
//  robot.brake(1);
//  robot.brake(2);  
//  delay(2000);

//
//  robot.rotate(motor1, 100, CW);//run motor1 at 100% speed in CW direction
//  robot.rotate(motor2, 100, CW);//run motor2 at 100% speed in CW direction
//  delay(3000);
//  
//  
//  robot.brake(1);
//  robot.brake(2);   
//  delay(2000);  
//
//  for(int i=60; i<=100; i++)
//  {
//    robot.rotate(motor1, i, CW);// turn motor1 with i% speed in CW direction (whatever is i) 
//    robot.rotate(motor2, i, CW);// turn motor2 with i% speed in CW direction (whatever is i) 
//    delay(1000);
      
//  }
    while (Serial.available()) {
      char c = Serial.read();  //gets one byte from serial buffer
      incomingbyte += c; //makes the string readString
      delay(2);  //slow looping to allow buffer to fill with next character
    }
    duty = incomingbyte.toInt();
    Serial.println(duty);
    robot.rotate(motor1, duty, CW);// turn motor1 with i% speed in CW direction (whatever is i) 
    robot.rotate(motor2, duty, CW);
          
    
//  delay(2000);
//  
//  robot.brake(1);
//  
//  robot.brake(2);
//  delay(2000);  
//    
  // Robojax L298N Library. Watch video instruciton https://youtu.be/2JTMqURJTwg
}
