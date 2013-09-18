#! /usr/bin/env python3

from tkinter import *
import fan_comm

#portname = '/dev/ttyACM0'
portname = 'COM7'

#application class
class runapp_gui(Frame):
    ''' some gui '''

    def __init__(self,master=None):
        self.root = Tk()
        self.root.title('MSP430')
        Frame.__init__(self,master)
        self.createWidgets()

    def createWidgets(self):
        #temperature frame
        self.frmTemp = Frame(self.root, height=2, bd=1, relief=GROOVE)
        self.frmTemp.pack(fill=Y, padx=2, pady=2, side=LEFT)
        #temperature 1
        self.lblTemp1 = Label(self.frmTemp,text='T1:')
        self.lblTemp1.grid(row=0, column=0, sticky=E, pady=3)
        self.strTemp1 = StringVar()
        self.entTemp1 = Entry(self.frmTemp,
                             textvariable=self.strTemp1,
                             width=10,
                             justify=RIGHT,
                             state=DISABLED)
        self.entTemp1.grid(row=0, column=1)
        self.lblTempU1 = Label(self.frmTemp,text='°C')
        self.lblTempU1.grid(row=0, column=2, sticky=W)
        #temperature 2
        self.lblTemp2 = Label(self.frmTemp,text='T2:')
        self.lblTemp2.grid(row=1, column=0, sticky=E, pady=3)
        self.strTemp2 = StringVar()
        self.entTemp2 = Entry(self.frmTemp,
                             textvariable=self.strTemp2,
                             width=10,
                             justify=RIGHT,
                             state=DISABLED)
        self.entTemp2.grid(row=1, column=1)
        self.lblTempU2 = Label(self.frmTemp,text='°C')
        self.lblTempU2.grid(row=1, column=2, sticky=W)
        #temperature 3
        self.lblTemp3 = Label(self.frmTemp,text='T3:')
        self.lblTemp3.grid(row=2, column=0, sticky=E, pady=3)
        self.strTemp3 = StringVar()
        self.entTemp3 = Entry(self.frmTemp,
                             textvariable=self.strTemp3,
                             width=10,
                             justify=RIGHT,
                             state=DISABLED)
        self.entTemp3.grid(row=2, column=1)
        self.lblTempU3 = Label(self.frmTemp,text='°C')
        self.lblTempU3.grid(row=2, column=2, sticky=W)
        #temperature 4
        self.lblTemp4 = Label(self.frmTemp,text='T4:')
        self.lblTemp4.grid(row=3, column=0, sticky=E, pady=3)
        self.strTemp4 = StringVar()
        self.entTemp4 = Entry(self.frmTemp,
                             textvariable=self.strTemp4,
                             width=10,
                             justify=RIGHT,
                             state=DISABLED)
        self.entTemp4.grid(row=3, column=1)
        self.lblTempU4 = Label(self.frmTemp,text='°C')
        self.lblTempU4.grid(row=3, column=2, sticky=W)
        #heating frame
        self.frmHeating = Frame(self.root, height=2, bd=1, relief=GROOVE)
        self.frmHeating.pack(fill=Y, padx=2, pady=2, side=LEFT)
        self.varAuto = IntVar()
        self.chboxAuto = Checkbutton(self.frmHeating,text='Auto',
                                     variable=self.varAuto,
                                     command=self.AutoClick)
        self.chboxAuto.pack()
        self.varHeating = IntVar()
        self.chboxHeating = Checkbutton(self.frmHeating,text='Heating',
                                        variable=self.varHeating,
                                        command=self.HeatingClick)
        self.chboxHeating.pack()
        #get values
        self.ReadValues()

    def AutoClick(self):
        if self.varAuto.get()==1:
            fan_comm.set_out(portname,'AUTO')
        else:
            if self.varHeating.get()==1:
                fan_comm.set_out(portname,'ON')
            else:
                fan_comm.set_out(portname,'OFF')

    def HeatingClick(self):
        self.varAuto.set(0)
        if self.varHeating.get()==1:
            fan_comm.set_out(portname,'ON')
        else:
            fan_comm.set_out(portname,'OFF')

    def ReadValues(self):
        answ = fan_comm.read_temp(portname,1)
        if answ[-1]=='C':
            answ=answ[0:-1]
        self.strTemp1.set(answ)
        answ = fan_comm.read_temp(portname,2)
        if answ[-1]=='C':
            answ=answ[0:-1]
        self.strTemp2.set(answ)
        answ = fan_comm.read_temp(portname,3)
        if answ[-1]=='C':
            answ=answ[0:-1]
        self.strTemp3.set(answ)
        answ = fan_comm.read_temp(portname,4)
        if answ[-1]=='C':
            answ=answ[0:-1]
        self.strTemp4.set(answ)
        answ=fan_comm.read_out(portname)
        if answ[0]!='?':
            hval=0
            if answ[-1]=='N':
                hval=1
            aval=0
            if answ[0]=='A':
                aval=1
            if self.varAuto.get()!=aval:
                self.varAuto.set(aval)
            if self.varHeating.get()!=hval:
                self.varHeating.set(hval)
        self.after(1000,self.ReadValues)

#run application
if __name__ == "__main__":
    app = runapp_gui()
    app.mainloop()
