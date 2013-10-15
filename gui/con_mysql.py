#! /usr/bin/env python3

import pymysql
import fan_comm
from time import sleep

''' serial port setup '''
portname = 'COM7'

''' MySQL server setup:
Assuming database 'ebio' with table 'temp' is present on the server.
Table 'temp' should look like:
+------+-------+
| id   | value |
+------+-------+
|    1 | 22.5C |
|    2 | 22.4C |
|    3 | ---   |
|    4 | ---   |
+------+-------+
where 'id' is type int and 'value' is type varchar.
Settings:'''
db_host = '192.168.113.80'
db_user = 'root'
db_pass = '1234'
db_name = 'ebio'
db_temp_table = 'temp'

while True:
    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    #read temperatures from table
    cur.execute('SELECT * FROM {}.{}'.format(db_name,db_temp_table))
    tdb = cur.fetchall()
    
    #read actual real temperatures
    tf = fan_comm.read_temp(portname)

    #compare and update
    for i in range(0,4):
        if tdb[i][1]!=tf[i]:
            cur.execute('''UPDATE {}.{} SET value='{}' WHERE id={};'''.format(db_name,db_temp_table,tf[i],i+1))

    sleep(1)    
