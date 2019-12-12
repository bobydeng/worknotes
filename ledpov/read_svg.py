#!/usr/bin/python3
# requires svg.path, install it like this: pip3 install svg.path

# converts a list of path elements of a SVG file to simple line drawing commands
from svg.path import parse_path
from xml.dom import minidom

# read the SVG file
doc = minidom.parse('heart1.svg')
svg = doc.getElementsByTagName('svg')[0]
img_height = float(svg.getAttribute('height'))
img_width = float(svg.getAttribute('width'))
left_margin = (img_width - img_height)/2
path_strings = [path.getAttribute('d') for path
                in doc.getElementsByTagName('path')]
doc.unlink()

# print the line draw commands
device_height = 112.5
ID_SCALE = img_height/device_height #scale between image image and device
scale = 1.1#0.9 #

def img2device(a):
	return (a - img_height/2)/ID_SCALE*scale + device_height/2

for path_string in path_strings:
    path = parse_path(path_string)
    for e in path:
        if type(e).__name__ == 'Line':
            x0 = img2device(e.start.real - left_margin)
            y0 = img2device(e.start.imag)
            x1 = img2device(e.end.real - left_margin)
            y1 = img2device(e.end.imag)
            #print("(%.2f, %.2f) - (%.2f, %.2f)" % (x0, y0, x1, y1))
            print("{%d, %d}," % (round(x0), round(y0)))
