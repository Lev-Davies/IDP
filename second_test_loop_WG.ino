// WG Created: 23/11/22 Modified: 23/11/22
// from "first_test_loop.ino"

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
const int target = 41;
int I, P, prev_I, error;
unsigned long current_time = 0;
unsigned long prev_time = 0;
long pingTime, mm;
const int med_speed = 160;

// Define constants
const int grabber_closed_position = 80;
const int grabber_open_position = 140;
const long rampUpDelay = 6000;
const long rampDownDelay = 11000;
const int left_offset = 20; // Amount to increase left motor speed by over the right to go straight

// Initiate variables
long progStartTime = millis();
int position = 999;
unsigned long previousMillis = 0;

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
  if (Left.is_White() && Right.is_White()) {
    String outputText = "Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read());
    //Serial.println(outputText);
    motorLeft->setSpeed(230 + left_offset);
    motorRight->setSpeed(230);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (Left.is_White() && Right.is_Black()){ // or use the difference in analog reading to decide whether we need a left or right turn
    String outputText = "Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read());
    //Serial.println(outputText);
    motorLeft->setSpeed(20);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (Left.is_Black() && Right.is_White()){
    String outputText = "Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read());
    //Serial.println(outputText);
    motorLeft->setSpeed(220);
    motorRight->setSpeed(20);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (Left.is_Black() && Right.is_Black()) {
    String outputText = "Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read());
    //Serial.println(outputText);
    motorLeft->setSpeed(230 + left_offset);
    motorRight->setSpeed(230);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  }
}

void turn_right_90(){
  delay(200);
  motorLeft->setSpeed(255);
  motorRight->setSpeed(255 - left_offset);
  motorLeft->run(FORWARD);
  motorRight->run(BACKWARD);
  delay(1000); 
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
    if(speedLeft < 0){speedLeft = 0;}
    if(speedRight > 255){speedRight = 255;}
    if(speedRight < 0){speedRight = 0;}
    motorLeft->setSpeed(speedLeft);
    motorRight->setSpeed(speedRight);
}

void ramp_up(){
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(2000); 
}

void ramp_down(){
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(2000); 
}


//
//
//


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
  grabber.attach(grabberServoPin);  // attaches the servo on pin 9 to the servo object
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
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    position = 999;
  }

  if(position == 999){ // Wait for button press before starting
    delay(1000);
    while (digitalRead(buttonPin) == LOW){
      Serial.println("Waiting for button press to start");
      delay(10);
    }
    progStartTime = millis();
    position = 0;
  }

  else if(position == 0){  // Moving straight in the starting box
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    if(WideLeft.is_White() || WideRight.is_White()){// Reaches perpendicular line
      position = 1; // move on to next position
    }
  }
  
  else if (position == 1){ // Crossing first line to leave starting box
    if(WideLeft.is_Black() && WideRight.is_Black()){// Has crossed perpendicular line
      position = 2; // move on to next position
    }
  }
  
  else if (position == 2){ // Moving along the first short white line
    if(WideRight.is_White() || WideLeft.is_White()){
      position = 3; // move on to next position
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }
  
  else if(position == 3){ // Turning right at first junction
    if((currentMillis - previousMillis) > 250){
      turn_right_90();
      position = 4; // move on to next position
    }
  }
  
  else if(position == 4){ // Following straight white line
    if(WideRight.is_White()){
        position = 5; // move on to next position
        digitalWrite(redLedPin, 1);
        previousMillis = currentMillis;
    } else {
      follow_line();
    }
  } else if(position == 4){
   if (currentMillis - previousMillis >= rampUpDelay && currentMillis - previousMillis <= 7000) {
     // go up the ramp
     ramp_up();
    } else if (currentMillis - previousMillis >= rampDownDelay && currentMillis - previousMillis <= 13000){
    // go down the ramp
    ramp_down();
    } else {
    follow_line();
    }
    // Insert ramp function
    // Approaching next junction
    previousMillis = currentMillis;
    if(WideLeft.is_White()){
        position = 5;
    }  
  } else if(position == 5){
    // Driving from junction A to B
    if(WideRight.is_White() || WideLeft.is_White()){
        position = 6;
    } else {
      follow_line();
    }
  } else if(position == 6){
    // Driving from junction B to C
    if(WideLeft.is_White()){
        position = 7;
    } else {
      follow_line();
    }
  } else if(position == 7){
    if(mm < 100){
        tunnel_drive();
    } else if(WideRight.is_White()){
        position = 8;
    } else {
        follow_line();
    }
  } else if(position == 8){
    if(WideRight.is_White()){
        turn_right_90();
        motorLeft->setSpeed(255);
        motorRight->setSpeed(255);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
        delay(500);
        position = 9;
    } else {
        follow_line();
    }
  } else if(position == 9){
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
  }
  delay(2);
}
