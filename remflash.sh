#!/bin/bash

#the script should copy binary file into raspbi and run another script to flash it

#author: ondrejh.ck@email.cz
#created: 9.6.2013

#expects:
#   rpi with address 192.168.0.110 connected
#   msp430 launchpad connected to rpi
#   there is the "./flash.sh" script in the home dir. too
#   flashing launchpad works without root privilegues (on rpi)

scp msp430fan.hex pi@192.168.0.110:/home/pi/
ssh pi@192.168.0.110 "cd /home/pi/; ./flash.sh"

#flash.sh script content: mspdebug rf2500 "prog msp430fan.hex"
