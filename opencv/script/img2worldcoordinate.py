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
import sys


# with open('./calib/calibration.yaml') as f:
#     loadeddict = yaml.load(f)
# camera_matrix = loadeddict.get('camera_matrix')
# dist_coeffs = loadeddict.get('dist_coeff')


def loadCameraParam():
    # Load previously saved data
    with np.load('cameraParam.npz') as X:
        mtx, dist, _, _ = [X[i] for i in ('mtx','dist','rvecs','tvecs')]
    
    return mtx, dist
        
        
camera_matrix, dist_coeffs = loadCameraParam()

tnsPoints = np.zeros((19, 3)) 
tnsPoints[ 0] = (   0.00,     0.00, 0)
tnsPoints[ 1] = (   0.00,   137.16, 0)
tnsPoints[ 2] = (   0.00,   548.64, 0)
tnsPoints[ 3] = (   0.00,   960.12, 0)
tnsPoints[ 4] = (   0.00,  1097.28, 0)
tnsPoints[ 5] = ( 548.64,   137.16, 0)
tnsPoints[ 6] = ( 548.64,   548.64, 0)
tnsPoints[ 7] = ( 548.64,   960.12, 0)
tnsPoints[ 8] = (1188.72 ,    0.00, 0)
tnsPoints[ 9] = (1188.72 ,  137.16, 0)
tnsPoints[10] = (1188.72 ,  548.64, 0)
tnsPoints[11] = (1188.72 ,  960.12, 0)
tnsPoints[12] = (1188.72 , 1097.28, 0)
tnsPoints[13] = (1828.80 ,  137.16, 0)
tnsPoints[14] = (1828.80 ,  548.64, 0)
tnsPoints[15] = (1828.80 ,  960.12, 0)
tnsPoints[16] = (2377.44 ,    0.00, 0)
tnsPoints[17] = (2377.44 ,  137.16, 0)
tnsPoints[18] = (2377.44 ,  548.64, 0)


imPoints = np.zeros((19,2))
imPoints[ 0] = (302, 158)
imPoints[ 1] = (326, 156)
imPoints[ 2] = (398, 154)
imPoints[ 3] = (471, 150)
imPoints[ 4] = (494, 148)
imPoints[ 5] = (319, 172)
imPoints[ 6] = (406, 170)
imPoints[ 7] = (491, 167)
imPoints[ 8] = (270, 206)
imPoints[ 9] = (306, 206)
imPoints[10] = (421, 203)
imPoints[11] = (532, 197)
imPoints[12] = (570, 195)
imPoints[13] = (283, 266)
imPoints[14] = (446, 260)
imPoints[15] = (607, 252)
imPoints[16] = (146, 390)
imPoints[17] = (235, 387)
imPoints[18] = (499, 374)


retval, rvec, tvec = cv2.solvePnP(tnsPoints,
                                  imPoints,
                                  np.asarray(camera_matrix),
                                  np.asarray(dist_coeffs))
rotMat, _ = cv2.Rodrigues(rvec)


def groundProjectPoint(image_point, z = 0.0):
    camMat = np.asarray(camera_matrix)
    iRot = inv(rotMat)
    iCam = inv(camMat)

    uvPoint = np.ones((3, 1))

    # Image point
    uvPoint[0, 0] = image_point[0]
    uvPoint[1, 0] = image_point[1]

    tempMat = np.matmul(np.matmul(iRot, iCam), uvPoint)
    tempMat2 = np.matmul(iRot, tvec)

    s = (z + tempMat2[2, 0]) / tempMat[2, 0]
    wcPoint = np.matmul(iRot, (np.matmul(s * iCam, uvPoint) - tvec))

    # wcPoint[2] will not be exactly equal to z, but very close to it
    assert int(abs(wcPoint[2] - z) * (10 ** 8)) == 0
    wcPoint[2] = z

    return wcPoint

pixel = (400, 400)
print("Pixel: %s" % (pixel, ))
print(groundProjectPoint(pixel))