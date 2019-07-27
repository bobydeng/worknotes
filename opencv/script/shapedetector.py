# -*- coding: utf-8 -*-
"""
Created on Wed Jul 10 21:35:51 2019

@author: bobydeng
"""
import cv2
import math

class ShapeDetector:
    def __init__(self):
        pass
    
    def detect(self,c):
        shape = None
        peri = cv2.arcLength(c, True)
        approx = cv2.approxPolyDP(c, 0.04 * peri, True)
        k = cv2.isContourConvex(approx)      
        if k == False:
            return shape
        
        #print 'approx edges', len(approx)
        if len(approx) == 3:
            shape = "triangle"
            
        elif len(approx) == 4:
            (x,y,w,h) = cv2.boundingRect(approx)
            ar = w/float(h)
            
            shape = "square" if ar >=0.95 and ar <= 1.05 else "rectangle"
         
        elif len(approx) == 5:
            shape = "pentagon"
            
        elif len(approx) > 6:
            area = cv2.contourArea(c)
            circularity = 4*math.pi*area/(peri*peri)
            print 'circurity', circularity
            if circularity > 0.8:
                shape = "circle"
            
        return shape