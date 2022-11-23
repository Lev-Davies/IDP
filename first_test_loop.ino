// LD & WADDELL Created: 04/11/22 Modified: 17/11/22
// Modified by WG on 18/11/22 to update pin numbers for prototyping board
// testing the line sensors, with four line sensors and a Schmitt Trigger
// Testing the servo

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <Servo.h>
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
Adafruit_DCMotor *motorLeft = AFMS.getMotor(1);
Adafruit_DCMotor *motorRight = AFMS.getMotor(2);

// Define all pin numbers (updated for prototype board)
const int blockReceivePin = 6;
const int blockPingPin = 7;
const int grabberServoPin = 8;
const int tunnelReceivePin = 9;
const int tunnelPingPin = 10;
const int redLedPin = 11;
const int greenLedPin = 12;
const int orangeLedPin = 13;
unsigned long previousMillis = 0;
const long rampUpDelay = 6000;
const long rampDownDelay = 11000;


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

// Initiate variables
int position = 0;

Servo grabber;  // create servo object to control a servo

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

  bool is_White(){
    return analogRead(pin) <= threshold;
  }
  bool is_Black(){
    return analogRead(pin) >= threshold;
  }
};

// Define all Line Sensors
LineSensor Left(A3, 900);
LineSensor Right(A2, 800);
LineSensor WideLeft(A1, 900);
LineSensor WideRight(A0, 900);

void follow_line(){
  if (Left.is_White() && Right.is_White()) {
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (Left.is_White() && Right.is_Black()){ // or use the difference in analog reading to decide whether we need a left or right turn
    motorLeft->setSpeed(20);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (Left.is_Black() && Right.is_White()){
    motorLeft->setSpeed(220);
    motorRight->setSpeed(20);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (Left.is_Black() && Right.is_Black()) {
    Serial.println("Ultrasonic mode");
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  }
  delay(10);
}

void turn_right_90(){
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(1200); 
}

void turn_left_90(){
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(1200);    
}

void tunnel_drive(){
    //controlling motor
    P = target - mm;
    I = prev_I + P*(current_time-prev_time);
    error = kp*P + ki*I;
    prev_time = current_time;
    prev_I = I;

    double speedLeft = med_speed + error;
    double speedRight = med_speed - error;

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
}

void RAMP_UP(){
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(2000); 
}

void RAMP_DOWN(){
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(2000); 
}

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
  motorRight->setSpeed(200);
  grabber.attach(9);  // attaches the servo on pin 9 to the servo object
}

void loop() {
  unsigned long currentMillis = millis();
  // establish variables for duration of the ping, and the distance result
  // in inches and centimeters:
  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
   // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
   digitalWrite(tunnelPingPin, LOW);
   delayMicroseconds(2);
   digitalWrite(tunnelPingPin, HIGH);
   delayMicroseconds(5);
   digitalWrite(tunnelPingPin, LOW);

   pingTime = pulseIn(tunnelReceivePin, HIGH);

   mm = distance_in_millimeters(pingTime);
   Serial.print(mm);
   Serial.print("mm");
   Serial.println();

  if(position = 0){
    // Moving straight in the starting box
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);

    if(Left.is_White() || Right.is_White()){
        // Detects line
        position = 1;
    }
  } else if(position = 1){
    // Following line to first junction
    follow_line();
    if(WideRight.is_White() || WideLeft.is_White()){
        position = 2;
    }
  } else if(position = 2){
    // Turning right at first junction
    turn_right_90();
    position = 3;
  } else if(position = 3){
    // Driving past red junction
    follow_line();
    if(WideRight.is_White()){
        position = 4;
        previousMillis = currentMillis;
    }
  } else if(position = 4){
   if (currentMillis - previousMillis >= rampUpDelay && currentMillis - previousMillis <= 7000) {
     // go up the ramp
     RAMP_UP();
    } else if (currentMillis - previousMillis >= rampDownDelay && currentMillis - previousMillis <= 13000){
    // go down the ramp
    RAMP_DOWN();
    } else {
    follow_line();
    }
    // Insert ramp function
    // Approaching next junction
    previousMillis = currentMillis;
    if(WideLeft.is_White()){
        position = 5;
    }  
  } else if(position = 5){
    // Driving from junction A to B
    follow_line();
    if(WideRight.is_White() || WideLeft.is_White()){
        position = 6;
    }
  } else if(position = 6){
    // Driving from junction B to C
    follow_line();
    if(WideLeft.is_White()){
        position = 7;
    }
  } else if(position = 7){
    if(mm < 100){
        tunnel_drive();
    } else if(WideRight.is_White()){
        position = 8;
    } else {
        follow_line();
    }
  } else if(position = 8){
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
  } else if(position = 9){
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
  }
  delay(2);
}
