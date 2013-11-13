#! /usr/bin/env python3

import pymysql
from ebio_config import *
from pylab import *
import sys
import datetime

def temp2float(t):
    if t[-1] == 'C':
        t = t[:-1]
        try:
            retval=float(t)
            return retval
        except:
            return None
    return None
        
def show_temperature(datatime=60, filename=None, wsel=[True,True,True,True,True], showlegend=True):

    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    sqlwhat=''
    strlegend=[]
    for c in range(0,len(wsel)):
        if wsel[c]:
            sqlwhat+=',t{}'.format(c+1)
            if c!=4:
                strlegend.append('t{}'.format(c+1))
            else:
                strlegend.append('mcu')
    if sqlwhat=='':
        print('No data to show selected')
    else:
        sqlwhat = sqlwhat[1:]
    sqlwhat += ',tstamp'
    sql = '''select {} from {}.{}'''.format(sqlwhat,db_name,db_log_table)
    if datatime!=None:
        sql += ''' where tstamp>\'{}\';'''.format((datetime.datetime.now()-datetime.timedelta(minutes=datatime)).strftime('%Y-%m-%d %H:%M:%S'))
    print(sql)
    #print(strlegend)
    
    #get data
    cur.execute(sql)
    dbdata = cur.fetchall()

    #close database
    cur.close()
    conn.close()

    #use data
    tmp=[]
    for i in range(0,len(strlegend)):
        tmp.append([])
    tim = []
    for i in range (0,len(dbdata)):
        for c in range(0,len(strlegend)):
            tmp[c].append(temp2float(dbdata[i][c]))
        tim.append(dbdata[i][len(strlegend)])

    for c in range(0,len(strlegend)):
        plot(tim,tmp[c])
    xlabel('time')
    ylabel('temperature [°C]')
    if showlegend:
        legend(strlegend)
    grid(True)
    if filename==None:
        show()
    else:
        savefig(filename)

if __name__ == "__main__":
    show_temperature(wsel=[True])
