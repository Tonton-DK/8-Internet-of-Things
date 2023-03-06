#!/usr/bin/env python3

import serial
from sys import argv, exit
from datetime import datetime

# guard: args
if len(argv) != 4:
  print("Syntax: %s DEVICE SPEED FILENAME" % argv[0])
  print("        %s /dev/ttyUSB0 115200 output.csv" % argv[0])
  print("        %s COM3 115200 output.csv" % argv[0])
  exit(1)

# command-line arguments
device   =     argv[1]
speed    = int(argv[2])
filename =     argv[3]

# open output file
outputhandle = open(filename, "a")

# open input device
inputhandle = serial.Serial(port=device, baudrate=speed)

# keep on reading
try:
  while(True):
    line = inputhandle.read_until(b'\n').decode('utf-8').strip()
    timestamp = datetime.now().timestamp()
    
    outputhandle.writelines(["%f %s\n" % (timestamp, line)])
    outputhandle.flush()
except Exception as e:
  print("Err: "+str(e))

# close output file
outputhandle.close()
