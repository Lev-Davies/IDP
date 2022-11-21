class LineSensor {
    public:
    const int pin;
    const int threshold;

    bool is_White(){
        return analogRead(pin) <= threshold;
    }
    bool is_Black(){
        return analogRead(pin) >= threshold;
    }
}

void follow_line(){
  if (analogLeft <= lineThresholdLeft && analogRight <= lineThresholdRight) {
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (analogLeft <= lineThresholdLeft && analogRight >= lineThresholdRight){ // or use the difference in analog reading to decide whether we need a left or right turn
    motorLeft->setSpeed(20);
    motorRight->setSpeed(220);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (analogLeft >= lineThresholdLeft && analogRight <= lineThresholdRight){
    motorLeft->setSpeed(220);
    motorRight->setSpeed(20);
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  } else if (analogLeft >= lineThresholdLeft && analogRight >= lineThresholdRight) {
    Serial.println("Ultrasonic mode");
    motorLeft->run(FORWARD);
    motorRight->run(FORWARD);
  }
  delay(10);
}

void turn_right_90(){
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(FORWARD);
    motorRight->run(BACKWARD);
    delay(875); 
}

void turn_left_90(){
    delay(200);
    motorLeft->setSpeed(255);
    motorRight->setSpeed(255);
    motorLeft->run(BACKWARD);
    motorRight->run(FORWARD);
    delay(875);    
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
