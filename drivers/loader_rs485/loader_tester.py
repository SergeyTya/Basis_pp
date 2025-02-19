

import serial
import time

s = serial.Serial('COM3', timeout=1, baudrate=9600)

def send(mes):
    print("--->" + str(mes))
    s.write(mes)
    time.sleep(0.5)
    res = s.read_all()
    print("<---" + str(res))

if __name__ == '__main__':

   
   # send(b"I")
    send(b"A")
    time.sleep(0.1)
    send(b"E")
    
    # binary_data = bytes([0x01, 0x02, 0x03, 0x04])
    # send(binary_data)
    # send(binary_data)
 

   # send(b"M")
   # send(b"V")

    
  