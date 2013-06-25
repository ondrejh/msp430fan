#!/usr/bin/env python3

from time import strftime,sleep
from comm import set_local_time,test_time

LogFileName = 'pool.log'

def write_log(LogFileName,LogToWrite):
    ''' write log into end of the file '''
    if LogToWrite[-1]!='\n': #append EOLn if it's not
        LogToWrite='{}\n'.format(LogToWrite)
    logfile = open(LogFileName,'at')
    logfile.write(LogToWrite)
    logfile.close()

def write_log_with_time(LogFileName,LogToWrite):
    ''' write log with datetime stamp '''
    LogToWrite = '{}: {}'.format(strftime('%m%d_%H%M%S'),LogToWrite)
    write_log(LogFileName,LogToWrite)

if __name__ == '__main__':

    write_log_with_time(LogFileName,'START')

    while True:
        dt = test_time()
        if dt==None:
            write_log_with_time(LogFileName,'Cant test device time settings')
        elif abs(dt)>=10:
            write_log_with_time(LogFileName,'Setting time .. {}'.format(set_local_time()))

        sleep(300) #sleep 5 mins
