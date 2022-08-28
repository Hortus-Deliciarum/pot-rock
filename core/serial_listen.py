#!/usr/bin/env python3
#pylint: disable=too-many-function-args
#pylint: disable=missing-module-docstring
#pylint: disable=missing-function-docstring
#pylint: disable=global-statement

from pathlib import Path
import serial
import toml
from debugs import debug
from pythonosc import udp_client

MAINPATH = Path(__file__).parent.absolute()
CONFIG_PATH = MAINPATH / Path('../config/config.toml')
IP, OUTPORT, DEBUG, ADDRESS = [None] * 4
SERIAL = serial.Serial('/dev/ttyS1', 115200)  # Start serial communication


def init_config():
    """config global labels"""
    global IP, OUTPORT, DEBUG, ADDRESS

    with open(CONFIG_PATH, 'r', encoding='utf-8') as c_file:
        t_data = toml.load(c_file)

    DEBUG = t_data['debug']['DEBUG']
    IP = t_data['network']['IP']
    OUTPORT = t_data['network']['OUT_PORT']
    ADDRESS = t_data['address']['ADDRESS']
    debug(DEBUG, "[ VALUES ]\t", DEBUG, IP, OUTPORT, ADDRESS)

def parse_msg(msg):
    init_config()
    address, datum = msg.split(' ')[:2]
    datatyped = None

    try:
        datatyped = float(datum.strip(None))
    except ValueError:
        datatyped = datum

    print(address, datatyped)
    return (address, datatyped)

def send_msg(_client, _addr, _val):
    if _addr in ADDRESS:
        _client.send_message(_addr, _val)

if __name__ == '__main__':
    client = udp_client.SimpleUDPClient(IP, OUTPORT)

    while True:
        data = SERIAL.readline()
        s = data.decode('ascii')
        addr, value = parse_msg(s)
        send_msg(client, addr, value)
