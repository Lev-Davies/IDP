#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
const int blockPingPin = 8;
const int blockRecievePin = 9;
long distance_in_millimeters(long microseconds){
   // The speed of sound is 340 m/s or 29 microseconds per centimeter.
   // The ping travels out and back, so to find the distance of the object we
   // take half of the distance travelled.
   return microseconds / 2.9 / 2;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(blockPingPin, OUTPUT);
  pinMode(blockRecievePin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:  
  long pingTime, mm;
  const int med_speed = 160;
  int density;
  double ave_mm = 0; //average distance
  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  for (int cycle = 0; cycle < 15; cycle += 1){
    digitalWrite(blockPingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(blockPingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(blockPingPin, LOW);

    pingTime = pulseIn(blockRecievePin, HIGH);

    mm = distance_in_millimeters(pingTime);
    ave_mm = ave_mm + mm;
    //Serial.print(mm);
    //Serial.print("mm");
    //Serial.println();
    delay(20);     
  }
  ave_mm = ave_mm / 10;
  if (ave_mm <= 400){
    density = 0; //low density
    Serial.println("low");
    Serial.println(ave_mm);
  } else {
    density = 1; //high density
    Serial.println("high");
    Serial.println(ave_mm);
  }
  delay(5);
   
}
