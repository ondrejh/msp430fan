#! /usr/bin/env python3

from serial import Serial

def comm(portname,command):
    ''' send command and receive answer on specified port '''

    with Serial(portname,baudrate=9600,timeout=0.1) as port:
        if type(command)==str:
            command = [command]
        answers = [];
        for cmd in command:
            port.write(cmd.encode('ascii'))
            answ = port.readlines()
            if len(answ)>1:
                answers += [answ[-2].strip().decode('ascii')]
            else:
                answers += ['???']

        if len(answers)==1:
            return answers[0]

        return answers
            

def read_temp(portname,channel=None):
    ''' read temperature(s) '''

    if channel == None:
        return comm(portname,['T1?\n','T2?\n','T3?\n','T4?\n'])

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

