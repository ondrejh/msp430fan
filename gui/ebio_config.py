#! /usr/bin/env python3

''' serial port setup '''
#portname = 'COM7'
portname = '/dev/ttyACM0'

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
#db_host = '192.168.113.80'
db_host = 'localhost'
db_user = 'root'
#db_pass = '1234'
db_pass = ''
db_name = 'ebio'
db_temp_table = 'temp'
db_heating_table = 'heating'
