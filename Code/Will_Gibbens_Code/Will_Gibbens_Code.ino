// WG Created: 4/11/22 Modified: 4/11/22
// testing the line sensors

const int emitterPin = 13;                // This is the yellow wire for the diode
const int signalPin =  9;                 // This is the blue wire for the receiver
int sensorState = 0;                      // variable for reading the sensor state
float sensorValue = 0;
int zeros_counter = 0;
int ones_counter = 0;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(emitterPin, OUTPUT);
  pinMode(signalPin, INPUT);
  digitalWrite(emitterPin, HIGH);
}

void loop() {
  // put your main code here, to run repeatedly:
  sensorValue = analogRead(signalPin);
  sensorState = digitalRead(signalPin);
  if (sensorState == 1) {
    ones_counter = ones_counter + 1;
  }
  else if (sensorState == 0) {
    zeros_counter = zeros_counter + 1;
  }
  String outputText = "Value: " + String(sensorValue) + " State: " + String(sensorState) + " Whites: " + String(zeros_counter)+ " Blacks: " + String(ones_counter);
  Serial.println(outputText);
  delay(100);
}
