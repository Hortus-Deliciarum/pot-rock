#!/usr/bin/env python3
#pylint: disable=too-many-function-args
#pylint: disable=missing-module-docstring
#pylint: disable=missing-function-docstring
# pylint: disable=global-statement

from pathlib import Path
import serial
import toml
from debugs import debug
from pythonosc import udp_client

SERIAL_PATH = '/dev/ttyS1'
BAUDRATE = 115200
MAINPATH = Path(__file__).parent.absolute()
CONFIG_PATH = MAINPATH / Path('../config/config.toml')
IP, OUT_PORT, DEBUG, ADDRESS = [None] * 4
SERIAL = serial.Serial(SERIAL_PATH, BAUDRATE)  # Start serial communication


def init_config():
    """config global labels"""
    global IP, OUT_PORT, DEBUG, ADDRESS

    with open(CONFIG_PATH, 'r', encoding='utf-8') as c_file:
        t_data = toml.load(c_file)

    DEBUG = t_data['debug']['DEBUG']
    IP = t_data['network']['IP']
    OUT_PORT = t_data['network']['OUT_PORT']
    ADDRESS = t_data['address']['ADDRESS']
    debug(DEBUG, "[ VALUES ]\t", DEBUG, IP, OUT_PORT, ADDRESS)


def parse_msg(msg):
    init_config()
    address, datum = msg.split(' ')[:2]
    datatyped = None

    try:
        datatyped = float(datum.strip(None))
    except ValueError:
        datatyped = datum

    debug(DEBUG, address, datatyped)
    return (address, datatyped)


def send_msg(_client, _addr, _val):
    if _addr in ADDRESS:
        _client.send_message(_addr, _val)
    else:
        debug(DEBUG, f"EXCLUDED ADDRESS/MESSAGE:\t{_addr}\t{_val}")


if __name__ == '__main__':
    init_config()
    client = udp_client.SimpleUDPClient(IP, OUT_PORT)

    while True:
        data = SERIAL.readline()
        s = data.decode('ascii')
        addr, value = parse_msg(s)
        send_msg(client, addr, value)
