#! /usr/bin/env python3

import pymysql
import fan_comm
from time import sleep

''' serial port setup '''
portname = 'COM7'

''' MySQL server setup:
Assuming database 'ebio' with table 'temp' and 'heating' is present on the server.
Table 'temp' should look like:
+------+-------+
| id   | value |
+------+-------+
|    1 | 22.5C |
|    2 | 22.4C |
|    3 | ---   |
|    4 | ---   |
|    5 | ---   |
+------+-------+
where 'id' is type int and 'value' is type varchar.
Table 'heating' should look like:
+---------+--------+
| request | status |
+---------+--------+
| OFF     | OFF    |
+---------+--------+
Settings:'''
db_host = '192.168.113.80'
db_user = 'root'
db_pass = '1234'
db_name = 'ebio'
db_temp_table = 'temp'
db_heating_table = 'heating'

while True:
    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    #read temperatures from table
    cur.execute('SELECT * FROM {}.{}'.format(db_name,db_temp_table))
    tdb = cur.fetchall()

    #read output request and status from table
    cur.execute('SELECT * FROM {}.{}'.format(db_name,db_heating_table))
    hdb = cur.fetchall()[0]
    #print(hdb)
    
    #read actual real temperatures and output status
    val = fan_comm.comm(portname,['T1?\n','T2?\n','T3?\n','T4?\n','T5?\n','H?\n'])
    #print(val)

    #compare and update temperatures
    for i in range(0,5):
        #temperatures
        if tdb[i][1]!=val[i]:
            cur.execute('''UPDATE {}.{} SET value='{}' WHERE id={};'''.format(db_name,db_temp_table,val[i],i+1))
    
    #compare and update heating status
    if hdb[1]!=val[5]:
        cur.execute('''UPDATE {}.{} SET status='{}';'''.format(db_name,db_heating_table,val[5]))

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

    sleep(1)    
