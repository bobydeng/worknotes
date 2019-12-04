# -*- coding: utf-8 -*-
"""
Created on Sat Aug 10 21:35:26 2019

@author: bobydeng
"""

import cv2
import math
import numpy as np

name = 'taya'
name = 'hacked'
name = 'circles'
fname = '/home/bobydeng/myworks/worknotes/worknotes/Arduino/led_pov/led_pov_' + name + '.png'
img = cv2.imread(fname)
#cv2.imshow('img',img)

gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
cv2.imshow('gray',gray)

ret, thresh = cv2.threshold(gray, 50, 1, cv2.THRESH_BINARY)
#cv2.imshow('thresh',thresh)

width = img.shape[0]
offset = width/2
print img.shape

'''
len_total = 57.0
len0 = 19
stepLen = 4
led_cnt = 10

'''
len_total = 56.0 #55.0
len0 = 16.5
stepLen = 2.567
led_cnt = 16


'''
len_total = 58.0
len0 = 19
stepLen = 2
led_cnt = 20
'''

'''
len_total = 58.5
len0 = 19
stepLen = 1
led_cnt = 40
'''

rate = width/len_total/2.0

div = 360#*8
RAD_PER_DIV = math.pi*2/div 
#DEG_PER_RAD = 180.0/math.pi

pov_img = []
for i in range(div):
    theta = i*RAD_PER_DIV
    leds = 0
    for j in range(led_cnt):
        r = (len0 + j * stepLen) * rate
        x = r * math.cos(theta)
        y = r * math.sin(theta)
        #print '(x y) = (' ,x ,y , ')'
        col = int(round(offset + x))
        row = int(round(offset - y))
        #print '(row col) = (' ,row,col , ')'        
        #print '(i j) = (' ,i,j , ')'       
        
        leds |= thresh[row][col] << (led_cnt -1 - j)
    pov_img.append(leds)    
        
data = '{'
for i in range(div/10):
    data += '%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n'%tuple(pov_img[10*i:10*i+10])

data = data[:-2] + '}'
print data

'''
img_rcv = np.zeros((width,width,1), np.uint8)

for row in range(width):
    for col in range(width):
       x = (col - offset)/rate
       y = (offset - row)/rate
       theta = math.atan2(y,x)
       i = int(theta/RAD_PER_DIV)
       r = math.sqrt(x*x + y*y)
       j = int(round((r - len0)/stepLen))
       if (j >=0) and (j < led_cnt):
           if pov_img[i] & (1 << (led_cnt -1 - j)):
               img_rcv[row,col] = 255

cv2.imshow('img_rcv',img_rcv)
'''

div_rcv = 16
img_rcv2 = np.zeros((width,width,1), np.uint8)
for i in range(div*div_rcv):
    theta = i*RAD_PER_DIV/div_rcv
    for j in range(led_cnt):
        if pov_img[i/div_rcv] & (1 << (led_cnt -1 - j)):
            r = (len0 + j * stepLen) * rate
            x = r * math.cos(theta)
            y = r * math.sin(theta)
            col = int(round(offset + x))
            row = int(round(offset - y))
            cv2.circle(img_rcv2, (col,row), int(round(stepLen/2)), (255))

cv2.imshow('img_rcv2',img_rcv2)

cv2.waitKey(0)

cv2.destroyAllWindows()