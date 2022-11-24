// WG Created: 23/11/22 Modified: 24/11/22
// from "second_test_loop.ino"

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *motorRight = AFMS.getMotor(2);
Servo grabber;  // create servo object to control a servo

// Define pin numbers
const int buttonPin = 2;
const int blockReceivePin = 6;
const int blockPingPin = 7;
const int grabberServoPin = 8;
const int tunnelReceivePin = 9;
const int tunnelPingPin = 10;
const int redLedPin = 11;
const int greenLedPin = 12;
const int orangeLedPin = 13;

// Define tunnel driving variables
const int kp = 3;
const int ki = 3;
const int target = 55;
int I, P, prev_I, error;
unsigned long current_time = 0;
unsigned long prev_time = 0;
long pingTime, mm;
const int med_speed = 160;

// Define constants
const int grabber_closed_position = 80;
const int grabber_open_position = 140;
const long rampUpDelay = 7000;
const long rampDownDelay = 9000;
const int left_offset = 20; // Amount to increase left motor speed by over the right to go straight
const int robotSpeed = 10; // centimetres per millisecond

// Initiate variables
long progStartTime = millis();
int position = 999;
unsigned long previousMillis = 0;
int orangeLedState;
long lastFlashTime = progStartTime;
long expectedSectionDuration;

class LineSensor{
  private:
   int pin;
   int threshold;
  public:
  LineSensor(int p, int t){
    threshold = t;
    pin = p;
    pinMode(pin, INPUT);
  }

  double read(){
    return analogRead(pin);
  }

  bool is_White(){
    return analogRead(pin) <= threshold;
  }
  bool is_Black(){
    return analogRead(pin) >= threshold;
  }
};

// Define all Line Sensors
LineSensor Left(A3, 600);
LineSensor Right(A2, 600);
LineSensor WideLeft(A1, 600);
LineSensor WideRight(A0, 600);

void follow_line(){
  //Serial.println("Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read()));
  if (Left.is_White() && Right.is_White()) {
    motorLeft->setSpeed(230 + left_offset);
    motorRight->setSpeed(230);
  } else if (Left.is_White() && Right.is_Black()){ // or use the difference in analog reading to decide whether we need a left or right turn
    motorLeft->setSpeed(20);
    motorRight->setSpeed(220);
  } else if (Left.is_Black() && Right.is_White()){
    motorLeft->setSpeed(220);
    motorRight->setSpeed(20);
  } else if (Left.is_Black() && Right.is_Black()) {
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
 } else if (Left.is_Black() && Right.is_Black() && WideLeft.is_White()) {
    motorLeft->setSpeed(100);
    motorRight->setSpeed(230);
  } else if (Left.is_Black() && Right.is_Black() && WideRight.is_White()) {
    motorLeft->setSpeed(230);
    motorRight->setSpeed(100);
  }
  motorLeft->run(FORWARD);
  motorRight->run(FORWARD);
}

void turn_right_90(){
  delay(200);
  motorLeft->setSpeed(255);
  motorRight->setSpeed(255 - left_offset);
  motorLeft->run(FORWARD);
  motorRight->run(BACKWARD);
  delay(1100); 
}

void turn_left_90(){
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(950);
}

void tunnel_drive(){
    // establish variables for duration of the ping, and the distance
    // The PING is triggered by a HIGH pulse of 2 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(tunnelPingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(tunnelPingPin, HIGH);
    delayMicroseconds(5);
    digitalWrite(tunnelPingPin, LOW);

    pingTime = pulseIn(tunnelReceivePin, HIGH);

    mm = pingTime / 2.9 / 2;
    // The speed of sound is 340 m/s or 2.9 microseconds per millimeter.
    // Divide by 2 to account for signal goin there and back
    Serial.println(String(pingTime) + String(mm) + " mm");

    //controlling motor
    P = target - mm;
    I = prev_I + P*(current_time-prev_time);
    error = kp*P + ki*I;
    prev_time = current_time;
    prev_I = I;
    double speedLeft = med_speed + error;
    double speedRight = med_speed - error;
    if(speedLeft > 255){speedLeft = 255;}
    else if(speedLeft < 0){speedLeft = 0;}
    if(speedRight > 255){speedRight = 255;}
    else if(speedRight < 0){speedRight = 0;}
    motorLeft->setSpeed(speedLeft);
    motorRight->setSpeed(speedRight);
}









void setup() {
  Serial.begin(9600); //initiates serial moniter
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
  motorRight->setSpeed(200 - left_offset);
  grabber.attach(grabberServoPin);
}




//
//
//
//
//
//
//
//
//




void loop() {
  unsigned long currentMillis = millis();
  Serial.println(position);

  // Check for button to be pressed
  if (digitalRead(buttonPin) == HIGH && (currentMillis - progStartTime) > 1000){
    position = 999;
  }

  // Flash the orange beacon while running
  if ((currentMillis - lastFlashTime) > 250){
    digitalWrite(orangeLedPin, (1 - orangeLedState));
    orangeLedState = (1 - orangeLedState);
    lastFlashTime = currentMillis;
  }

  if(position == 999){ // Wait for button press before starting
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    digitalWrite(orangeLedPin, LOW);
    delay(1000);
    while (digitalRead(buttonPin) == LOW){
      Serial.println("Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read()));
      delay(10);
    }
    progStartTime = millis();
    position = 0;
    previousMillis = currentMillis;
  }

  else if(position == 0){  // Moving straight in the starting box
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    if(WideLeft.is_White() && WideRight.is_White()){// Reaches perpendicular line
      position = 1; // move on to next position
      previousMillis = currentMillis;
    }
  }
  
  else if (position == 1){ // Crossing first line to leave starting box
    if(WideLeft.is_Black() && WideRight.is_Black()){// Has crossed perpendicular line
      position = 2; // move on to next position
      previousMillis = currentMillis;
    }
  }
  
  else if (position == 2){ // Moving along the first short white line
    expectedSectionDuration = 16/robotSpeed;
    if(WideRight.is_White() || WideLeft.is_White()){
      if (currentMillis - previousMillis > 0.8 * expectedSectionDuration) {
        position = 3; // move on to next position
        previousMillis = currentMillis;
      }
    } else {
      follow_line();
    }
  }
  
  else if(position == 3){ // Turning right at first junction
    if((currentMillis - previousMillis) > 250){
      turn_right_90();
      position = 4; // move on to next position
      previousMillis = currentMillis;
    }
  }
  
  else if(position == 4){ // Following straight white line
    expectedSectionDuration = 68/robotSpeed;
    if(WideRight.is_White()  && (currentMillis - previousMillis > 0.8 * expectedSectionDuration)){
      if (currentMillis - previousMillis > 0.8 * expectedSectionDuration) {
        position = 5; // move on to next position
        previousMillis = currentMillis;
      }
    } else {
      follow_line();
    }
  }
  else if(position == 5){ // Cross the junction leading to the red box
    if (WideRight.is_Black()){
      position = 6; // move on to next position
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }

  else if(position == 6){ // follow the path across the ramp
    expectedSectionDuration = 232/robotSpeed;
    // Approaching next junction
    if(WideLeft.is_White() && currentMillis - previousMillis > 0.6 * expectedSectionDuration){
      if (Left.is_White() || Right.is_White()){
        position = 7; // move on to next position
        previousMillis = currentMillis;
      }
    } else{
      follow_line();
    }
  }

  else if (position == 7){ // cross the junction after the ramp
    if (WideLeft.is_Black()) {
      position = 8; // move on to next position
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }
  else if(position == 8){ // Driving from junction 7 to the cross
  expectedSectionDuration = 38/robotSpeed;
    if((WideRight.is_White() || WideLeft.is_White()) && (currentMillis - previousMillis > 0.6 * expectedSectionDuration)){
        position = 9; // move on to next position
        previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }
  
  else if(position == 9){ // Crossing the cross
    if(WideLeft.is_Black() && WideRight.is_Black()){
        position = 10; // move on to next position
        previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }
  
  else if (position == 10) { // Follow the straight line after the cross
  expectedSectionDuration = 38/robotSpeed;
    if (WideLeft.is_White() && currentMillis - previousMillis > 0.6 * expectedSectionDuration){ 
      position = 11; // move on to next position
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }

  else if (position == 11) { // Cross the junction before the tunnel
    if (WideLeft.is_Black()){ 
      position = 12; // move on to next position
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }

  else if(position == 12){ // Follow the path under the tunnel
    if (currentMillis-previousMillis > 9000 && currentMillis-previousMillis < 11000){
      motorLeft->setSpeed(255);
      motorRight->setSpeed(255-left_offset);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
    } else if(Left.is_Black() && Right.is_Black()){
      tunnel_drive();
    } else if(WideRight.is_White()){
        position = 15; // move on to next position
        previousMillis = currentMillis;
    } else {
        follow_line();
        if ((currentMillis - previousMillis)>9000){
        }
    }
  }

  else if(position == 15) {
    if (WideRight.is_Black()) {
      position = 16; // move on to next position
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }
  
  else if (position == 16) {
  expectedSectionDuration = 68/robotSpeed;
    if (WideRight.is_White() && currentMillis - previousMillis > 0.6 * expectedSectionDuration) {
      position = 17; // move on to next position
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }

  else if(position == 17){ // Turning right back into home
    if((currentMillis - previousMillis) > 250){
      turn_right_90();
      position = 18; // move on to next position
      previousMillis = currentMillis;
    }
  }

  else if (position == 18){ // Last little line segment
  expectedSectionDuration = 16/robotSpeed;
    if (WideLeft.is_White() || WideRight.is_White()) {
      if (currentMillis - previousMillis > 0.8 * expectedSectionDuration) {
        position = 19; // move on to next position
        previousMillis = currentMillis;
      }
    } else {
      follow_line();
    }
  }
  
  else if (position == 19){ // Cross final perpendicular line
    if (WideLeft.is_Black() && WideRight.is_Black()) {
      motorLeft->setSpeed(255);
      motorRight->setSpeed(255 - left_offset);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
      delay(2500);
      motorLeft->run(RELEASE);
      motorRight->run(RELEASE);
      position = 999; // Finish
      previousMillis = currentMillis;
    }
  }
  delay(2);
}