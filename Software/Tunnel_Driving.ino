#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *motorRight = AFMS.getMotor(2);
// this constant won't change. It's the pin number of the sensor's output:
const int pingPin = 7;
const int recievePin = 8;

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
    long duration, mm;
  const int kp = 20;
  const int ki = 0;
  const int target = 200;
  int I, P, prev_I, error;
    unsigned long current_time = 0;
  unsigned long prev_time = 0;
    AFMS.begin();
    const int med_speed = 150;
  motorLeft->setSpeed(med_speed);
  motorRight->setSpeed(med_speed);
}


void loop() {
  // establish variables for duration of the ping, and the distance result
  // in millimeters:

    unsigned long current_time = millies();

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  pinMode(pingPin, OUTPUT); //maybe move
  digitalWrite(pingPin, LOW);
  delayMicroseconds(2);
  digitalWrite(pingPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pingPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  pinMode(recievePin, INPUT); //maybe move
  duration = pulseIn(recievePin, HIGH);

  // convert the time into a distance
  mm = distance_in_millimeters(duration);

  //Serial.print(mm);
  //Serial.print("mm");
  //Serial.println();

    P = target - mm;
    I = prev_I + P*(current_time-prev_time);
    error = kp*P + ki*I;
    prev_time = current_time;
    prev_I = I;


    motorLeft->setSpeed(med_speed - error);
    motorRight->setSpeed(med_speed + error);

  delay(50);
}


long distance_in_millimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 2.9 / 2;


}
