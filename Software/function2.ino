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
}

long flash_beacon(int delayDuration, long lastFlashTime){
  // code for flashing the orange beacon when moving
  long startTime = millis();
  if (lastFlashTime == 0){
    long lastFlashTime = millis();
  }
  while ((millis() - startTime) < delayDuration){ // only continue within this function for a specified duration
    if ((millis()- lastFlashTime) > 250){
      digitalWrite(orangeLedPin, (1- orangeLedState));
      orangeLedState = (1- orangeLedState);
      lastFlashTime = millis();
    }
  }
  return lastFlashTime;
}

int foamRecognition() {
  long pingTime, mm;
  double ave_mm = 0; //average distance
  int numberOfCylesForAverage = 4;
  int foamType; // 1 or 0 - high or low
  int DistanceThreshold = 400; // if the distance from sensor is below this value, it will be recognised as low density - if IR is used simultanously, this can be modified and compared with the result from IR sensor

  for (int cycle = 0; cycle < numberOfCylesForAverage; cycle += 1){
    digitalWrite(blockPingPin, LOW);
    delayMicroseconds(2);
    digitalWrite(blockPingPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(blockPingPin, LOW);

    pingTime = pulseIn(blockReceivePin, HIGH);

    mm = distance_in_millimeters(pingTime);
    ave_mm = ave_mm + mm;
    //Serial.print(mm);
    //Serial.print("mm");
    //Serial.println();
    delay(20);     
  }
};

void deposit(){
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(2500);
    //long lastFlashTime = flash_beacon(2500, 0);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    delay(1000);
    //long lastFlashTime = flash_beacon(1000,lastFlashTime);
    grabber.write(grabber_open_position);
    delay(2000);
    //long lastFlashTime = flash_beacon(2000,lastFlashTime);
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(BACKWARD);
    motorRight->run(BACKWARD);
    delay(1000);
    //long lastFlashTime = flash_beacon(1000,lastFlashTime);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(1750);
    //long lastFlashTime = flash_beacon(1750,lastFlashTime);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    grabber.write(grabber_closed_position);
    delay(2000);
    //long lastFlashTime = flash_beacon(2000,lastFlashTime);
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(1500);
    //long lastFlashTime = flash_beacon(1500,lastFlashTime);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
}

void collect(){
      grabber.write(grabber_open_position);
      delay(2000);
      //long lastFlashTime = flash_beacon(2000,0);
      int density = 1
      for (int cycle = 0; cycle < 15; cycle += 1){
        if (foamRecognition() == 0){
          density = 0;
        }
        motorLeft->setSpeed(150);
        motorRight->setSpeed(150);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
        delay(100);
        //long lastFlashTime = flash_beacon(100,lastFlashTime);  
    }
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
    delay(1000);
    //long lastFlashTime = flash_beacon(1000,lastFlashTime);
    grabber.write(grabber_closed_position);
    delay(2000);
    //long lastFlashTime = flash_beacon(2000,lastFlashTime);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(1750);
    //long lastFlashTime = flash_beacon(1750,lastFlashTime);
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(1500);
    //long lastFlashTime = flash_beacon(1500,lastFlashTime);
    motorLeft->run(RELEASE);
    motorRight->run(RELEASE);
}

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
    //long lastFlashTime = flash_beacon(200,0);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(1200);
    //long lastFlashTime = flash_beacon(1200,lastFlashTime);
}

void turn_left_90(){
    delay(200);
    //long lastFlashTime = flash_beacon(200,0);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(225);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(1200);
    //long lastFlashTime = flash_beacon(1200,lastFlashTime);   
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


//block detection from tj - feel free to doubt and make changes as they have not been tested yet
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

    mm = distance_in_millimeters(pingTime);
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

void grab(){ // to be completed!!!! to be integrated with lev's code
    resetDensity();
}

void resetDensity(){ //reset when the robot is on the stright line and ready to detect the block
    density = 1;
}

void moveBackAndForthAndDetect() { //move back and forth to detect the type of the foam
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(BACKWARD);
    motorRight->run(BACKWARD);
    for (int i = 0; i < 40; i++){
      if (foamRecognition() == 0){ //a totay delay of 50 in each foamRocognition()
        density = 0;
      }
      else {
        density = 1;
      }
    }
    motorLeft->setSpeed(150);
    motorRight->setSpeed(150);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
    delay(2100); //slightly greater than 2000 to ensure the foam is in front of the robot
    motorLeft->setSpeed(0);
    motorRight->setSpeed(0);
    LEDforFoamType();
    delay(6000); //The LED should clearly illuminate for >5 seconds while the robot is stationary
  }

void LEDforFoamType() {
  if (density == 1){ // high density - red led on
    digitalWrite(redLedPin, HIGH);
    digitalWrite(greenLedPin, LOW);
  }
  else {
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, HIGH);
  }
}
