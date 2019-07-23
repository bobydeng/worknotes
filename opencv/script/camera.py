# -*- coding: utf-8 -*-
"""
Created on Sat Jul 13 10:02:49 2019

@author: bobydeng
"""
import numpy as np
import cv2
import glob
import v4l2
#import yaml

"""
opencv setting
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

v4l2 setting
[{'name': 'Brightness', 'min': -64, 'default': 0, 'max': 64, 'value': 0, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963776}, 
 {'name': 'Contrast', 'min': 0, 'default': 32, 'max': 64, 'value': 32, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963777},
 {'name': 'Saturation', 'min': 0, 'default': 64, 'max': 128, 'value': 64, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963778},
 {'name': 'Hue', 'min': -40, 'default': 0, 'max': 40, 'value': 0, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963779},
 {'name': 'White Balance Temperature, Auto', 'min': 0, 'default': 1, 'max': 1, 'value': 1, 'disabled': False, 'step': 1, 'type': 'bool', 'id': 9963788},
 {'name': 'Gamma', 'min': 72, 'default': 100, 'max': 500, 'value': 100, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963792},
 {'name': 'Gain', 'min': 0, 'default': 0, 'max': 100, 'value': 0, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963795},
 {'name': 'Power Line Frequency', 'min': 0, 'default': 1, 'max': 2, 'value': 1, 'disabled': False, 'step': 1, 'menu': {'Disabled': 0, '60 Hz': 2, '50 Hz': 1}, 'type': 'menu', 'id': 9963800}, 
 {'name': 'White Balance Temperature', 'min': 2800, 'default': 4600, 'max': 6500, 'value': 4600, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963802}, 
 {'name': 'Sharpness', 'min': 0, 'default': 4, 'max': 6, 'value': 4, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963803}, 
 {'name': 'Backlight Compensation', 'min': 0, 'default': 1, 'max': 2, 'value': 1, 'disabled': False, 'step': 1, 'type': 'int', 'id': 9963804}, 
 {'name': 'Exposure, Auto', 'min': 0, 'default': 3, 'max': 3, 'value': 3, 'disabled': False, 'step': 1, 'menu': {'Manual Mode': 1, 'Aperture Priority Mode': 3}, 'type': 'menu', 'id': 10094849},
 {'name': 'Exposure (Absolute)', 'min': 1, 'default': 157, 'max': 5000, 'value': 157, 'disabled': False, 'step': 1, 'type': 'int', 'id': 10094850}, 
 {'name': 'Exposure, Auto Priority', 'min': 0, 'default': 0, 'max': 1, 'value': 1, 'disabled': False, 'step': 1, 'type': 'bool', 'id': 10094851}
]
"""

class Camera:
    def  __init__(self, cameraSettingFile, paramFile):
        self._loadSettings(cameraSettingFile)
        self._configCameraV4L2()    
        self.cap = cv2.VideoCapture(self.cameraId)
        self._configCamera()
        
        if paramFile is not None:
            self._loadCameraParam(paramFile)
    
    def _loadSettings(self, cameraSettingFile):
        self.cameraId = 1
        self.brightness = 0
        self.gamma = 250
        self.exposureAuto = 3
        self.exposureAutoPriority = 1
        self.frameWidth = 1280
        self.frameHeight = 1024
        
    def _loadCameraParam(self, paramFile):
        # Load previously saved data
        """
        with open('./calib/calibration.yaml') as f:
            loadeddict = yaml.load(f)
        self.mtx = loadeddict.get('camera_matrix')
        self.dist = loadeddict.get('dist_coeff')        
        """        
        with np.load(paramFile) as X:
            self.mtx, self.dist, _, _ = [X[i] for i in ('mtx','dist','rvecs','tvecs')]
       
    def capture(self):
        """testFileFolder = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/bigboard/"
        fname = testFileFolder + 'board104.jpg'
        frame = cv2.imread(fname)
        return frame
        """
        ret, frame = self.cap.read()
        return frame
        
    def getParams(self):
        return self.mtx, self.dist

    def getCameraSetting(self):
        print 'width: ' + str(self.cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        print 'height: ' + str(self.cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        print 'fps: ' + str(self.cap.get(cv2.CAP_PROP_FPS))
        print 'fourcc: ' + str(self.cap.get(cv2.CAP_PROP_FOURCC))
        print 'format: ' + str(self.cap.get(cv2.CAP_PROP_FORMAT))
        print 'mode: ' + str(self.cap.get(cv2.CAP_PROP_MODE))
        print 'brightness: ' + str(self.cap.get(cv2.CAP_PROP_BRIGHTNESS))
        print 'contrast: ' + str(self.cap.get(cv2.CAP_PROP_CONTRAST))
        print 'saturation: ' + str(self.cap.get(cv2.CAP_PROP_SATURATION))
        print 'hue: ' + str(self.cap.get(cv2.CAP_PROP_HUE))
        print 'gain: ' + str(self.cap.get(cv2.CAP_PROP_GAIN))
        print 'exposure: ' + str(self.cap.get(cv2.CAP_PROP_EXPOSURE))
        print 'convert rgb: ' + str(self.cap.get(cv2.CAP_PROP_CONVERT_RGB))
        print 'rectification: ' + str(self.cap.get(cv2.CAP_PROP_RECTIFICATION))
    
    
    def _configCamera(self):
        self.cap.set(cv2.CAP_PROP_FRAME_WIDTH, self.frameWidth)
        self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT, self.frameHeight)
        """
        self.cap.set(cv2.CAP_PROP_EXPOSURE, 5000)
    
        self.cap.set(cv2.CAP_PROP_BRIGHTNESS, 1)
        self.cap.set(cv2.CAP_PROP_CONTRAST, 1)
        self.cap.set(cv2.CAP_PROP_SATURATION, 1)
        self.cap.set(cv2.CAP_PROP_HUE, 0)
        self.cap.set(cv2.CAP_PROP_GAIN, 0)
    
        #self.cap.set(cv2.CAP_PROP_FRAME_WIDTH,2592)
        #self.cap.set(cv2.CAP_PROP_FRAME_HEIGHT,1944)
        """

    def _configCameraV4L2(self):
        cap = v4l2.Capture('/dev/video' + str(self.cameraId))
        cap.set_control(9963776, self.brightness)
        cap.set_control(9963792, self.gamma)
        cap.set_control(10094849, self.exposureAuto)
        cap.set_control(10094851, self.exposureAutoPriority)
        cap.close()
        cap = None

    def __destroy__(self):
        self.cap.release()
        
    def _capture_pattern(self, patternFileFolder):
        cnt = 104
        while(True):
            ret, frame = self.cap.read()
            if ret == True:
                cv2.imshow('frame',frame)        
                
                key = cv2.waitKey(1) & 0xFF
                if  key == ord('r'):
                    patternFile = patternFileFolder + 'board' + str(cnt) + '.jpg'
                    print patternFile
                    cv2.imwrite(patternFile, frame)
                    cnt += 1
                elif key == ord('q'):
                    break
                
                """
                gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
            
                # Find the chess board corners
                ret, corners = cv2.findChessboardCorners(gray, (9,6),None)
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
                """
            key = cv2.waitKey(1) & 0xFF
            if key == ord('q'):
                break
                
        cv2.destroyAllWindows()

    def _calibrate(self, patternFileFolder, paramFile):
        #grid_width = 8.95
        #grid_height = 10.7
        
        grid_width = 13.2  #118.5/9
        grid_height = 15.8  #110.5/7
        
        # termination criteria
        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
        
        # prepare object points, like (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
        objp = np.zeros((6*9,3), np.float32)
        objp[:,:2] = np.mgrid[0:9,0:6].T.reshape(-1,2)
        objp[:,0] *= grid_width
        objp[:,1] *= grid_height
        
        # Arrays to store object points and image points from all the images.
        objpoints = [] # 3d point in real world space
        imgpoints = [] # 2d points in image plane.
        
        images = glob.glob(patternFileFolder + '*.jpg')
        
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
        
        ret, self.mtx, self.dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, gray.shape[::-1],None,None)
        
        tot_error = 0
        for i in xrange(len(objpoints)):
            imgpoints2, _ = cv2.projectPoints(objpoints[i], rvecs[i], tvecs[i], self.mtx, self.dist)
            error = cv2.norm(imgpoints[i],imgpoints2, cv2.NORM_L2)/len(imgpoints2)
            tot_error += error
        
        print "mean error: ", tot_error/len(objpoints)
        
        if paramFile is not None:
            #data = {'mtx':self.mtx, 'dist':self.dist,'rvecs':rvecs,'tvecs':tvecs}
            #with open(paramFile,'w') as f:
            #    yaml.dump(data, f, default_flow_style=False)
            np.savez(paramFile, mtx=self.mtx, dist=self.dist, rvecs=rvecs, tvecs=tvecs)
    

    def undistort(self, imgFile):    
        img = cv2.imread(imgFile)
        h,  w = img.shape[:2]
        newcameramtx, roi=cv2.getOptimalNewCameraMatrix(self.mtx, self.dist,(w,h),1,(w,h))
        
        # undistort
        dst = cv2.undistort(img, self.mtx, self.dist, None, newcameramtx)
        
        # crop the image
        x,y,w,h = roi
        dst = dst[y:y+h, x:x+w]
        #cv2.imwrite('calibresult.png',dst)
        return dst
        
if __name__ == '__main__':
    paramFile='cameraParam.npz'
    patternFilePath = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/"
    testFile = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/board1.jpg"
    camera = Camera('setting.yaml', None)
    #camera._capture_pattern(patternFilePath)
    camera._calibrate(patternFilePath, paramFile)
    cv2.imshow('dst', camera.undistort(testFile))
    camera = None
    cv2.waitKey(0)
    camera = Camera('setting.yaml', paramFile)
    camera.undistort(testFile)
    cv2.imshow('dst', camera.undistort(testFile))
    cv2.destroyAllWindows()
    camera = None
