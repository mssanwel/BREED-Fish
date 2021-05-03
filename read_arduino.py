#!/usr/bin/env python3

import serial 


def main(args):
    return 0

if __name__ == '__main__':
    ser = serial.Serial('/dev/ttyACM0', 9600, timeout=1)
    ser.flush
    
    while True:
        if ser.in_waiting > 0:
            line = ser.readline().decode('UTF-8').rstrip()
            parse = line.split(': ')
            if(len(parse) > 1 and (parse[0][0] == "C" or parse[0][0] == "R")):
                print(parse)
               # print(parse[1][0] + parse[1][1] + parse[1][2] + parse[1][3])
