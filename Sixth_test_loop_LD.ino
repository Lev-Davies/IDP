// WG Created: 28/11/22 Modified: 29/11/22
// based on "fourth_test_loop_WG.ino"

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
const int target = 61;
int I, P, prev_I, error;
unsigned long current_time = 0;
unsigned long prev_time = 0;
long pingTime, mm;
const int med_speed = 160;

// Define constants
const int grabber_closed_position = 50;
const int grabber_open_position = 110;
const int grabber_transport_position = 70;
const int left_offset = 20; // Amount to increase left motor speed by over the right to go straight
const double robotSpeed = 0.014; // centimetres per millisecond

// Initiate variables
long progStartTime = millis();
int prev_position = 999;
int position = 999;
int next_position = 0;
unsigned long previousMillis = 0;
int orangeLedState;
long lastFlashTime = progStartTime;
long expectedSectionDuration;
String in_grabber = "empty";
int block_location = 1; // Starting position for the block

void motor_forward(int leftSpeed, int rightSpeed){
  motorLeft->setSpeed(leftSpeed);
  motorRight->setSpeed(rightSpeed);
  motorLeft->run(FORWARD);
  motorRight->run(FORWARD);
}

void turn_right_90(){
  motorLeft->setSpeed(255);
  motorRight->setSpeed(255 - left_offset);
  motorLeft->run(FORWARD);
  motorRight->run(BACKWARD);
  delay(1050); 
}

void turn_left_90(){
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(1050);
}

void turn_left_180(){
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(2100);
}

class LineSensor{
  private:
   int pin;
  public:
   int threshold;
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
  if (Left.is_White() && Right.is_White()) { // On track: go straight
    motor_forward(255, 255 - left_offset);
  } else if (Left.is_White() && Right.is_Black()){ // Too right, go left
    motor_forward(20, 220);
  } else if (Left.is_Black() && Right.is_White()){ // Too left, go right
    motor_forward(220, 20);
  } else if (Left.is_Black() && Right.is_Black()) {
    if (WideLeft.is_White()){ // Much too right, go left
      motor_forward(20, 230);
      delay(200);
    } else if (WideRight.is_White()) { // Much too left, go right
      motor_forward(230, 20);
      delay(200);
    } else { // All black, go forwards blindly
      motor_forward(255, 255 - left_offset);
    }
  }
}

int foamRecognition() { //return the type of the foam
  long pingTime, mm2;
  double ave_mm = 0; //average distance
  int numberOfCylesForAverage = 5;
  int foamType; // 1 or 0 - high or low
  int DistanceThreshold = 200; // if the distance from sensor is below this value, it will be recognised as low density - if IR is used simultanously, this can be modified and compared with the result from IR sensor

  for (int cycle = 0; cycle < numberOfCylesForAverage; cycle += 1){
    digitalWrite(blockPingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(blockPingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(blockPingPin, LOW);

    pingTime = pulseIn(blockReceivePin, HIGH);
    mm2 = pingTime / 2.9 / 2;
    ave_mm = ave_mm + mm2;
    delay(10);
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

long get_tunnel_distance(){
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
    return mm;
}

void tunnel_drive(){
    //Using a PI loop to follow the distance sensor in the tunnel
    //current_time = millis();
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

void collect(){
  grabber.write(grabber_open_position);
  String density = "low";
  motorLeft->setSpeed(150);
  motorRight->setSpeed(150);
  motorLeft->run(BACKWARD);
  motorRight->run(BACKWARD);

  for (int i = 0; i < 15; i++){
    if (foamRecognition() == 1){ //a totay delay of 50 in each foamRocognition()
      density = "high";
    }
    Serial.println(density);
  }
  in_grabber = density;
  if (density == "low"){
    digitalWrite(greenLedPin, HIGH);
    digitalWrite(redLedPin, LOW);
  } else if (density == "high"){
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, HIGH);
  }
  motor_forward(100, 100);
  delay(6000);
  motorLeft->run(RELEASE);
  motorRight->run(RELEASE);
  delay(5000);
  grabber.write(grabber_closed_position);

  if (block_location == 2 || block_location == 3){
    turn_left_180();
  }
}

void deposit(){
    motor_forward(150, 150);
    delay(2500);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    grabber.write(grabber_open_position);
    motorLeft->run(BACKWARD);
    motorRight->run(BACKWARD);
    delay(1000);
    turn_left_180();
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    grabber.write(grabber_transport_position);
    motor_forward(150, 150);
    delay(1500);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    in_grabber = "empty";
    digitalWrite(greenLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    if (block_location == 1){
      block_location = 2;
    } else if (block_location == 2){
      block_location = 3;
    } else if (block_location == 3){
      block_location = 1;
    }
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
  Serial.println("Prev: " + String(prev_position) + " Position: " + String(position) + " Next: " + String(next_position));

  // Check for button to be pressed
  if (digitalRead(buttonPin) == HIGH && (currentMillis - progStartTime) > 1000){
    position = 999;
    next_position = 0;
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
    grabber.write(grabber_transport_position);
    digitalWrite(orangeLedPin, LOW);
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    delay(1000);
    while (digitalRead(buttonPin) == LOW){
      Serial.println("Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read()));
      delay(10);
    }
    progStartTime = millis();
    prev_position = position; position = next_position; next_position = 1;
    previousMillis = currentMillis;
  }

  else if(position == 0){  // Moving straight in the starting box
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    if(WideLeft.is_White() && WideRight.is_White()){// Reaches perpendicular line
      prev_position = position; position = next_position; next_position = 2;
      previousMillis = currentMillis;
    }
  }

  else if (position == 1){ // Crossing first line to leave starting box
    if (next_position == 2){
      if(WideLeft.is_Black() && WideRight.is_Black()){// Has crossed perpendicular line
        prev_position = position; position = next_position; next_position = 3;
        previousMillis = currentMillis;
      }
    } else if (next_position == 999){
      if(WideLeft.is_Black() && WideRight.is_Black()){// Has crossed perpendicular line
        motor_forward(255, 255 - left_offset);
        delay(2500);
        turn_left_180();
        prev_position = position; position = next_position; next_position = 0;
        previousMillis = currentMillis;
      }
    }
  }
  
  else if (position == 2){ // Moving along the first short white line
    expectedSectionDuration = 16/robotSpeed;
    if (prev_position == 1 && next_position == 3){
      if(WideRight.is_White() || WideLeft.is_White()){
        if (currentMillis - previousMillis > 0.9 * expectedSectionDuration) {
          prev_position = position; position = next_position; next_position = 4;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    } else if (prev_position == 3 && next_position == 1){
      if(WideRight.is_White() || WideLeft.is_White()){
        if (currentMillis - previousMillis > 0.9 * expectedSectionDuration) {
          prev_position = position; position = next_position; next_position = 999;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    }
  }
  
  else if(position == 3){ // Turning right out of first junction
    if (prev_position == 2 && next_position == 4){
      if((currentMillis - previousMillis) > 250){
        turn_right_90();
        prev_position = position; position = next_position; next_position = 5;
        previousMillis = currentMillis;
      }
    } else if (prev_position == 16 && next_position == 4){
      if (WideRight.is_Black()){
        prev_position = position; position = next_position; next_position = 5;
        previousMillis = currentMillis;
      } else{
        follow_line();
      }
    } else if (prev_position == 16 && next_position == 2){
      if((currentMillis - previousMillis) > 250){
        turn_right_90();
        prev_position = position; position = next_position; next_position = 1;
        previousMillis = currentMillis;
      }
    }
  }
  
  else if(position == 4){ // Following straight white line
    expectedSectionDuration = 68/robotSpeed;
    if(WideRight.is_White()){
      if (currentMillis - previousMillis > 0.9 * expectedSectionDuration) {
        prev_position = position; position = next_position; // move on to next position
        if (in_grabber == "high"){
          next_position = 17;
        } else{
          next_position = 6;
        }
        previousMillis = currentMillis;
      }
    } else {
      follow_line();
    }
  }

  else if(position == 5){ // Cross the junction leading to the red box
    if (prev_position == 4 && next_position == 6){
      if (WideRight.is_Black()){
        prev_position = position; position = next_position; next_position = 7;
        previousMillis = currentMillis;
      } else{
        follow_line();
      }
    } else if (prev_position == 4 && next_position == 17){
      if((currentMillis - previousMillis) > 250){
        turn_right_90();
        prev_position = position; position = next_position; next_position = 18;
        previousMillis = currentMillis;
      }
    } else if (prev_position == 17 && next_position == 6){
      if((currentMillis - previousMillis) > 250){
        turn_right_90();
        prev_position = position; position = next_position; next_position = 7;
        previousMillis = currentMillis;
      }
    }
  }

  else if(position == 6){ // follow the path across the ramp
    expectedSectionDuration = 232/robotSpeed;
    if (currentMillis - previousMillis >= 6500 && currentMillis - previousMillis <= 10000) {
      motor_forward(255, 255 - left_offset);
    } else if (currentMillis - previousMillis >= 14000 && currentMillis - previousMillis <= 16000){
      motor_forward(100, 100);
    } else {
      follow_line();
    }
    if(WideLeft.is_White() && (currentMillis - previousMillis > 0.9 * expectedSectionDuration)){
      if (Left.is_White() || Right.is_White()){
        prev_position = position; position = next_position;
        if (block_location == 2){
          next_position = 19;
        } else{
          next_position = 8;
        }
        previousMillis = currentMillis;
      }
    }  
  }

  else if (position == 7){ // cross the junction after the ramp
    if (prev_position == 6 && next_position == 8){
      if (WideLeft.is_Black()) {
        prev_position = position; position = next_position; next_position = 9;
        previousMillis = currentMillis;
      } else {
        follow_line();
      }
    } else if (prev_position == 6 && next_position == 19){
      if((currentMillis - previousMillis) > 250){
        turn_left_90();
        prev_position = position; position = next_position; next_position = 19;
        previousMillis = currentMillis;
      }
    } else if (prev_position == 19 && next_position == 8){
      if((currentMillis - previousMillis) > 250){
        turn_left_90();
        prev_position = position; position = next_position; next_position = 9;
        previousMillis = currentMillis;
      }
    }
  }

  else if (position == 8){ // Driving from junction 7 to the cross
    expectedSectionDuration = 38/robotSpeed;
    if (WideRight.is_White() || WideLeft.is_White()){
      if (currentMillis - previousMillis > 0.9 * expectedSectionDuration){
        if (block_location == 1){
          collect();
        }
        prev_position = position; position = next_position; next_position = 10;
        previousMillis = currentMillis;
      }
    } else {
      follow_line();
    }
  }
  
  else if(position == 9){ // Crossing the cross
    if(WideLeft.is_Black() && WideRight.is_Black()){
        prev_position = position; position = next_position; next_position = 11;
        previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }
  
  else if (position == 10){ // Follow the straight line after the cross
    expectedSectionDuration = 38/robotSpeed;
    if (WideLeft.is_White() && currentMillis - previousMillis > 0.9 * expectedSectionDuration){ 
      prev_position = position; position = next_position;
      if (block_location == 3){
        next_position = 20;
      } else {
        next_position = 12;
      }
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }

  else if (position == 11){ // cross the junction after the ramp
    if (prev_position == 10 && next_position == 12){
      if (WideLeft.is_Black()) {
        prev_position = position; position = next_position; next_position = 15;
        previousMillis = currentMillis;
      } else {
        follow_line();
      }
    } else if (prev_position == 10 && next_position == 20){
      if((currentMillis - previousMillis) > 250){
        turn_left_90();
        prev_position = position; position = next_position; next_position = 20;
        previousMillis = currentMillis;
      }
    } else if (prev_position == 20 && next_position == 12){
      if((currentMillis - previousMillis) > 250){
        turn_left_90();
        prev_position = position; position = next_position; next_position = 15;
        previousMillis = currentMillis;
      }
    }
  }

  else if(position == 12){ // Follow the path under the tunnel
    long tunnelDistance = get_tunnel_distance();
    WideLeft.threshold = 400;
    WideRight.threshold = 400;
    if (Left.is_Black() && Right.is_Black() && tunnelDistance > 100){
      motor_forward(255, 255 - left_offset);
    } else if(Left.is_Black() && Right.is_Black() && tunnelDistance < 100){
      tunnel_drive();
    } else if(WideRight.is_White()){
      prev_position = position; position = next_position;
      if (in_grabber == "low"){
        next_position = 21;
      } else{
        next_position = 16;
      }
      previousMillis = currentMillis;
      WideLeft.threshold = 600;
      WideRight.threshold = 600;
    } else {
        follow_line();
    }
  }

  else if(position == 15){ // Cross the junction leading to the red box
    if (prev_position == 12 && next_position == 16){
      if (WideRight.is_Black()){
        prev_position = position; position = next_position; next_position = 3;
        previousMillis = currentMillis;
      } else{
        follow_line();
      }
    } else if (prev_position == 12 && next_position == 21){
      if((currentMillis - previousMillis) > 250){
        turn_right_90();
        prev_position = position; position = next_position; next_position = 22;
        previousMillis = currentMillis;
      }
    } else if (prev_position == 21 && next_position == 16){
      if((currentMillis - previousMillis) > 250){
        turn_right_90();
        prev_position = position; position = next_position; next_position = 3;
        previousMillis = currentMillis;
      }
    }
  }
  
  else if (position == 16) {
    expectedSectionDuration = 68/robotSpeed;
    if (WideRight.is_White() && currentMillis - previousMillis > 0.9 * expectedSectionDuration) {
      prev_position = position; position = next_position; next_position = 4;
      previousMillis = currentMillis;
    } else {
      follow_line();
    }
  }

  else if (position == 17){ // Moving along the short white line
    expectedSectionDuration = 16/robotSpeed;
    if(WideRight.is_White() || WideLeft.is_White()){
      if (currentMillis - previousMillis > 0.9 * expectedSectionDuration) {
        prev_position = position; position = next_position; next_position = 18;
        previousMillis = currentMillis;
      }
    } else {
      follow_line();
    }
  }

  else if (position == 18){ // crossing perpendicular line to deposit high density block
    if (prev_position == 17 && next_position == 18){
      if(WideRight.is_Black() || WideLeft.is_Black()){
        deposit();
        prev_position = position; position = next_position; next_position = 17;
        previousMillis = currentMillis;
      }
    } else if (prev_position == 18 && next_position == 17){
      if(WideRight.is_Black() || WideLeft.is_Black()){
        prev_position = position; position = next_position; next_position = 5;
        previousMillis = currentMillis;
      }
    }
  }

  else if (position == 19){
    expectedSectionDuration = 18/robotSpeed;
    if (prev_position == 7 && next_position == 19){
      if (Left.is_Black() && Right.is_Black()){
        if (currentMillis - previousMillis > 0.9 * expectedSectionDuration){
          collect();
          prev_position = position; position = next_position; next_position = 7;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    } else if (prev_position == 19 && next_position == 7){
      if (WideLeft.is_White() || WideRight.is_White()){
        if (currentMillis - previousMillis > 0.9 * expectedSectionDuration){
          prev_position = position; position = next_position; next_position = 8;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    }
  }

  else if (position == 20){
    expectedSectionDuration = 18/robotSpeed;
    if (prev_position == 11 && next_position == 20){
      if (Left.is_Black() && Right.is_Black()){
        if (currentMillis - previousMillis > 0.9 * expectedSectionDuration){
          collect();
          prev_position = position; position = next_position; next_position = 11;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    } else if (prev_position == 20 && next_position == 11){
      if (WideLeft.is_White() || WideRight.is_White()){
        if (currentMillis - previousMillis > 0.9 * expectedSectionDuration){
          prev_position = position; position = next_position; next_position = 12;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    }
  }

  else if (position == 21){ // Moving along the short white line
    expectedSectionDuration = 16/robotSpeed;
    if (prev_position == 15 && next_position == 22){
      if(WideRight.is_White() || WideLeft.is_White()){
        if (currentMillis - previousMillis > 9 * expectedSectionDuration){
          prev_position = position; position = next_position; next_position = 22;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    } else if (prev_position == 22 && next_position == 15){
      if(WideRight.is_White() || WideLeft.is_White()){
        if (currentMillis - previousMillis > 9 * expectedSectionDuration){
          prev_position = position; position = next_position; next_position = 16;
          previousMillis = currentMillis;
        }
      } else {
        follow_line();
      }
    }
  }

  else if (position == 22){ // crossing perpendicular line to deposit high density block
    if (prev_position == 21 && next_position == 22){
      if(WideRight.is_Black() || WideLeft.is_Black()){
        deposit();
        prev_position = position; position = next_position; next_position = 21;
        previousMillis = currentMillis;
      }
    } else if (prev_position == 22 && next_position == 21){
      if(WideRight.is_Black() || WideLeft.is_Black()){
        prev_position = position; position = next_position; next_position = 15;
        previousMillis = currentMillis;
      }
    }
  }

  delay(2);
}
