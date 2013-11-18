#! /usr/bin/env python3

''' test ... script should set power on request and waits until t1
or t2 will reach 80C. Than it should switch the power off. Log will
be used to determine system dynamics. '''

import pymysql
from time import sleep
from ebio_config import *
import os,sys

final_temp = 32.0 #final temperature (watch t1 and t2)
max_mcu_temp = 55.0 #max mcu temp to ensure hotglue wound melt

def temp2float(t):
    if t[-1] == 'C':
        t = t[:-1]
        try:
            retval=float(t)
            return retval
        except:
            return None
    return None

def set_heating_on_request():
    ''' set heating on request in heating table '''

    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    cur.execute('''UPDATE {}.{} SET request='ON 3';'''.format(db_name,db_heating_table))

    cur.close()
    conn.close()

    print('''{}: power on heating set'''.format(os.path.basename(sys.argv[0])))
    
def set_heating_off_request():
    ''' set heating off request in heating table '''

    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    cur.execute('''UPDATE {}.{} SET request='OFF';'''.format(db_name,db_heating_table))

    cur.close()
    conn.close()

    print('''{}: power off heating set'''.format(os.path.basename(sys.argv[0])))

def read_heating_status():
    ''' read heating status from heating table '''

    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    cur.execute('''SELECT status FROM {}.{};'''.format(db_name,db_heating_table))
    ret = cur.fetchall()[0][0]

    cur.close()
    conn.close()

    print('''{}: heating status .. {}'''.format(os.path.basename(sys.argv[0]),ret))
    return ret

def read_temperatures():
    ''' read t1,t2 and mcu temperature '''

    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    cur.execute('''SELECT value FROM {}.{} ORDER BY id;'''.format(db_name,db_temp_table))
    ret = cur.fetchall()
    ret = (ret[0][0],ret[1][0],ret[4][0])

    cur.close()
    conn.close()

    print('''{}: temperatures .. {}'''.format(os.path.basename(sys.argv[0]),ret))
    return ret
   

def main():

    print('''{}: started'''.format(os.path.basename(sys.argv[0])))

    #set heating on
    while True:
        set_heating_on_request()
        sleep(3)
        if read_heating_status()=='ON 3':
            break

    #read temperatures
    while True:
        ret = read_temperatures()
        t1 = temp2float(ret[0])
        t2 = temp2float(ret[1])
        tmcu = temp2float(ret[2])
        if (t1==None) or (t1>final_temp) or (t2>final_temp) or (tmcu>max_mcu_temp):
            break
        sleep(10)

    #set heating off
    while True:
        set_heating_off_request()
        sleep(3)
        if read_heating_status()=='OFF':
            break


if __name__ == "__main__":
    if len(sys.argv)<=1:
        #just run the programm
        main()
        
    else:
        if sys.argv[1]=='start':
            #start main as unix daemon
            import daemon
            with daemon.DaemonContext():
                main()
