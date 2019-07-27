# -*- coding: utf-8 -*-
"""
Created on Sun Jul 21 15:25:52 2019

@author: bobydeng
"""

import math

import serial
import time
import threading
import re

BAUD_RATE = 115200
REPORT_INTERVAL = 1.0 # seconds
STATUS_IDLE = 0
STATUS_RUN = 1
STATUS_HOME = 2
STATUS_OTHER = 100
DEG_PER_RAD = 180/math.pi

UNKNOW_VALUE = 10000

#<Idle|MPos:0.000,0.000,0.000|FS:0,0>
#<Idle|MPos:0.000,0.000,0.000|FS:0,0|WCO:0.000,0.000,0.000>
#<Run|MPos:0.000,0.000,0.000|FS:52,0>
#status_pattern = r'<(\S+)\|WPos:(\S+?),(\S+?),(\S+?)\|FS:(\d+\.*\d*),(\d+\.*\d*).*'
status_pattern = r'<(\S+)\|WPos:(\S+?),(\S+?),(\S+?)\|.*'
gcode_format = 'G0 X{:-.3f} Y{:-.3f} Z{:-.3f}'
gcode_xy_format = 'G0 X{:-.3f} Y{:-.3f}'
gcode_z_format = 'G0 Z{:-.3f}'

def _isVeryClose(a, b):
    return abs(a - b) < (10 ** -2)


class Robot:
    def  __init__(self, robotParamFile, homing=True):
        #print 'robot init thread', threading.currentThread().ident
        self.j0 = UNKNOW_VALUE
        self.j1 = UNKNOW_VALUE
        self.z = UNKNOW_VALUE
        
        self._loadParams(robotParamFile)
        self.sqrlink1 = self.link1len * self.link1len
        self.sqrlink2 = self.link2len * self.link2len        
        
        self.s = serial.Serial(self.com_device_file,BAUD_RATE)
        self.status = STATUS_OTHER
        self.sig_shutdown = False
        self.comReceiverThread = threading.Thread(target=self._com_receiver)
        self.comReceiverThread.daemon = True
        self.comReceiverThread.start()
        
        self.timerThread = threading.Thread(target=self._status_timer)
        self.timerThread.daemon = True
        self.timerThread.start()
        
        if homing:
            self._homing()
       
    def _loadParams(self, robotParamFile):
        self.com_device_file = '/dev/ttyUSB0'
        self.link1len = 140
        self.link2len = 100
        self.init_j0 = -60
        self.init_j1 = 30
        self.init_z = 0#-30
       
    def __del__(self):
        print 'destruct robot'
        self.sig_shutdown = True
        #time.sleep(3)
        self.comReceiverThread.join()
        self.timerThread.join()
        self.s.close() 
      
    def _send_gcode(self, l_block):
        self.s.write(l_block + '\n') # Send g-code block to grbl
        self.s.flushOutput()
        print "SND> : \"" + l_block + "\""
       
    def _homing(self):
        # Wake up grbl
        print "Initializing Grbl..."
        self.s.write("\r\n\r\n")

        self._send_gcode('$H')
        #self._wait_idle(60)
        time.sleep(20)
        self.move_to_joint(self.init_j0, self.init_j1, self.init_z)
       
    def _com_receiver(self):
      while not self.sig_shutdown:
          grbl_out = self.s.readline().strip() # Wait for grbl response
          #print"  MSG: \""+grbl_out+"\""
          if grbl_out.startswith('<') :
              self._extract_status(grbl_out)    
      print 'exits _com_receiver'
          
    def _extract_status(self, grbl_out):
        match = re.search(status_pattern, grbl_out)
        if match is not None:
            [strStatus, j0, j1, z] = match.groups()
            self.j0 = float(j0)
            self.j1 = float(j1)
            self.z = float(z)
            if strStatus == 'Idle':
              self.status = STATUS_IDLE
            elif strStatus == 'Run':
              self.status = STATUS_RUN
            elif strStatus == 'Home':
              self.status = STATUS_HOME
            else: 
              #self.status = STATUS_OTHER
              raise Exception('Error, status: ' + strStatus)

    def _status_timer(self) :
        while not self.sig_shutdown:
          self.s.write('?')
          time.sleep(REPORT_INTERVAL)
        print 'exits _status_timer'
              
    def _wait_reach(self, timeout):
        while timeout > 0:
            if _isVeryClose(self.j0, self.to_j0) and \
                _isVeryClose(self.j1, self.to_j1) and  \
                _isVeryClose(self.z, self.to_z):
                break;
            time.sleep(1)
            timeout -=1
        if timeout == 0:
            raise Exception('move joint timeout ')
            
    def _ik(self, x, y):
        r = math.sqrt(x*x + y*y)
        theta = math.atan2(y,x)
        print theta*DEG_PER_RAD
    
        alpha = math.acos( (r*r + self.sqrlink1 - self.sqrlink2)/(2*r* self.link1len))
        print alpha*DEG_PER_RAD
        beta = theta - alpha
        print beta*DEG_PER_RAD
        sigma = math.acos((self.sqrlink1 + self.sqrlink2 -r*r)/(2 * self.link1len * self.link2len))
        gama = math.pi - sigma
        return [beta*DEG_PER_RAD, gama*DEG_PER_RAD]
       
    def move_to(self, x, y):
        j0, j1 = self._ik(x, y)
        self.to_j0 = j0
        self.to_j1 = j1
        l_block = gcode_xy_format.format(j0, j1)
        self._send_gcode(l_block)
        #self._wait_reach(20)

    def move_updown(self, z):
        self.to_z = z
        l_block = gcode_z_format.format(z)
        self._send_gcode(l_block)
        #self._wait_reach(20)

    def move_to_joint(self, j0, j1, z):
        self.to_j0 = j0
        self.to_j1 = j1
        self.to_z = z
        l_block = gcode_format.format(j0, j1, z)
        self._send_gcode(l_block)
        #self._wait_reach(20)
        
    def pick(self):
        self._wait_reach(20)
        self.bump_on()
        
    def place(self):
        self._wait_reach(20)
        self.bump_off()
        self.valve_on()
        self._holdon(1)
        self.valve_off()
        
    def bump_on(self):
        self._send_gcode('M8')
        #self._holdon(1)
        
    def bump_off(self):
        self._send_gcode('M9')
        
    def valve_on(self):
        self._send_gcode('M7')
        #self._holdon(1)
        
    def valve_off(self):
        self._send_gcode('M9')
        
    def _holdon(self, t):
        self._send_gcode('G4 P' + str(t)) 
        
if __name__ == '__main__':
    """robot = Robot('robotParam.yaml')
    robot.move_updown(-30)
    robot.move_to(200, -30)
    cmd = raw_input("press enter to continue")
    robot.move_to(200, 30)
    cmd = raw_input("press enter to continue")
    robot.move_to_joint(-60,30,-30)
    robot = None
    """
    robot = Robot('robotParam.yaml',False)
    robot._ik(181, -104)
    robot._ik(104, -181)
    robot = None
    