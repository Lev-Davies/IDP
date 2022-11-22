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

int density = 1; // the density is initialised as 1 (high). Once there is a low reading, it will become 0 and will not be back to 1 unless a reset function to be called.

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
}

int foamRecognition() {
  long pingTime, mm;
  double ave_mm = 0; //average distance
  int numberOfCylesForAverage = 4;
  int foamType; // 1 or 0 - high or low
  int DistanceThreshold = 400; // if the distance from sensor is below this value, it will be recognised as low density - if IR is used simultanously, this can be modified and compared with the result from IR sensor

  for (int cycle = 0; cycle < numberOfCylesForAverage; cycle += 1){
    digitalWrite(blockPingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(blockPingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(blockPingPin, LOW);

    pingTime = pulseIn(blockReceivePin, HIGH);

    mm = distance_in_millimeters(pingTime);
    ave_mm = ave_mm + mm;
    //Serial.print(mm);
    //Serial.print("mm");
    //Serial.println();
    delay(20);     
  }

  ave_mm = ave_mm / numberOfCylesForAverage;

  if (ave_mm <= DistanceThreshold){
    foamType = 0; //low density
    Serial.println("low");
    Serial.println(ave_mm);
  } else {
    foamType = 1; //high density
    Serial.println("high");
    Serial.println(ave_mm);
  }
  return foamType;
}

void loop() {
  // put your main code here, to run repeatedly:  
  const int med_speed = 160;


  if (foamRecognition() == 0){
    density = 0;
  }
  Serial.println(density);
  delay(5);
   
}
