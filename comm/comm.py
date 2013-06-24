from serial import Serial
from time import localtime

portname = 'COM7'
timeout = 0.1

def smalltalk(saywhat,portname=portname,timeout=timeout):
    ''' function should open port sends "saywhat" string
    and listens and returns the answer '''
    with Serial(portname,9600,timeout=timeout) as port:
        port.write(((saywhat.strip()+'\n').encode('ascii')))
        return(port.readlines())

def decompose(answer):
    ''' function should decompose answer returned from serial
    port by smalltalk function'''
    answ = []
    for line in answer:
        cline = line.strip()
        if len(cline)>0:
            answ.append(cline.decode('ascii'))
    return answ
    
def read_temperatures():
    ''' function calls smalltalk/decompose with gettemps string
    and digs out temperatures (returned in list if success) '''
    answ = decompose(smalltalk('T?'))
    if len(answ)<4:
        return None
    temps = []
    for item in answ:
        if item!='T?':
            try:
                temps.append(float(item[2:-1]))
            except:
                temps.append='---'
    return temps

wdays = ['MON','TUE','WED','THU','FRI','SAT','SUN']

def str2wday(wdaystr):
    ''' function converts 3 letters dayow shorts to number
    return 0..6 if success -1 if not '''
    for i in (range(len(wdays))):
        if wdays[i]==wdaystr:
            return i
    return -1

def wday2str(wday):
    ''' function convets day of week number (0..6) to 3 letters day shorts '''
    try:
        return wdays[wday]
    except:
        return '---'

def read_time():
    ''' function calls smalltalk/decompose with gettime string
    and digs out device time (return [wday,hour,minute,second] if success)'''
    answ = decompose(smalltalk('t?'))
    if len(answ)<2:
        return None
    return [str2wday(answ[1][0:3]),int(answ[1][4:6]),int(answ[1][7:9]),int(answ[1][10:12])]

def set_time(wday,hour,mins,sec):
    ''' function sets the device time
    return OK if success or None if fails '''
    s = 't {:3s} {:02d}:{:02d}:{:02d}'.format(wday2str(wday),hour,mins,sec)
    answ = decompose(smalltalk(s))
    if len(answ)<2:
        return None
    else:
        return answ[1]

def test_time():
    ''' function reads device time and compares it with system time
    returns difference in seconds '''
    tDev = read_time()
    if tDev==None:
        return None
    tSys = localtime()
    SecOfWeekSys = tSys.tm_sec+60*tSys.tm_min+3600*tSys.tm_hour+24*3600*tSys.tm_wday
    SecOfWeekDev = tDev[3]+60*tDev[2]+3600*tDev[1]+24*3600*tDev[0]
    SecByWeek = 7*24*3600
    return (SecOfWeekSys-SecOfWeekDev)%SecByWeek

def set_local_time():
    ''' function sets local time to the device '''
    tSys = localtime()
    return set_time(tSys.tm_wday,tSys.tm_hour,tSys.tm_min,tSys.tm_sec)

''' test if time set and set it if not '''
td = test_time()
if abs(td)>5: #if device time differs from system time
    print(set_local_time()) #setup device time
    print('device time is correct now')
else:
    print('device time is {}s off'.format(td))
