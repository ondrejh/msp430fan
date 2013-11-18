#! /usr/bin/env python3

import pymysql
from ebio_config import *
#from pylab import *
import sys
import datetime
import subprocess

def temp2float(t):
    if t[-1] == 'C':
        t = t[:-1]
        try:
            retval=float(t)
            return retval
        except:
            return None
    return None

def fuse2int(f):
    if f=='ON':
        return 1
    elif f=='OFF':
        return 0
    return None

def heating2int(h):
    if h[-1]=='1':
        return 1
    elif h[-1]=='2':
        return 2
    elif h[-1]=='3':
        return 3
    return 0

def split_log(splitinterval=datetime.timedelta(seconds=15)):
    ''' it should search log database table for pause
    if pause found, older logs are moved into log_YYYYMMDD_HHMM table
    and removed from actual table.
    splitinterval: length of pause'''

    if type(splitinterval)!=datetime.timedelta:
        splitinterval=datetime.timedelta(seconds=15)

    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    #get timestamps
    sql = '''select tstamp from {}.{} order by tstamp desc'''.format(db_name,db_log_table)
    cur.execute(sql)
    dbdata=cur.fetchall()

    #find next pause
    tstop = None
    for i in range(1,len(dbdata)):
        if (dbdata[i-1][0]-dbdata[i][0])>splitinterval:
            tstop=dbdata[i][0]
            break
    #print(tstop)

    #get older data log timestamp
    sql = '''select tstamp from {}.{} where tstamp<=\'{}\' order by tstamp;'''.format(db_name,db_log_table,tstop)
    cur.execute(sql)
    dbdata=cur.fetchall()
    tstart=dbdata[0][0]

    #create table with older log data
    olderlogname = tstart.strftime('log_%Y%m%d_%H%M')
    sql = '''create table {}.{} select * from {}.{} where tstamp<=\'{}\';'''.format(db_name,olderlogname,db_name,db_log_table,tstop)
    cur.execute(sql)

    #delete older rows in current log
    sql = '''delete from {}.{} where tstamp<=\'{}\';'''.format(db_name,db_log_table,tstop)
    cur.execute(sql)

    #close database
    cur.close()
    conn.close()

def save_to_datfile(tablename=db_log_table, whattoshow=['t1','t2','t3','t4','t5'], showinterval=None, filename=None, showlegend=True):
    ''' create chart out of db data
    arguments:
      whattoshow .. list of columns to show (t1 .. t5) (list of strings)
      showinterval .. time interval (datetime.interval or None) of data to show (if None .. not used)
      filename .. filename to store chart (string or None) (if None it is displayed)
      showlegend .. show legend in chart (bool)'''

    #connect to db
    conn = pymysql.connect(host=db_host,user=db_user,passwd=db_pass)
    conn.autocommit(True)
    cur = conn.cursor()

    if whattoshow==None:
        whattoshow=['t1','t2','t3','t4','t5','f','h']
    sqlwhat=''
    strlegend=[]
    for s in whattoshow:
        sqlwhat+=',{}'.format(s)
        if s=='t5':
            strlegend.append('mcu')
        elif s=='f':
            strlegend.append('fuse')
        elif s=='h':
            strlegend.append('heating')
        else:
            strlegend.append(s)
    if sqlwhat=='':
        print('No data to show selected')
    else:
        sqlwhat = sqlwhat[1:]
    sqlwhat += ',tstamp'
    sql = '''select {} from {}.{} order by tstamp desc'''.format(sqlwhat,db_name,tablename)
    if showinterval!=None:
        sql += ''' where tstamp>\'{}\';'''.format((datetime.datetime.now()-showinterval).strftime('%Y-%m-%d %H:%M:%S'))
    #print(strlegend)
    
    #get data
    cur.execute(sql)
    dbdata = cur.fetchall()

    #close database
    cur.close()
    conn.close()

    #use data
    data=[]
    tim = []
    for i in range (0,len(dbdata)):
        tact = dbdata[i][len(strlegend)]
        tim.append(tact)
        data.append([])
        for c in range(0,len(strlegend)):
            if strlegend[c]=='fuse':
                data[i].append(fuse2int(dbdata[i][c]))
            if strlegend[c]=='heating':
                data[i].append(heating2int(dbdata[i][c]))
            else:
                data[i].append(temp2float(dbdata[i][c]))

    f = open(filename,'w')

    tend=tim[0]
    tstart=tim[-1]
    dlen = len(data)
    for c in range(0,len(strlegend)):
        f.write('#\"{}\"\n'.format(strlegend[c]))
        for i in range(1,dlen):
            if data[-i][c]!=None:
                f.write('{:.2f} {}\n'.format((tim[-i]-tstart).total_seconds()/60,data[-i][c])) #minutes

        if c!=len(strlegend)-1:
            f.write('\n\n\n')

    f.close()

    #for c in range(0,len(strlegend)):
    #    plot(tim,tmp[c])
    #xlabel('time')
    #ylabel('temperature [°C]')
    #if showlegend:
    #    legend(strlegend)
    #grid(True)
    #if filename==None:
    #    show()
    #else:
    #    savefig(filename)

if __name__ == "__main__":
    save_to_datfile(filename='data.dat',whattoshow=['t1','t2','t5','h','f'])
    subprocess.call(['gnuplot','gnuplot.gp'], shell=True)
