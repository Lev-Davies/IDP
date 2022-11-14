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
const int pingPin = 7;
const int recievePin = 8;
const int grabberServoPin = 9;

// Define tunnel driving variables
const int kp = 3;
const int ki = 3;
const int target = 81;
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
  pinMode(sensorLeftPin, INPUT);
  pinMode(sensorRightPin, INPUT);
  pinMode(sensorForwardLeftPin, INPUT);
  pinMode(sensorForwardRightPin, INPUT);
  pinMode(pingPin, OUTPUT);
  pinMode(recievePin, INPUT);
  AFMS.begin();
  motorLeft->setSpeed(200);
  motorRight->setSpeed(200);
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
    motorLeft->setSpeed(220);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (sensorLeft == 0 && sensorRight == 1){
    motorLeft->setSpeed(-20);
    motorRight->setSpeed(210);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (sensorRight == 0 && sensorLeft == 1){
    motorLeft->setSpeed(210);
    motorRight->setSpeed(-20);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if ((sensorLeft && sensorRight) == 1) {
    motorLeft->run(FORWARD);
   motorRight->run(FORWARD);
   const int med_speed = 150;
   // establish variables for duration of the ping, and the distance result
   // in inches and centimeters:
   long pingTime, mm;

   // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
   digitalWrite(pingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(pingPin, HIGH);
   delayMicroseconds(5);
   digitalWrite(pingPin, LOW);

   // The same pin is used to read the signal from the PING))): a HIGH pulse
   // whose duration is the time (in microseconds) from the sending of the ping
   // to the reception of its echo off of an object.
   pingTime = pulseIn(recievePin, HIGH);

   // convert the time into a distance
   mm = distance_in_millimeters(pingTime);
   Serial.print(mm);
   Serial.print("mm");
   Serial.println();

   //controlling motor
 P = target - mm;
    I = prev_I + P*(current_time-prev_time);
    error = kp*P + ki*I;
    prev_time = current_time;
    prev_I = I;

    double speedLeft = med_speed - error;
    double speedRight = med_speed + error;

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
    delay(90);
  }
  delay(10);
/*
  grabber.write(grabber_open_position);
  delay(2000);
  grabber.write(grabber_closed_position);
  delay(2000);
  */
}
