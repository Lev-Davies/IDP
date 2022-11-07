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
int sensorLeft = 0;  
int sensorRight = 0;                     // variable for reading the sensor state

void setup() {
  Serial.begin(9600);
  pinMode(sensorPin1, INPUT);
  pinMode(sensorPin2, INPUT);
  // initialize digital pin LED_BUILTIN as an output.
  AFMS.begin();
  motorLeft->setSpeed(255);
  motorRight->setSpeed(255);
}

// the loop function runs over and over again forever
void loop() {
    sensorLeft = 1 - digitalRead(sensorPin1);
    sensorRight = 1 - digitalRead(sensorPin2);
    String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight);
    Serial.println(outputText);
    if (( sensorLeft && sensorRight ) == 1) {
      motorLeft->setSpeed(255);
      motorRight->setSpeed(255);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
    } else if (sensorLeft == 0 && sensorRight == 1){
      motorLeft->setSpeed(80);
      motorRight->setSpeed(255);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
    } else if (sensorRight == 0 && sensorLeft == 1){
      motorLeft->setSpeed(255);
      motorRight->setSpeed(80);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
    } else if ((sensorLeft && sensorRight) == 0) {
      motorLeft->setSpeed(255);
      motorRight->setSpeed(255);
      motorLeft->run(RELEASE);
      motorRight->run(RELEASE);
    }
    delay(10);
}
