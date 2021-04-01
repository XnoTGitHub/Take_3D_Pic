#############################################################################
# Name        : main.cpp
# Author      : Florian Neugebauer
# Version     : 1.0
# Copyright   : -
# Description : Draw the Depth Image in 3D
#               
#############################################################################

import numpy as np
import cv2
from matplotlib import pyplot as plt
import csv
from mpl_toolkits.mplot3d import Axes3D
from PIL import Image

#temprory variable for reading the image in
M = []

#Read Depth Image and save it into A
with open('final_depth_img.txt', 'r') as fd:
    i = 0
    for line in fd:
        i=i+1
        line = line[:-2]
        if('[' in line):
        	line = line[2:]

        split_line = line.split(',')
        M.append(list(map(int, split_line)))


#Depth Matrix of the image
Depth_Array = np.array(M)

# To cut all the unnecessairy things out.
# Used to extract the elephant

#Depth_Array[Depth_Array >= 120] = 0
#Y_Cutlevel = 280
#Depth_Array[Y_Cutlevel:480,:] = np.zeros_like(Depth_Array[Y_Cutlevel:480,:])
#Depth_Array[:,:150] = np.zeros_like(Depth_Array[:,:150])
#Depth_Array[Depth_Array >= 120] = 0
#Y_Cutlevel = 280
#Depth_Array[Y_Cutlevel:480,:] = np.zeros_like(Depth_Array[Y_Cutlevel:480,:])
#Depth_Array[:,:150] = np.zeros_like(Depth_Array[:,:150])


indizes_x,indizes_y = Depth_Array.nonzero()
indizes_x= indizes_x[1:-1]
indizes_y= indizes_y[1:-1]
Depth_values = Depth_Array[Depth_Array != 0]
Depth_values = Depth_values[1:-1]
print(Depth_values)
print(indizes_x.shape)
print(Depth_values.shape)

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(indizes_x, indizes_y, Depth_values, zdir='z', marker='.', c= Depth_values)

plt.show()