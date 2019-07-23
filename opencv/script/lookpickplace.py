# -*- coding: utf-8 -*-
"""
Created on Sat Jul 20 20:23:28 2019

@author: bobydeng
"""

import cv2
from camera import Camera
from robot import Robot
from worldRobotRel import WorldRobotRel
from cameraWorldRel import CameraWorldRel
from partDetector import PartDetector

class LPP:
    def __init__(self, configFile):
        self._loadConfig(configFile)
        self.camera = Camera(self.camera_setting, self.camera_param)
        self.robot = Robot(self.robot_param)
        self.partDetector = PartDetector()
        self.cwRel = CameraWorldRel(self.camera.getParams()[0], self.pnpParamFile)
        self.wrRel = WorldRobotRel()
        self.up_z = -30
        self.down_z = -40
        self.robot.move_updown(self.up_z)

    def __destroy__(self):
        self.camera = None
        self.robot = None

    def _loadConfig(self, configFile):
        self.camera_setting = "camera_setting.yaml"
        self.camera_param = 'cameraParam.npz'
        self.robot_param = 'robotParam.yaml'
        self.pnpParamFile = 'pnpParams.npz'
        self.circle_part_to = (170, -50)
        self.square_part_to = (150, -70)
        self.other_part_to = (140, -80)

    def run(self):
        #print 'lpp run', threading.currentThread().ident               
        testFileFolder = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/bigboard/"
        fname = testFileFolder + 'board104.jpg'
        img1 = cv2.imread(fname)
        while True:
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
            #img = self.camera.capture()
            img = img1.copy()
            shape, center = self.partDetector.detect(img, True)
            cv2.imshow('img',img)
            if shape is None:
                continue
            
            print 'shape:' , shape
            wp = self.cwRel.img2World(center)
            r_x, r_y = self.wrRel.world2Robot(wp[0], wp[1])
            self.robot.move_to(r_x, r_y)
            self.robot.move_updown(self.down_z)
            self.robot.pick()
            self.robot.move_updown(self.up_z)
            if shape == 'cirlce':
                self.robot.move_to(self.circle_part_to[0], self.circle_part_to[1])
            elif shape == 'square':
                self.robot.move_to(self.square_part_to[0], self.square_part_to[1])
            else:
                self.robot.move_to(self.other_part_to[0], self.other_part_to[1])
            self.robot.move_updown(self.down_z)
            self.robot.place()
            self.robot.move_updown(self.up_z)

        cv2.destroyAllWindows()
    
if __name__ == '__main__':
    lpp = LPP('lppConfig.yaml')
    lpp.run()
    lpp = None
