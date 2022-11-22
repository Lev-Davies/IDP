// WG Created: 22/11/22 Modified: 22/11/22
// Code to calibrate the analogue read for the line sensors at the start of each run


#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>

// Define all pin numbers (updated for prototype board)
const int sensorLeftPinDigital = 2;
const int sensorRightPinDigital =  3;
const int sensorForwardLeftPinDigital = 4;
const int sensorForwardRightPinDigital = 5;
const int sensorLeftPinAnalog = A0;
const int sensorRightPinAnalog =  A1;
const int sensorForwardLeftPinAnalog = A2;
const int sensorForwardRightPinAnalog = A3;
const int blockReceivePin = 6;
const int blockPingPin = 7;
const int grabberServoPin = 8;
const int tunnelReceivePin = 9;
const int tunnelPingPin = 10;
const int redLedPin = 11;
const int greenLedPin = 12;
const int orangeLedPin = 13;

// Define constants
const int grabber_closed_position = 80;
const int grabber_open_position = 140;

// Initiate variables
int sensorLeft;  
int sensorRight;                    
int sensorForwardLeft;  
int sensorForwardRight;
int analogLeft;
int analogRight;
int analogForwardLeft;
int analogForwardRight;
int lineThresholdLeft = 500;
int lineThresholdRight = 400;
int lineThresholdForwardLeft = 500;
int lineThresholdForwardRight = 400;

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
}

void loop() {
  // 0 = white, 1 = black
  analogLeft = analogRead(sensorLeftPinAnalog);
  analogRight = analogRead(sensorRightPinAnalog);
  analogForwardLeft = analogRead(sensorForwardLeftPinAnalog);
  analogForwardRight = analogRead(sensorForwardRightPinAnalog);

  Serial.println(String(analogLeft) + String(analogRight) + String(analogForwardLeft) + String(analogForwardRight));

  lineThresholdLeft = analogLeft - 50;
  lineThresholdRight = analogRight - 50;
  lineThresholdForwardLeft = analogForwardLeft - 50;
  lineThresholdForwardRight = analogForwardRight - 50;

  Serial.println(String(lineThresholdLeft) + String(lineThresholdRight) + String(lineThresholdForwardLeft) + String(lineThresholdForwardRight));

//this block of the code is only for some output on the serial monitor
  if (analogLeft >= lineThresholdLeft)
      sensorLeft = 1; // black
    else
      sensorLeft = 0; // white

  if (analogRight >= lineThresholdLeft)
      sensorRight = 1; // black
    else
      sensorRight = 0; // white

  String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight);
  Serial.println(analogLeft);
  Serial.println(analogRight);
  Serial.println(analogForwardLeft);
  Serial.println(analogForwardRight);
  Serial.println(outputText);
}

