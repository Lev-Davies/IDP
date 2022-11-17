// LW & LD Created: 4/11/22 Modified: 15/11/22

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
const int tunnelPingPin = 6;
const int tunnelRecievePin = 7;
const int blockPingPin = 8;
const int blockRecievePin = 9;
const int grabberServoPin = 10;
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
const int grabber_closed_position = 0;// degrees
const int grabber_open_position = 140; // degrees
const int beacon_flashing_frequency = 2; // Hz

// Initiate variables
bool sensorLeft;
bool sensorRight;
bool sensorForwardLeft;
bool sensorForwardRight;
bool orangeLedState = 0;
long last_flash_time;
int pos_counter = 0;

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
  pinMode(sensorLeftPin, INPUT);
  pinMode(sensorRightPin, INPUT);
  pinMode(sensorForwardLeftPin, INPUT);
  pinMode(sensorForwardRightPin, INPUT);
  pinMode(tunnelPingPin, OUTPUT);
  pinMode(tunnelRecievePin, INPUT);
  pinMode(blockPingPin, OUTPUT);
  pinMode(blockRecievePin, INPUT);
  pinMode(orangeLedPin, OUTPUT);
  digitalWrite(orangeLedPin, LOW);
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  AFMS.begin();
  motorLeft->setSpeed(200);
  motorRight->setSpeed(200);
  grabber.attach(grabberServoPin);
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

   pingTime = pulseIn(tunnelRecievePin, HIGH);

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
  sensorLeft = digitalRead(sensorLeftPin);
  sensorRight = digitalRead(sensorRightPin);
  sensorForwardLeft = digitalRead(sensorForwardLeftPin);
  sensorForwardRight = digitalRead(sensorForwardRightPin);
  String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight) + " Forward Left: " + String(sensorForwardLeft) + " Forward Right: " + String(sensorForwardRight);
  Serial.println(outputText);

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
