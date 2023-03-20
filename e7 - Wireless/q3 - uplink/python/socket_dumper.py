#!/usr/bin/env python3.5
# author: Aslak Johansen <asjo@mmmi.sdu.dk>

from sys import argv
import socket
from datetime import datetime

INTERFACE = '0.0.0.0'
PORT = 8008

def meta   (text): return '\033[1;34m'+text+'\033[1;m'
def escape (text): return '\033[1;31m'+text+'\033[1;m'
def body   (text): return '\033[1;32m'+text+'\033[1;m'
def error  (text): return '\033[1;31m'+text+'\033[1;m'

# binary to string translation hack
def b2s (b):
    s = str(b)[2:-1]
    
    try:
        if len(s)>1:
            s = {
                '\\n': '\n',
                '\\r': '\r',
                '\\\\': '\\',
            }[s]
    except KeyError:
        return error('<'+s+'>')
    
    return s

# guard: command line arguments
if not len(argv) in [2, 3]:
    print('Syntax: %s FILENAME (PORT|)' % argv[0])
    print('        %s logfile.csv' % argv[0])
    print('        %s logfile.csv %d' % (argv[0], PORT))
    exit(1)
if len(argv)==3:
    PORT = int(argv[2])
filename = argv[1]

# setup server socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((INTERFACE, PORT))
s.listen(1)
print('Accepting connections on '+INTERFACE+':'+str(PORT))

# open file
fo = open(filename, "a")

while True:
  try:
    # establish connection
    (client, address) = s.accept()
    print(meta('[[CONNECTION: '+str(address)+']]'))
    
    while True:
      # fetch line
      data = ''
      while len(data)==0 or not data[-1] in ["\n", "\r"]:
        c = b2s(client.recv(1))
        if len(c)==0:
          print('Zero-length return. Breaking connection ...')
          client.close()
          raise Exception()
        data += c
      
      # produce timestamp
      t = datetime.now().timestamp()
      
      # to file
      if len(data.strip())>0:
        fo.writelines(["%f, %s\n" % (t, data.rstrip())])
        fo.flush()
      
      # process characters the ugly way
      line = ""
      for char in data:
        if char in ["\n", "\r"]:
          char = escape({"\n": "<\\\\n>", "\r": "<\\\\r>"}[char])
        line += char
      
      # to screen
      print("%s: %s" %
        (
          body("%f"%t),
          line
        )
      )
  except:
    pass
