#! /usr/bin/env python3

from serial import Serial

def comm(portname,command):
    ''' send command and receive answer on specified port '''

    with Serial(portname,baudrate=9600,timeout=0.1) as port:
        port.write(command.encode('ascii'))
        answ = port.readlines()
        if len(answ)>1:
            return answ[-2].strip().decode('ascii')
        return '???'

def read_temp(portname,channel):
    ''' read temperature '''

    if type(channel)!=int:
        print('wrong channel type')
        return
    if int(channel)<1 or int(channel)>4:
        print('channel out of range')
        return

    return comm(portname,'T{}?\n'.format(channel))

def read_out(portname):
    ''' read ouput status '''

    return comm(portname,'H?\n')

def set_out(portname,outval):
    ''' set output status '''

    return comm(portname,'H {}\n'.format(outval))

if __name__ == '__main__':
    #print(comm('/dev/ttyACM0','T1?\n'))
    print(comm('COM7','T1?\n'))

