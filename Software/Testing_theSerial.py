
import serial #Serial imported for Serial communication

import time #Required to use delay functions

 

ArduinoSerial = serial.Serial('com7',9600) #Create Serial port object called arduinoSerialData INSERT COM INTO 'COM18'

time.sleep(2) #wait for 2 secounds for the communication to get established


print(ArduinoSerial.readline()) #read the serial data and print it as line

print ("Enter 1 to turn ON LED and 0 to turn OFF LED")


 

while 1: #Do this forever


    var = input() #get input from user

    print ("you entered", var) #print the intput for confirmation

    

    if (var == '1'): #if the value is 1

        ArduinoSerial.write(var.encode()) #send 1

        print ("LED turned ON")

        time.sleep(1)

    

    if (var == '0'): #if the value is 0

        ArduinoSerial.write(var.encode()) #send 0

        print ("LED turned OFF")

        time.sleep(1)