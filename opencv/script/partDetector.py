# -*- coding: utf-8 -*-
"""
Created on Wed Jul 10 20:19:46 2019

@author: bobydeng
"""

import cv2
from shapedetector import ShapeDetector
import imutils


class PartDetector:
    def __init__(self):
        self.sd = ShapeDetector()
    
    def detect(self, frame, markInFrame=False):
        resized = imutils.resize(frame, width=300)
        ratio = frame.shape[0] / float(resized.shape[0])
        
        contours = self._find_contour(resized)
        if len(contours) == 0:
            return None, None
        
        contour, center = self._Filter_n_find_center(contours, ratio)
        
        if contour is None:
            return  None, None
        
        shape = self.sd.detect(contour)    
        
        if markInFrame:
            self._mark_object(frame, contour, center, shape, ratio)

        return shape, center

    def _find_contour(self, frame):
        #cv2.imshow('frame',frame)
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        
        #cv2.imshow('gray',gray)
        
        blurred = cv2.GaussianBlur(gray, (5,5),0)
        #blurred = cv.medianBlur(gray, 5)
        
        #cv2.imshow('blurred',blurred)
        
        #clahe = cv2.createCLAHE(clipLimit=2.0, tileGridSize=(8,8))
        #hist_equal = clahe.apply(blurred)
        
        hist_equal = cv2.equalizeHist(blurred)
        
        #cv2.imshow('hist_equal',hist_equal)
        
        ret, thresh = cv2.threshold(hist_equal, 50, 255, cv2.THRESH_BINARY_INV)# + cv2.THRESH_OTSU)
        
        thresh = cv2.adaptiveThreshold(blurred, 255, cv2.ADAPTIVE_THRESH_MEAN_C,\
                    cv2.THRESH_BINARY_INV,11,2)
                    
        thresh = cv2.adaptiveThreshold(blurred, 255, cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
                    cv2.THRESH_BINARY_INV,11,2)
        
        """        
        thresh[:,:60] = 0
        thresh[:,-60:] = 0
        thresh[:60,:] = 0
        thresh[-60:,:] = 0
        """
        
        #cv2.imshow('thresh',thresh)
        
        # Taking a matrix of size 5 as the kernel 
        #kernel = np.ones((3,3), np.uint8) 
          
        # The first parameter is the original image, 
        # kernel is the matrix with which image is  
        # convolved and third parameter is the number  
        # of iterations, which will determine how much  
        # you want to erode/dilate a given image.
        #img_erosion = cv2.erode(thresh, kernel, iterations=1) 
        #thresh = cv2.dilate(img_erosion, kernel, iterations=1) 
        
        #cv2.imshow('erode_dilate',thresh)
        
        cnts = cv2.findContours(thresh.copy(), cv2.RETR_EXTERNAL,
                                cv2.CHAIN_APPROX_SIMPLE)
        cnts = imutils.grab_contours(cnts)
        return cnts
        

    def _Filter_n_find_center(self, cnts, ratio=1):
        for c in cnts:
            M = cv2.moments(c)
            if M["m00"] == 0:
                continue
            cX = int((M["m10"] / M["m00"]) * ratio)
            cY = int((M["m01"] / M["m00"]) * ratio)
            
            area = M['m00']
            if area < 1000:
                continue
            
            return c, [cX, cY]            
            
        return None, None
        
    def _mark_object(self, frame, contour, center, shape, ratio=1):
        cv2.circle(frame, tuple(center), 5, (0,255,0))
       
        c = contour.astype("float")
        c *= ratio
        c = c.astype("int")
        cv2.drawContours(frame, [c], -1, (0, 255 ,0), 2)
        cv2.putText(frame, shape, tuple(center), cv2.FONT_HERSHEY_SIMPLEX,
                    0.5, (255,255,255), 2)
                    
        cv2.imshow("Image", frame)
    

if __name__ == '__main__':
    #fname = '/home/bobydeng/myworks/test/shape_detection.jpg'
    testFileFolder = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/bigboard/"
    fname = testFileFolder + 'board104.jpg'
    frame = cv2.imread(fname)
    dt = PartDetector()
    mark = False
    shape, center = dt.detect(frame, mark)
    print shape, center
    if mark:
        cv2.waitKey(0)
        cv2.destroyAllWindows()
