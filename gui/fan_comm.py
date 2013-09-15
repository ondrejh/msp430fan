#! /usr/bin/env python3

from serial import Serial

def comm(portname,command):
    ''' send command and receive answer on specified port '''

    with Serial(portname,baudrate=9600,timeout=0.1) as port:
        port.write(command.encode('ascii'))
        return port.readlines()[-2].strip().decode('ascii')

def read_temp(portname):
    ''' read temperature '''

    return comm(portname,'@A1:T?\n')

def read_out(portname):
    ''' read ouput status '''

    return comm(portname,'@A1:P?\n')

def set_out(portname,outval):
    ''' set output status '''

    return comm(portname,'@A1:P {}\n'.format(outval))

if __name__ == '__main__':
    print(comm('/dev/ttyACM0','@A1:T?\n'))

