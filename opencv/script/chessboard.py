# -*- coding: utf-8 -*-
"""
Created on Wed Jul 24 20:31:23 2019

@author: bobydeng
"""

import numpy as np

#grid_width = 8.95
#grid_height = 10.7
grid_width = 13.2  #118.5/9
grid_height = 15.8  #110.5/7

cols = 9
rows = 6

board_width = grid_width * (cols - 1)
board_height = grid_height * (rows - 1)

grid_size = (cols, rows)

def grid_corner_pos():
    objp = np.zeros((rows * cols, 3), np.float32)
    objp[:,:2] = np.mgrid[0: cols, 0:rows].T.reshape(-1,2)
    objp[:,0] *= grid_width
    objp[:,1] *= grid_height
    return objp
