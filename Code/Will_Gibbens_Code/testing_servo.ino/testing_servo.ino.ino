#include <Servo.h>

Servo grabber;  // create servo object to control a servo

int val;    // variable to read the value from the analog pin
int i = 1;
const int closed_position = 80;
const int open_position = 140;

void setup() {
  Serial.begin(9600);
  grabber.attach(9);  // attaches the servo on pin 9 to the servo object
}


void loop() {
  grabber.write(open_position);
  delay(2000);
  grabber.write(closed_position);
  delay(2000);
}
