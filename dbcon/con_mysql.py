#! /usr/bin/env python3

import pymysql
import fan_comm
from time import sleep,strftime
from ebio_config import *
import sys
import os

# log file
log = False
logfilename = None

print('''{}: started'''.format(os.path.basename(sys.argv[0])))

#connect to db
conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
conn.autocommit(True)
cur = conn.cursor()

cur.execute('''UPDATE {}.{} SET status='STARTING', tstamp=current_timestamp;'''.format(db_name,db_progstat_table))

print('''{}: database connected'''.format(os.path.basename(sys.argv[0])))

while True:

    #read temperatures from table
    cur.execute('SELECT * FROM {}.{}'.format(db_name,db_temp_table))
    tdb = cur.fetchall()
    #print(tdb)

    #read output request and status from table
    cur.execute('SELECT * FROM {}.{}'.format(db_name,db_heating_table))
    hdb = cur.fetchall()[0]
    #print(hdb)

    #read program status table
    cur.execute('SELECT * FROM {}.{}'.format(db_name,db_progstat_table))
    sdb = cur.fetchall()[0]
    #print(sdb)
    
    #read actual real temperatures and output status
    val = fan_comm.comm(portname,['T1?\n','T2?\n','T3?\n','T4?\n','T5?\n','H?\n','F?\n'])
    if (log == False) and (val[6]=='ON'): #start log
        log = True
        logfilename = log_path+strftime('%y%m%d_%H%M_')+log_rootname+'.'+log_extension
    if (log == True): #log
        logfile = open(logfilename,'a')
        logfile.write(val[0]+'; '+val[1]+'; '+val[2]+'; '+val[3]+'; '+val[4]+'; '+val[5]+'; '+val[6]+'\n')
        logfile.close()
    if (log == True) and (val[6]=='OFF') and (float(val[0][:-1])<35.0): #stop log
        log = False

    #compare and update temperatures
    for i in range(0,5):
        #temperatures
        if tdb[i][1]!=val[i]:
            cur.execute('''UPDATE {}.{} SET value='{}' WHERE id={};'''.format(db_name,db_temp_table,val[i],i+1))
    
    #compare and update heating status and fuse
    if hdb[1]!=val[5]:
        cur.execute('''UPDATE {}.{} SET status='{}';'''.format(db_name,db_heating_table,val[5]))
    if hdb[2]!=val[6]:
        cur.execute('''UPDATE {}.{} SET fuse='{}';'''.format(db_name,db_heating_table,val[6]))

    #check if some request (need to change heating status)
    #valid requests:
    #   ON   .. increase heating power by one
    #   ON 1 .. on with power 1
    #   ON 2 .. on with power 2
    #   ON 3 .. on with power 3
    #   OFF  .. off
    #   AUTO .. automatic mode
    if hdb[0]!='':
        #use request
        fan_comm.set_out(portname,hdb[0])
        #clear request
        cur.execute('''UPDATE {}.{} SET request='';'''.format(db_name,db_heating_table))

    #check if program stop request .. if not, update timestamp
    if sdb[0]=='STOP':
        cur.execute('''UPDATE {}.{} SET request='', status='STOPPED';'''.format(db_name,db_progstat_table))
        break
    else:
        cur.execute('''UPDATE {}.{} SET status='RUNNING', tstamp=current_timestamp;'''.format(db_name,db_progstat_table))

    sleep(1)    

cur.close()
conn.close()

print('''{}: stopped'''.format(os.path.basename(sys.argv[0])))
