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
const int target = 81;
int I, P, prev_I, error;
unsigned long current_time = 0;
unsigned long prev_time = 0;

// Define constants
const int grabber_closed_position = 80;
const int grabber_open_position = 140;
const int lineThreshold = 600; // depends on the distance of the line sensor to the ground (according to my experience, the larger the distance, the higher the threshold is) 
// we might also need to set a threshold for each line sensor, since I found that they sometimes had different analog readings for the same color.

// Initiate variables
int sensorLeft;  
int sensorRight;                    
int sensorForwardLeft;  
int sensorForwardRight;
int analogLeft;
int analogRight;

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
  motorLeft->setSpeed(130);
  motorRight->setSpeed(130);
  grabber.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  // 0 = white, 1 = black
  analogLeft = analogRead(sensorLeftPinAnalog);
  analogRight = analogRead(sensorRightPinAnalog);

//this block of the code is only for some output on the serial monitor
  if (analogLeft >= lineThreshold)
      sensorLeft = 1; // black
    else
      sensorLeft = 0; // white

  if (analogRight >= lineThreshold)
      sensorRight = 1; // black
    else
      sensorRight = 0; // white

  String outputText = "Left: " + String(sensorLeft) + " Right: " + String(sensorRight) ;
  Serial.println(analogLeft);
  Serial.println(analogRight);
  Serial.println(outputText);

//The code to control the motor
  if (analogLeft <= lineThreshold && analogRight <= lineThreshold) {
    motorLeft->setSpeed(220);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (analogLeft - analogRight <= -200){ // I used the difference in analog reading to decide whether we need a left or right turn
    motorLeft->setSpeed(0);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (analogLeft - analogRight >= 200){
    motorLeft->setSpeed(220);
    motorRight->setSpeed(0);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (analogLeft >= lineThreshold && analogRight >= lineThreshold) {
    Serial.println("Ultrasonic mode");
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  }
  delay(10);
/*
  grabber.write(grabber_open_position);
  delay(2000);
  grabber.write(grabber_closed_position);
  delay(2000);
  */
}
