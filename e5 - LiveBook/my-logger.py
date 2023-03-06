#!/usr/bin/env python3

import serial
from datetime import datetime

# command-line arguments
device   =  "COM3"
speed    =  int(115200)
filename =  "output.csv"

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
