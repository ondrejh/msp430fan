#!/bin/bash

#the script should copy binary file into raspbi and run another script to flash it

#author: ondrejh.ck@email.cz
#created: 9.6.2013

#expects:
#   rpi with address 10.0.0.177 connected
#   msp430 launchpad connected to rpi
#   there is the "solar" in the "pi"'s home directory
#   there is the "./flash.sh" script in this dir. too
#   flashing launchpad works without root privilegues (on rpi)

scp msp430solar.hex pi@bazen:/home/pi/solar/
ssh pi@10.0.0.177 "cd /home/pi/solar; ./flash.sh"

#flash.sh script content: mspdebug rf2500 "prog msp430solar.hex"
