# BinocularStereoVison_SAD_DP
Implemented block matching with SAD algorithm and dynamic programming

Binocular Stereo Vision is the fundamental part of machine vision. A Block Matching Algorithm is a way of locating matching macroblocks in a sequence of digital video frames for the purposes of motion estimation. 

Block matching can be achieved in different algorithms such as SSD, SAD, SSD, NCC, and NSSD. In this project, I implemented block matching with SAD algorithm.

SAD(u,v) = Sum{|Left(u,v) - Right(u,v)|}  is to find the minimum difference of the sum of block values. It is simple but computational expensive. We can use dynamic programming to optimize the time complexity and make it faster.


The input images include a pair of rectified images captured by both left and right camera:
Left:

![image](http://github.com/delphine777/BinocularStereoVison_SAD_DP/raw/master/left.jpg)

Right:

![image](http://github.com/delphine777/BinocularStereoVison_SAD_DP/raw/master/right.jpg)

The disparity Map calculated from left to right is:
![image](http://github.com/delphine777/BinocularStereoVison_SAD_DP/raw/master/disparity1.jpg)

From right to left is:
![image](http://github.com/delphine777/BinocularStereoVison_SAD_DP/raw/master/disparity2.jpg)
