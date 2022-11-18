// WG Created: 4/11/22 Modified: 18/11/22
// testing the line sensors, with four line sensors and a Schmitt Trigger
// Testing the servo
// Adding flashing light

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
//const int grabber_closed_position = 80;// degrees
//const int grabber_open_position = 140; // degrees
//const int beacon_flashing_frequency = 2; // Hz

// Initiate variables
bool sensorLeft;
bool sensorRight;
bool sensorForwardLeft;
bool sensorForwardRight;
//bool orangeLedState = 0;
//long last_flash_time;
int pos_counter = 0;

// Servo grabber;  // create servo object to control a servo

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
  //grabber.attach(grabberServoPin);
}

void loop() {
  // 1 = white, 0 = black
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

  // code for flashing the orange beacon when moving
  /*if (millis() > (last_flash_time + 500/beacon_flashing_frequency)){
    if (orangeLedState == 1){
      digitalWrite(orangeLedPin, LOW);
      orangeLedState = 0;
    }
    else{
      digitalWrite(orangeLedPin, HIGH);
      orangeLedState = 1;
    }
    last_flash_time = millis();
  }
  */
  // Get data from the line sensors - 1 = white
  sensorLeft = digitalRead(sensorLeftPinDigital);
  sensorRight = digitalRead(sensorRightPinDigital);
  sensorForwardLeft = digitalRead(sensorForwardLeftPinDigital);
  sensorForwardRight = digitalRead(sensorForwardRightPinDigital);
  //String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight) + " Forward Left: " + String(sensorForwardLeft) + " Forward Right: " + String(sensorForwardRight);
  //Serial.println(outputText);
  Serial.println(pos_counter);

  switch(pos_counter){
    case 0:
    // Driving from centre of starting square to edge of starting square
      motorLeft->setSpeed(255);
      motorRight->setSpeed(255);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
      if(sensorLeft || sensorRight || sensorForwardLeft || sensorForwardRight){
        pos_counter += 1;
        delay(300);
      }
      break;
    case 1:
    // Following line to first junction
      if(!sensorLeft && sensorRight){
        Serial.println("Line mode");
        motorLeft->setSpeed(220);
        motorRight->setSpeed(20);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
      }
      else if (!sensorRight && sensorLeft){
        motorLeft->setSpeed(20);
        motorRight->setSpeed(220);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
      }
      else if (sensorLeft && sensorRight) {
        motorLeft->setSpeed(255);
        motorRight->setSpeed(255);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
      }
      if(sensorForwardRight || sensorForwardLeft){
        pos_counter+=1;
      }
      break;
    case 2:
      // First 90 degree right turn 
      Serial.println("Turn mode");
      delay(200);
      motorLeft->setSpeed(255);
      motorRight->setSpeed(225);
      motorLeft->run(FORWARD);
      motorRight->run(BACKWARD);
      delay(1200);
      if(sensorLeft || sensorRight){
        pos_counter+= 1;
      }
      break;
    case 3:
      // Following line to second junction
      if(!sensorLeft && sensorRight){
        Serial.println("Line mode");
        motorLeft->setSpeed(220);
        motorRight->setSpeed(20);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
      }
      else if (!sensorRight && sensorLeft){
        motorLeft->setSpeed(20);
        motorRight->setSpeed(220);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
      }
      else if (sensorLeft && sensorRight) {
        motorLeft->setSpeed(255);
        motorRight->setSpeed(255);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
      }
      if(sensorForwardRight || sensorForwardLeft){
        pos_counter-=1;
      }
      break;
    /*if (mm < 180){

      // Controlling motor using ultrasonic sensor
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
      else if(speedLeft < 0){
        speedLeft = 0;
      }
      if(speedRight > 255){
        speedRight = 255;
      }
      else if(speedRight < 0){
        speedRight = 0;
      }

      motorLeft->setSpeed(speedLeft);
      motorRight->setSpeed(speedRight);
    }
    else {
      motorLeft->setSpeed(220);
      motorRight->setSpeed(220);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
    } */
  }
  delay(10);
}




/*
  // Code for opening and closing the grabber
  grabber.write(grabber_open_position);
  delay(2000);
  grabber.write(grabber_closed_position);
  delay(2000);
  */
