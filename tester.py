#!/usr/bin/env python
import serial
import time
print ("Hello")

i=0
try:
    arduino = serial.Serial('/dev/tty.usbmodem14101', 115200, timeout = 1)
except:
    print("Connection failed")
print("Connection successful")
while True:
    data = arduino.readline()
    if data:
        try:
            string_values = data.split(",")
            absolute_pressure_floats = []
            for i in range(len(string_values)):
                absolute_pressure_floats.append(float(string_values[i].translate(None, "[] ")))
            print (absolute_pressure_floats)
        except ValueError:
            print("invalid string:")
            print(data)
        i+=1

