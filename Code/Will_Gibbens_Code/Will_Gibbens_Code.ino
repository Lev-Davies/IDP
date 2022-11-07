// WG Created: 4/11/22 Modified: 7/11/22
// testing the line sensors, with four line sensors and a Schmitt Trigger

const int sensorOnePin = 2;
const int sensorTwoPin =  3;
const int sensorThreePin = 4;
const int sensorFourPin = 5;
int sensorOneState = 0;                      // variable for reading thestate of sensor 1
int sensorTwoState = 0;                      // variable for reading thestate of sensor 2
int sensorThreeState = 0;                      // variable for reading thestate of sensor 3
int sensorFourState = 0;                      // variable for reading thestate of sensor 4


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(sensorOnePin, INPUT);
  pinMode(sensorTwoPin, INPUT);
  pinMode(sensorThreePin, INPUT);
  pinMode(sensorFourPin, INPUT);
}

void loop() {
  sensorOneState = digitalRead(sensorOnePin);
  sensorTwoState = digitalRead(sensorTwoPin);
  sensorThreeState = digitalRead(sensorThreePin);
  sensorFourState = digitalRead(sensorFourPin);

  String outputText = "Sensor 1: " + String(sensorOneState) + " Sensor 2: " + String(sensorTwoState) + " Sensor 3: " + String(sensorThreeState) + " Sensor 4: " + String(sensorFourState);
  Serial.println(outputText);
  delay(100);
}
