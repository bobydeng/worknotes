# -*- coding: utf-8 -*-
"""
Created on Sat Jul 20 17:03:50 2019

@author: bobydeng
"""

from robot import Robot

class WorldRobotRel:
    def __init__(self):
        self.x_offset = 200
        self.y_offset = -30
    
    def world2Robot(self, x, y):
        return self.x_offset -x, self.y_offset + y
    
    def setChessBoardPosition(self, robot):
        robot.move_updown(self.x_offset, self.y_offset)
        raw_input("please move chessboard origin to tooltip, press enter to continue")
        robot.move_to(self.x_offset, -self.y_offset)
        raw_input("please rotate chessboard around origin to align tooltip with y axis, press enter to continue")
        robot.move_to_joint(-90,30,-30)

if __name__ == '__main__':
    robot = Robot('robotParam.yaml')
    wrRel = WorldRobotRel()
    wrRel.setChessBoardPosition(robot)
    
    robot = None
