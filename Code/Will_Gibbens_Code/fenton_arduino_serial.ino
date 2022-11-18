// WG Created: 14/11/22 Modified: 18/11/22
// Arduino code to send and recieve data with Python via the serial connection

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


// Initiate variables
int sensorLeft;  
int sensorRight;                     // variable for reading the sensor state
int sensorForwardLeft;  
int sensorForwardRight;
int data;

Servo grabber;  // create servo object to control a servo

void setup() { 
    Serial.begin(9600); //initialize serial COM at 9600 baudrate
    Serial.println("Hi!, I am Arduino");
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
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    grabber.attach(9);  // attaches the servo on pin 9 to the servo object
}

 

void loop() {
    while (Serial.available()){
    data = Serial.read();
    }
    if (data == '0'){
        motorLeft->run(FORWARD);
    }
    else if (data == '1') {
        motorLeft->run(RELEASE);
    }
}