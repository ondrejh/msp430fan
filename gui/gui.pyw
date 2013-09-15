#! /usr/bin/env python3

from tkinter import *
import fan_comm

portname = '/dev/ttyACM0'

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
        self.frmTemp = Frame(self.root)
        self.frmTemp.pack()
        self.lblTemp = Label(self.frmTemp,text='Temperature:')
        self.lblTemp.grid(row=0, column=0, sticky=E, pady=3)
        self.strTemp = StringVar()
        self.entTemp = Entry(self.frmTemp,
                             textvariable=self.strTemp,
                             width=10,
                             justify=RIGHT,
                             state=DISABLED)
        self.entTemp.grid(row=0, column=1)
        self.lblTempU = Label(self.frmTemp,text='°C')
        self.lblTempU.grid(row=0, column=2, sticky=W)
        self.ReadTemp()
        #slider frame
        self.frmPower = Frame(self.root)
        self.frmPower.pack()
        self.lblPower = Label(self.frmPower,text='Fan:')
        self.lblPower.grid(row=0, column=0, sticky=E, pady=3)
        self.sclPower = Scale(self.frmPower,
                              from_=0,to=100,resolution=1,
                              orient=HORIZONTAL)
        self.sclPower.grid(row=0, column=1, columnspan=3)
        self.btnPowerOn = Button(self.frmPower,
                                 text='On',
                                 command=self.PowerOn)
        self.btnPowerOn.grid(row=1, column=1)
        self.btnSetPower = Button(self.frmPower,
                                  text='Set',
                                  command=self.setPower)
        self.btnSetPower.grid(row=1, column=2)
        self.btnPowerOff = Button(self.frmPower,
                                  text='Off',
                                  command=self.PowerOff)
        self.btnPowerOff.grid(row=1, column=3)
        outstatus = fan_comm.read_out(portname)
        if outstatus == 'ON':
            self.sclPower.set(100)
        elif outstatus == 'OFF':
            self.sclPower.set(0)
        else:
            self.sclPower.set(outstatus[0:-1])

    def setPower(self):
        fan_comm.set_out(portname,self.sclPower.get())
        print('set power {}'.format(self.sclPower.get()))

    def PowerOff(self):
        fan_comm.set_out(portname,'OFF')
        print('power off')
        self.sclPower.set(0)

    def PowerOn(self):
        fan_comm.set_out(portname,'ON')
        print('power on')
        self.sclPower.set(100)

    def ReadTemp(self):
        self.strTemp.set(fan_comm.read_temp(portname)[0:-1])
        self.after(1000,self.ReadTemp)

#run application
if __name__ == "__main__":
    app = runapp_gui()
    app.mainloop()
        
