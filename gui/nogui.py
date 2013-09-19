#! /usr/bin/env python3

import fan_comm
import xml.etree.ElementTree as etree

portname = 'COM7'
filename = 'ebio.xml'

DefaultXML = """
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

def UpdateXML(file,port):
    ''' update xml file by read values '''

    try:
        tree = etree.parse(file)
    except:
        f = open(file,'w')
        f.write(DefaultXML)
        f.close()

    values = ReadValues(port)

    tree = etree.parse(file)
    root = tree.getroot()
    root.find('Temp').set('T1',values['T1'])
    root.find('Temp').set('T2',values['T2'])
    root.find('Out').set('Status',values['OUT'])

    request = root.find('Out').get('Request')
    if values['OUT'].split(' ')[-1]!=request:
        fan_comm.set_out(port,request)

    tree.write(file)

UpdateXML(filename,portname)
