# -*- coding: utf-8 -*-
"""
Created on Sat Jul 13 21:26:51 2019

@author: bobydeng
"""
import cv2
import numpy as np
import glob


# Load previously saved data
with np.load('cameraParam.npz') as X:
    mtx, dist, _, _ = [X[i] for i in ('mtx','dist','rvecs','tvecs')]

def draw(img, corners, imgpts):
    corner = tuple(corners[0].ravel())
    img = cv2.line(img, corner, tuple(imgpts[0].ravel()), (255,0,0), 5)
    img = cv2.line(img, corner, tuple(imgpts[1].ravel()), (0,255,0), 5)
    img = cv2.line(img, corner, tuple(imgpts[2].ravel()), (0,0,255), 5)
    return img

#grid_width = 8.95
#grid_height = 10.7

grid_width = 13.2  #118.5/9
grid_height = 15.8  #110.5/7
    
criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
objp = np.zeros((6*9,3), np.float32)
objp[:,:2] = np.mgrid[0:9,0:6].T.reshape(-1,2)
objp[:,0] *= grid_width
objp[:,1] *= grid_height

axis = np.float32([[3,0,0], [0,3,0], [0,0,-3]])#.reshape(-1,3)
axis[:,0] *= grid_width
axis[:,1:] *= grid_height

def drawCube(img, corners, imgpts):
    imgpts = np.int32(imgpts).reshape(-1,2)

    # draw ground floor in green
    img = cv2.drawContours(img, [imgpts[:4]],-1,(0,255,0),-3)

    # draw pillars in blue color
    for i,j in zip(range(4),range(4,8)):
        img = cv2.line(img, tuple(imgpts[i]), tuple(imgpts[j]),(255),3)

    # draw top layer in red color
    img = cv2.drawContours(img, [imgpts[4:]],-1,(0,0,255),3)

    return img

cube_axis = np.float32([[0,0,0], [0,3,0], [3,3,0], [3,0,0],
                   [0,0,-3],[0,3,-3],[3,3,-3],[3,0,-3] ])
cube_axis[:,0] *= grid_width
cube_axis[:,1:] *= grid_height

isCube=True

testFileFolder = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/bigboard/"

for fname in glob.glob(testFileFolder + 'board*.jpg'):
    img = cv2.imread(fname)
    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    ret, corners = cv2.findChessboardCorners(gray, (9,6),None)

    if ret == True:
        corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)

        # Find the rotation and translation vectors.
        _, rvecs, tvecs, inliers = cv2.solvePnPRansac(objp, corners2, mtx, dist)

        # project 3D points to image plane
        if isCube:
            imgpts, jac = cv2.projectPoints(cube_axis, rvecs, tvecs, mtx, dist)
            img = drawCube(img,corners2,imgpts)
        else:
            imgpts, jac = cv2.projectPoints(axis, rvecs, tvecs, mtx, dist)
            imgpts, jac = cv2.projectPoints(axis, rvecs, tvecs, mtx, dist)

        cv2.imshow('img',img)
        k = cv2.waitKey(0) & 0xff
        if k == 's':
            cv2.imwrite(fname[:6]+'.png', img)

cv2.destroyAllWindows()
