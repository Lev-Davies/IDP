void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:
   if (junction_counter == 0 && sensorForwardLeft || junction_counter == 0 && sensorForwardRight)
      {
        //junction code for moving right here, and junction_counter += 1
        motorLeft->setSpeed(255);
        motorRight->setSpeed(0);
        motorLeft->run(FORWARD);
        motorRight->run(FORWARD);
        delay(time_delay_for_junction); //forcing movement out of the white line and into the junction
        while (sensorLeft == 1 && sensorRight == 1)
        {
          motorLeft->setSpeed(255);
          motorRight->setSpeed(0);
          motorLeft->run(FORWARD);
          motorRight->run(FORWARD);
        }
        junction_counter += 1;
      }
}
