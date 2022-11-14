// WG Created: 4/11/22 Modified: 14/11/22
// testing the line sensors, with four line sensors and a Schmitt Trigger
// Testing the servo

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *motorRight = AFMS.getMotor(2);

// Define all pin numbers
const int sensorLeftPin = 2;
const int sensorRightPin =  3;
const int sensorForwardLeftPin = 4;
const int sensorForwardRightPin = 5;
const int ultrasonicTriggerPin = 7;
const int ultrasonicEchoPin = 8;
const int grabberServoPin = 9;


// Define constants
const int grabber_closed_position = 80;
const int grabber_open_position = 140;

// Initiate variables
int sensorLeft;  
int sensorRight;                     // variable for reading the sensor state
int sensorForwardLeft;  
int sensorForwardRight;

Servo grabber;  // create servo object to control a servo


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sensorLeftPin, INPUT);
  pinMode(sensorRightPin, INPUT);
  pinMode(sensorForwardLeftPin, INPUT);
  pinMode(sensorForwardRightPin, INPUT);
  AFMS.begin();
  motorLeft->setSpeed(255);
  motorRight->setSpeed(255);
  grabber.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  // 0 = white, 1 = black
  sensorLeft = 1 - digitalRead(sensorLeftPin);
  sensorRight = 1 - digitalRead(sensorRightPin);
  sensorForwardLeft = 1 - digitalRead(sensorForwardLeftPin);
  sensorForwardRight = 1 - digitalRead(sensorForwardRightPin);
  String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight) + " Forward Left: " + String(sensorForwardLeft) + " Forward Right: " + String(sensorForwardRight);
  Serial.println(outputText);
  if (sensorLeft == 0 && sensorRight == 0) {
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (sensorLeft == 0 && sensorRight == 1){
    motorLeft->setSpeed(0);
    motorRight->setSpeed(210);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (sensorRight == 0 && sensorLeft == 1){
    motorLeft->setSpeed(210);
    motorRight->setSpeed(0);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if ((sensorLeft && sensorRight) == 1) {
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
  }
  delay(10);
/*
  grabber.write(grabber_open_position);
  delay(2000);
  grabber.write(grabber_closed_position);
  delay(2000);
  */
}
