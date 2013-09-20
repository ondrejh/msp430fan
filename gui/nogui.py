#! /usr/bin/env python3

import fan_comm
import xml.etree.ElementTree as etree

portname = 'COM7'
filename = 'ebio.xml'

DefaultXML = \
"""<?xml version='1.0' encoding='utf-8'?>
<ebio>
    <Temp T1="---" T2="---"/>
    <Out Request="OFF" Status="---" />
</ebio>\n"""

def ReadValues(port):
    ''' get values from module (via serial port)'''
    
    retVal = {}

    #temperature
    for i in [1,2]:
        answ = fan_comm.read_temp(port,i)
        retVal['T{}'.format(i)] = answ

    #output status
    answ = fan_comm.read_out(port)
    retVal['OUT'] = answ

    return retVal

def OpenXML(file):
    ''' open xml file or create one
    return: xml tree '''
    
    try:
        tree = etree.parse(file)
    except:
        f = open(file,'w')
        f.write(DefaultXML)
        f.close()

    tree = etree.parse(file)

    return tree

def UpdateXML(tree,port):
    ''' update xml file by read values
    return: updated xml tree '''

    values = ReadValues(port)

    root = tree.getroot()
    root.find('Temp').set('T1',values['T1'])
    root.find('Temp').set('T2',values['T2'])
    root.find('Out').set('Status',values['OUT'])

    request = root.find('Out').get('Request')
    if values['OUT'].split(' ')[-1]!=request:
        fan_comm.set_out(port,request)

    return tree

def SaveXML(tree,file):
    ''' save xml tree into file '''
    
    tree.write(file)

def OpenLog(logfilename):
    ''' open log file or create one
    return: file descriptor '''

    try:
        logfile = open(logfilename, mode='a')
        return logfile
    except:
        logfile = open(logfilename, mode='w')
        logfile.writeln('{}'.format(time.localtime))
        

SaveXML(UpdateXML(OpenXML(filename),portname),filename)
