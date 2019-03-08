import re

import serial

port = re.findall("set[(].*_PORT (.*)[)]", open("../CMakeLists.txt", "r").read())[0]
speed = int(re.findall("SERIAL_SPEED ([0-9]*)", open("../src_cpp/connection.h", "r").read())[0])

ser = serial.Serial(port, speed)
while True:
    print(ser.readline())
