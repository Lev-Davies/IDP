// WG Created: 28/11/22 Modified: 28/11/22
// Based off LD "test collect \ deposit"


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
const int grabber_closed_position = 50;
const int grabber_empty_position = 90;
const int grabber_open_position = 110;
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

int foamRecognition() { //return the type of the foam
  long pingTime, mm;
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

    mm = pingTime / 2.9 / 2;
    ave_mm = ave_mm + mm;
    //Serial.print(mm);
    //Serial.print("mm");
    //Serial.println();
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

String collect(int block_position, String in_grabber){
  grabber.write(grabber_open_position);
    String density = "high";
    for (int cycle = 0; cycle < 15; cycle += 1){
      if (foamRecognition() == 0){
        density = "low";
      }
      motorLeft->setSpeed(150);
      motorRight->setSpeed(150);
      motorLeft->run(FORWARD);
      motorRight->run(FORWARD);
      delay(100);
    }
    grabber.write(grabber_closed_position);
  if (block_position == 2 || block_position == 3){
    turn_left_180();
  }
  in_grabber = density;
  return in_grabber;
}

String deposit(String in_grabber){
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(2500);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    grabber.write(grabber_open_position);
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(BACKWARD);
    motorRight->run(BACKWARD);
    delay(1000);
    turn_left_180();
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    grabber.write(grabber_closed_position);
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(1500);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    in_grabber = "empty";
    return in_grabber;
}

void follow_line(){
  //Serial.println("Wide Left: " + String(WideLeft.read()) + " Left: " + String(Left.read()) + " Right: " + String(Right.read()) + " Wide Right: " + String(WideRight.read()));
  if (Left.is_White() && Right.is_White()) { // On track: go straight
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
  } else if (Left.is_White() && Right.is_Black()){ // Too right, go left
    motorLeft->setSpeed(20);
    motorRight->setSpeed(220);
  } else if (Left.is_Black() && Right.is_White()){ // Too left, go right
    motorLeft->setSpeed(220);
    motorRight->setSpeed(20);
 } else if (Left.is_Black() && Right.is_Black() && WideLeft.is_White()) { // Much too right, go left
    motorLeft->setSpeed(20);
    motorRight->setSpeed(230);
  } else if (Left.is_Black() && Right.is_Black() && WideRight.is_White()) { // Much too left, go right
    motorLeft->setSpeed(230);
    motorRight->setSpeed(20);
  } else if (Left.is_Black() && Right.is_Black() && WideLeft.is_Black() && WideRight.is_Black()) { // All black, go forwards blindly
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
  }
  motorLeft->run(FORWARD);
  motorRight->run(FORWARD);
}

void turn_right_90(){
  motorLeft->setSpeed(255);
  motorRight->setSpeed(255 - left_offset);
  motorLeft->run(FORWARD);
  motorRight->run(BACKWARD);
  delay(1000); 
}

void turn_left_90(){
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(1000);
}

void turn_left_180(){
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255 - left_offset);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(2000);
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
  grabber.write(grabber_empty_position);
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
      Serial.println("no button pressed");
      delay(10);
    }
    int block_position = 1;
    String in_grabber = "empty";
    collect(block_position, in_grabber);
  }
  delay(2);
}
