# Take 3D Pic
This is one of my first kinect 360 programs.

The CPP program opens 4 different windows: 1. RGB View, 2. Canny on RGB, 3. Depth, 4. Canny on Depth
With pressing ESC an two images in two datatypes will be taken:
1. final_depth_img.txt 
2. final_depth_img.png
3. final_rgb_img.png

The final_depth_img.txt can be read by the Python3 program Draw_Image.py.

# How to build?

$ cd /path/to/Take_3D_Pic
$ cd build
$ cmake ..
$ make
$ ./main

# How to run Draw_Image.py?

$ cd /path/to/Take_3D_Pic
$ cd build
$ python3 Draw_Image.py

Have fun!

