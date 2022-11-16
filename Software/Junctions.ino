// making two motors move using a two light sensors


#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *motorRight = AFMS.getMotor(2);
float sensorValue = 0;

// unsigned long previous_time = 0;

// testing the line sensors

const int sensorPin1 =  2;
const int sensorPin2 = 3;
const int sensorPin3 =  4;
const int sensorPin4 = 5;
bool sensorLeft = 0;  
bool sensorRight = 0;                     // variable for reading the sensor state
bool sensorWideLeft = 0;  
bool sensorWideRight = 0;
int motorLeftspeed = 0;
int motorRightspeed = 0;

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  pinMode(sensorPin3, INPUT);
  pinMode(sensorPin4, INPUT);
  // initialize digital pin LED_BUILTIN as an output.
  AFMS.begin();
  motorLeft->setSpeed(255);
  motorRight->setSpeed(255);
}

// the loop function runs over and over again forever
void loop() {
    sensorLeft = 1 - digitalRead(sensorPin1);
    sensorRight = 1 - digitalRead(sensorPin2);
    sensorWideLeft = 1 - digitalRead(sensorPin3);
    sensorWideRight = 1 - digitalRead(sensorPin4);
    String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight) + " Forward Left: " + String(sensorWideLeft) + " Forward Right: " + String(sensorWideRight);
    Serial.println(outputText);
    if (sensorWideRight || sensorWideRight) {
      if (sensorWideRight &&  sensorLeft && sensorRight) {
          motorLeft->setSpeed(255);
          motorRight->setSpeed(-255);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
          delay(1000);
      } else if (sensorWideLeft &&  sensorLeft && sensorRight){
          motorLeft->setSpeed(-255);
          motorRight->setSpeed(255);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
          delay(1000);        
      } else if (sensorWideRight && sensorLeft && !sensorRight){
          motorLeft->setSpeed(255);
          motorRight->setSpeed(-180);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
          delay(1000);
      } else if (sensorWideLeft && !sensorLeft && sensorRight){
          motorLeft->setSpeed(-180);
          motorRight->setSpeed(255);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
          delay(1000);
      } else if (!sensorWideLeft && !sensorLeft && !sensorRight){
          motorLeft->setSpeed(-180);
          motorRight->setSpeed(255);
          motorLeft->run(RELEASE);
          motorRight->run(RELEASE);
          delay(1000);
      }
    } else {
        if (sensorLeft && sensorRight ) {
          motorLeft->setSpeed(255);
          motorRight->setSpeed(255);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
        } else if (!sensorLeft && sensorRight){
          motorLeft->setSpeed(0);
          motorRight->setSpeed(255);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
        } else if (!sensorRight && sensorLeft){
          motorLeft->setSpeed(255);
          motorRight->setSpeed(0);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
        } else if (!sensorLeft && !sensorRight) {
          motorLeft->setSpeed(255);
          motorRight->setSpeed(255);
          motorLeft->run(RELEASE);
          motorRight->run(RELEASE);
        }
    }
    delay(10);
}
