# -*- coding: utf-8 -*-
"""
Created on Wed Jul 10 20:19:46 2019

@author: bobydeng
"""

import numpy as np
import cv2
from shapedetector import ShapeDetector
import imutils

"""
imgfolder = '/home/bobydeng/myworks/go/data/collection/Images/users/dengbo/go'
img = cv2.imread(imgfolder + '/weiqi9.jpg',0)
cv2.imshow('go', img)
cv2.waitKey(0)
cv2.destroyAllWindows()
"""


frame = cv2.imread('/home/bobydeng/myworks/test/shape_detection.jpg')
cv2.imshow('frame',frame)

resized = imutils.resize(frame, width=300)
ratio = frame.shape[0] / float(resized.shape[0])
gray = cv2.cvtColor(resized, cv2.COLOR_BGR2GRAY)
blurred = cv2.GaussianBlur(gray, (5,5),0)
thresh = cv2.threshold(blurred, 60, 255, cv2.THRESH_BINARY)[1]

cv2.imshow('thresh',thresh)

cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL,
                        cv2.CHAIN_APPROX_SIMPLE)
cnts = imutils.grab_contours(cnts)
sd = ShapeDetector()

for c in cnts:
    M = cv2.moments(c)
    if M["m00"] == 0:
        continue
    cX = int((M["m10"] / M["m00"]) * ratio)
    cY = int((M["m01"] / M["m00"]) * ratio)
    shape = sd.detect(c)
    
    c = c.astype("float")
    c *= ratio
    c = c.astype("int")
    cv2.drawContours(frame, [c], -1, (0, 255 ,0), 2)
    cv2.putText(frame, shape, (cX, cY), cv2.FONT_HERSHEY_SIMPLEX,
                0.5, (255,255,255), 2)
                
    cv2.imshow("Image", frame)
    cv2.waitKey(0)
    
cv2.destroyAllWindows()

