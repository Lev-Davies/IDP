// LD & WADDELL Created: 04/11/22 Modified: 17/11/22
// Modified by WG on 18/11/22 to update pin numbers for prototyping board
// testing the line sensors, with four line sensors and a Schmitt Trigger
// Testing the servo

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *motorRight = AFMS.getMotor(2);

// Define all pin numbers (updated for prototype board)
const int sensorLeftPinDigital = 2;
const int sensorRightPinDigital =  3;
const int sensorForwardLeftPinDigital = 4;
const int sensorForwardRightPinDigital = 5;
const int sensorLeftPinAnalog = A0;
const int sensorRightPinAnalog =  A1;
const int sensorForwardLeftPinAnalog = A2;
const int sensorForwardRightPinAnalog = A3;
const int blockPingPin = 6;
const int blockReceivePin = 7;
const int grabberServoPin = 8;
const int tunnelPingPin = 9;
const int tunnelReceivePin = 10;
const int orangeLedPin = 11;
const int greenLedPin = 12;
const int redLedPin = 13;

// Define tunnel driving variables
const int kp = 3;
const int ki = 3;
const int target = 41;
int I, P, prev_I, error;
unsigned long current_time = 0;
unsigned long prev_time = 0;

// Define constants
const int grabber_closed_position = 80;
const int grabber_open_position = 140;

// Initiate variables
int sensorLeft;  
int sensorRight;                     // variable for reading the sensor state
int sensorForwardLeft;  
int sensorForwardRight;

Servo grabber;  // create servo object to control a servo

long distance_in_millimeters(long microseconds){
   // The speed of sound is 340 m/s or 29 microseconds per centimeter.
   // The ping travels out and back, so to find the distance of the object we
   // take half of the distance travelled.
   return microseconds / 2.9 / 2;
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sensorLeftPinDigital, INPUT);
  pinMode(sensorRightPinDigital, INPUT);
  pinMode(sensorForwardLeftPinDigital, INPUT);
  pinMode(sensorForwardRightPinDigital, INPUT);
  pinMode(sensorLeftPinAnalog, INPUT);
  pinMode(sensorRightPinAnalog, INPUT);
  pinMode(sensorForwardLeftPinAnalog, INPUT);
  pinMode(sensorForwardRightPinAnalog, INPUT);
  pinMode(blockPingPin, OUTPUT);
  pinMode(blockReceivePin, INPUT);
  pinMode(tunnelPingPin, OUTPUT);
  pinMode(tunnelReceivePin, INPUT);
  pinMode(orangeLedPin, OUTPUT);
  digitalWrite(orangeLedPin, LOW);
  pinMode(greenLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW);
  pinMode(redLedPin, OUTPUT);
  digitalWrite(redLedPin, LOW);
  AFMS.begin();
  motorLeft->setSpeed(200);
  motorRight->setSpeed(200);
  grabber.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  // 0 = white, 1 = black
  long pingTime, mm;
  const int med_speed = 160;
  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
   digitalWrite(tunnelPingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(tunnelPingPin, HIGH);
   delayMicroseconds(5);
   digitalWrite(tunnelPingPin, LOW);

   pingTime = pulseIn(tunnelReceivePin, HIGH);

   mm = distance_in_millimeters(pingTime);
   Serial.print(mm);
   Serial.print("mm");
   Serial.println();

   // The same pin is used to read the signal from the PING))): a HIGH pulse
   // whose duration is the time (in microseconds) from the sending of the ping
   // to the reception of its echo off of an object.

  sensorLeft = 1 - digitalRead(sensorLeftPinDigital);
  sensorRight = 1 - digitalRead(sensorRightPinDigital);
  sensorForwardLeft = 1 - digitalRead(sensorForwardLeftPinDigital);
  sensorForwardRight = 1 - digitalRead(sensorForwardRightPinDigital);
  String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight) + " Forward Left: " + String(sensorForwardLeft) + " Forward Right: " + String(sensorForwardRight);
  Serial.println(outputText);
  if (sensorLeft == 0 && sensorRight == 0) {
    if (sensorLeft == 0 && sensorRight == 0 && sensorForwardRight== 0) {
    Serial.println("Turn mode");
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(1200); 
    }else if (sensorLeft == 0 && sensorRight == 0 && sensorForwardRight== 1) {
    Serial.println("Line mode");
    motorLeft->setSpeed(220);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  }
  } else if (sensorLeft == 0 && sensorRight == 1){
    if ((sensorLeft == 0 || sensorRight == 1) && sensorForwardRight== 0) {
    Serial.println("Turn mode");
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(1200);
    } else {
      Serial.println("Line mode");
    motorLeft->setSpeed(220);
    motorRight->setSpeed(20);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    }
  } else if (sensorRight == 0 && sensorLeft == 1){
    if ((sensorLeft == 0 || sensorRight == 1) && sensorForwardRight== 0) {
    Serial.println("Turn mode");
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(1200);
    } else {
      Serial.println("Line mode");
    motorLeft->setSpeed(20);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    }
  } else if ((sensorLeft && sensorRight) == 1) {
    Serial.println("Ultrasonic mode");
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  
  if (mm < 180){
   //controlling motor
 P = target - mm;
    I = prev_I + P*(current_time-prev_time);
    error = kp*P + ki*I;
    prev_time = current_time;
    prev_I = I;

    double speedLeft = med_speed + error;
    double speedRight = med_speed - error;

    if(speedLeft > 255){
      speedLeft = 255;
    }
    if(speedLeft < 0){
      speedLeft = 0;
    }
     if(speedRight > 255){
      speedRight = 255;
    }
    if(speedRight < 0){
      speedRight = 0;
    }

    motorLeft->setSpeed(speedLeft);
    motorRight->setSpeed(speedRight);
  }else {
    motorLeft->setSpeed(220);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  }
  }
  delay(10);
/*
  grabber.write(grabber_open_position);
  delay(2000);
  grabber.write(grabber_closed_position);
  delay(2000);
  */
}
