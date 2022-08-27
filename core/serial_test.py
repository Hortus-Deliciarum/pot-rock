#!/usr/bin/env python3

import serial
from pythonosc import udp_client

ser = serial.Serial('/dev/ttyS1', 115200)  # Start serial communication

if __name__ == '__main__':
    client = udp_client.SimpleUDPClient("127.0.0.1", 8000)

    while True:
        data = ser.readline()
        s = data.decode('ascii')
        print(s)
