// WG Created: 14/11/22 Modified: 14/11/22
// Arduino code to send and recieve data with Python via the serial connection

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