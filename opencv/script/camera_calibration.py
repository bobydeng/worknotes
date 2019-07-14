# -*- coding: utf-8 -*-
"""
Created on Sat Jul 13 10:02:49 2019

@author: bobydeng
"""
import numpy as np
import cv2
import glob
#import yaml

patternFilePath = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/"

criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)


"""
0. CV_CAP_PROP_POS_MSEC Current position of the video file in milliseconds.
1. CV_CAP_PROP_POS_FRAMES 0-based index of the frame to be decoded/captured next.
2. CV_CAP_PROP_POS_AVI_RATIO Relative position of the video file
3. CV_CAP_PROP_FRAME_WIDTH Width of the frames in the video stream.
4. CV_CAP_PROP_FRAME_HEIGHT Height of the frames in the video stream.
5. CV_CAP_PROP_FPS Frame rate.
6. CV_CAP_PROP_FOURCC 4-character code of codec.
7. CV_CAP_PROP_FRAME_COUNT Number of frames in the video file.
8. CV_CAP_PROP_FORMAT Format of the Mat objects returned by retrieve() .
9. CV_CAP_PROP_MODE Backend-specific value indicating the current capture mode.
10. CV_CAP_PROP_BRIGHTNESS Brightness of the image (only for cameras).
11. CV_CAP_PROP_CONTRAST Contrast of the image (only for cameras).
12. CV_CAP_PROP_SATURATION Saturation of the image (only for cameras).
13. CV_CAP_PROP_HUE Hue of the image (only for cameras).
14. CV_CAP_PROP_GAIN Gain of the image (only for cameras).
15. CV_CAP_PROP_EXPOSURE Exposure (only for cameras).
16. CV_CAP_PROP_CONVERT_RGB Boolean flags indicating whether images should be converted to RGB.
17. CV_CAP_PROP_WHITE_BALANCE Currently unsupported
18. CV_CAP_PROP_RECTIFICATION Rectification flag for stereo cameras (note: only supported by DC1394 v 2.x backend currently)
"""
def configCamera(cap):
    #set the width and height, and UNSUCCESSFULLY set the exposure time
    cap.set(cv2.CAP_PROP_FRAME_WIDTH,1280)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT,1024)
    cap.set(cv2.CAP_PROP_EXPOSURE, 0.1) #0.5)

    #cap.set(cv2.CAP_PROP_FRAME_WIDTH,2592)
    #cap.set(cv2.CAP_PROP_FRAME_HEIGHT,1944)


def capture_pattern(cameraId):
    cap = cv2.VideoCapture(cameraId)
    #cap.set(cv2.CAP_PROP_SETTINGS, 0); #opens camera properties dialog
    configCamera(cap)
    cnt = 0
    while(True):
        ret, frame = cap.read()
        if ret == True:
            cv2.imshow('frame',frame)        
            
            key = cv2.waitKey(1) & 0xFF
            if  key == ord('r'):
                patternFile = patternFilePath + 'board' + str(cnt) + '.jpg'
                print patternFile
                cv2.imwrite(patternFile, frame)
                cnt += 1
            elif key == ord('q'):
                break
            
            #gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
        
            # Find the chess board corners
            #ret, corners = cv2.findChessboardCorners(gray, (9,6),None)
            ret = False
            # If found, add object points, image points (after refining them)
            if ret == True:
                corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
    
                img = frame.copy()
                # Draw and display the corners
                img = cv2.drawChessboardCorners(img, (9,6), corners2,ret)
                cv2.imshow('img',img)
                key = cv2.waitKey(0) & 0xFF
                if  key == ord('r'):
                    patternFile = patternFilePath + 'board' + str(cnt) + '.jpg'
                    print patternFile
                    cv2.imwrite(patternFile, frame)
                    cnt += 1
                elif key == ord('q'):
                    break
        
        key = cv2.waitKey(1) & 0xFF
        if key == ord('q'):
            break
            
    cap.release()
    cv2.destroyAllWindows()

#capture_pattern(2)

"""
grid size
10.70 x 7
 8.95 x 9

"""
def calibrate():
    # termination criteria
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    
    # prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
    objp = np.zeros((6*9,3), np.float32)
    objp[:,:2] = np.mgrid[0:9,0:6].T.reshape(-1,2)
    objp[:,0] *= 8.95
    objp[:,1] *= 10.7
    
    # Arrays to store object points and image points from all the images.
    objpoints = [] # 3d point in real world space
    imgpoints = [] # 2d points in image plane.
    
    images = glob.glob(patternFilePath + '*.jpg')
    
    for fname in images:
        img = cv2.imread(fname)
        gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    
        # Find the chess board corners
        ret, corners = cv2.findChessboardCorners(gray, (9,6),None)
    
        # If found, add object points, image points (after refining them)
        if ret == True:
            objpoints.append(objp)
    
            corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
            imgpoints.append(corners2)
    
            # Draw and display the corners
            img = cv2.drawChessboardCorners(img, (9,6), corners2,ret)
            cv2.imshow('img',img)
            cv2.waitKey(500)
    
    cv2.destroyAllWindows()
    
    ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1],None,None)
    
    testFile = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/board1.jpg"
    
    img = cv2.imread(testFile)
    h,  w = img.shape[:2]
    newcameramtx, roi=cv2.getOptimalNewCameraMatrix(mtx,dist,(w,h),1,(w,h))
    
    # undistort
    dst = cv2.undistort(img, mtx, dist, None, newcameramtx)
    
    # crop the image
    x,y,w,h = roi
    dst = dst[y:y+h, x:x+w]
    cv2.imwrite('calibresult.png',dst)
    
    
    tot_error = 0
    for i in xrange(len(objpoints)):
        imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[i], tvecs[i], mtx, dist)
        error = cv2.norm(imgpoints[i],imgpoints2, cv2.NORM_L2)/len(imgpoints2)
        tot_error += error
    
    print "mean error: ", tot_error/len(objpoints)

    #data = {'mtx':mtx, 'dist':dist,'rvecs':rvecs,'tvecs':tvecs}
    #with open('cameraParam.yaml','w') as f:
    #    yaml.dump(data, f, default_flow_style=False)
    np.savez('cameraParam.npz', mtx=mtx, dist=dist, rvecs=rvecs, tvecs=tvecs)

calibrate()


def loadCameraParam():
    # Load previously saved data
    with np.load('cameraParam.npz') as X:
        mtx, dist, _, _ = [X[i] for i in ('mtx','dist','rvecs','tvecs')]
        print mtx
        print dist
        
        
loadCameraParam()