# -*- coding: utf-8 -*-
"""
Created on Thu Jul 11 22:16:38 2019

@author: bobydeng
"""

"""
#----------c++ code-----------

Mat intrinsics, distCoeffs;
rvec.create(1,3,cv::DataType<double>::type);
tvec.create(1,3,cv::DataType<double>::type);
rotationMatrix.create(3,3,cv::DataType<double>::type);

cv::solvePnP(worldPlane, imagePlane, intrinsics, distCoeffs, rvec, tvec);
cv::Rodrigues(rvec,rotationMatrix);

cv::Mat uvPoint = cv::Mat::ones(3,1,cv::DataType<double>::type); //u,v,1

// image point
uvPoint.at<double>(0,0) = 3.; //got this point using mouse callback
uvPoint.at<double>(1,0) = 134.;

cv::Mat tempMat, tempMat2;
double s, zConst = 0;
tempMat = rotationMatrix.inv() * intrinsics.inv() * uvPoint;
tempMat2 = rotationMatrix.inv() * tvec;
s = zConst + tempMat2.at<double>(2,0);
s /= tempMat.at<double>(2,0);
cv::Mat wcPoint = rotationMatrix.inv() * (s * intrinsics.inv() * uvPoint - tvec);

Point3f realPoint(wcPoint.at<double>(0, 0), wcPoint.at<double>(1, 0), wcPoint.at<double>(2, 0)); // point in world coordinates
"""

import numpy as np
from numpy.linalg import inv
import cv2
from camera import Camera

class CameraWorldRel:
    def __init__(self, camera_mtx, pnpParamFile):
        camMat = np.asarray(camera_mtx)
        self.iCam = inv(camMat)

        if pnpParamFile is not None:
            self._loadPnpParam(pnpParamFile)
        
    def _loadPnpParam(self, pnpParamFile):
        with np.load(pnpParamFile) as X:
            rvec, self.tvec = [X[i] for i in ('rvec','tvec')]
        rotMat, _ = cv2.Rodrigues(rvec)
        self.iRot = inv(rotMat)
        
    def img2World(self, image_point, z = 0.0):
        uvPoint = np.ones((3, 1))
    
        # Image point
        uvPoint[0, 0] = image_point[0]
        uvPoint[1, 0] = image_point[1]
    
        tempMat = np.matmul(np.matmul(self.iRot, self.iCam), uvPoint)
        tempMat2 = np.matmul(self.iRot, self.tvec)
    
        s = (z + tempMat2[2, 0]) / tempMat[2, 0]
        wcPoint = np.matmul(self.iRot, (np.matmul(s * self.iCam, uvPoint) - self.tvec))
    
        # wcPoint[2] will not be exactly equal to z, but very close to it
        assert int(abs(wcPoint[2] - z) * (10 ** 8)) == 0
        wcPoint[2] = z

        return wcPoint
    
    def calcPnPParams(self, chessboardimg, camera_matrix, dist_coeffs, pnpParamFile):
        #grid_width = 8.95
        #grid_height = 10.7
        grid_width = 13.2  #118.5/9
        grid_height = 15.8  #110.5/7
        
        objp = np.zeros((6*9,3), np.float32)
        objp[:,:2] = np.mgrid[0:9,0:6].T.reshape(-1,2)
        objp[:,0] *= grid_width
        objp[:,1] *= grid_height
        
        criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
        
        gray = cv2.cvtColor(chessboardimg, cv2.COLOR_BGR2GRAY)
        ret, corners = cv2.findChessboardCorners(gray, (9,6),None)
        
        corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
        
        img = chessboardimg.copy()
        # Draw and display the corners
        img = cv2.drawChessboardCorners(img, (9,6), corners2,ret)
        cv2.imshow('img',img)
        cv2.waitKey(0)
        cv2.destroyAllWindows()
        
        # Find the rotation and translation vectors.
        _, rvec, self.tvec, inliers = cv2.solvePnPRansac(objp, corners2, camera_matrix, dist_coeffs)
        #retval, rvec, self.tvec = cv2.solvePnP(tnsPoints, imPoints, np.asarray(camera_matrix), np.asarray(dist_coeffs))
        
        rotMat, _ = cv2.Rodrigues(rvec)
        self.iRot = inv(rotMat)
        
        print rvec
        print self.tvec
        if pnpParamFile is not None:
            np.savez(pnpParamFile, rvec=rvec, tvec=self.tvec)


if __name__ == '__main__':
    """    
    testFileFolder = "/home/bobydeng/myworks/opencv/camera/pattern_captured_hr/bigboard/"
    fname = testFileFolder + 'board100.jpg'
    img = cv2.imread(fname)
    """
    
    paramFile='cameraParam.npz'
    camera = Camera('setting.yaml', paramFile)
    print 'capture chessboard pattern, enter "r" to accept the frame'
    while True:
        img = camera.capture()
        cv2.imshow('img', img)
        if cv2.waitKey(1) & 0xFF == ord('r'):
            break
    
    camera_matrix, dist_coeffs = camera.getParams()
    
    pnpParamFile = 'pnpParams.npz'
    cwRel = CameraWorldRel(camera_matrix, None)
    cwRel.calcPnPParams(img, camera_matrix, dist_coeffs, pnpParamFile)
    
    """
    criteria = (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
    gray = cv2.cvtColor(chessboardimg, cv2.COLOR_BGR2GRAY)
    ret, corners = cv2.findChessboardCorners(gray, (9,6),None)
        
    corners2 = cv2.cornerSubPix(gray,corners,(11,11),(-1,-1),criteria)
    for i in xrange(len(corners2)):
        pixel= tuple(corners2[i][0])
        print("Pixel: %s" % (pixel, ))
        print(cwRel.img2World(pixel))
        print(objp[i])
    """
    shape_centers = [[754, 532]]
    for i in xrange(len(shape_centers)):
        pixel= tuple(shape_centers[i])
        print("Pixel: %s" % (pixel, ))
        wcPoint= cwRel.img2World(pixel)
        print(wcPoint)
    
    cwRel = CameraWorldRel(camera_matrix, pnpParamFile)
    for i in xrange(len(shape_centers)):
        pixel= tuple(shape_centers[i])
        print("Pixel: %s" % (pixel, ))
        wcPoint= cwRel.img2World(pixel)
        print(wcPoint)
    
    camera = None