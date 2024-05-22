# Collection of assignments using C++

## Spinning Dogs
dog.txt is a space separated file containing float information for the "dogs" that exercise1.cpp will be drawing.

exercise1.cpp aims to read the space-separated dog.txt in order to draw "spinning" dogs around a circle centered at (30, 30) on the viewport which has a radius of 25. For each frame drawn (i.e. each iteration of the while loop) the dogs will be rotated one degree counterclockwise.

## Random Generated Dot Plot
exercise2.cpp aims to implement an algorithm provided by the professor in order to draw a dot plot on the screen. The file takes 3 command line inputs in order to help implement said algorithm. The file can be run by entering the arguments arg1 arg2 arg3 in sets program arguments in projects using codeblocks, is the integer for N in the algorithm (number of dots to draw), arg2 is the integer for the screen width, and arg3 is the integer for the height of the screen. If not using codeblocks, enter this command to run:  g++ exercise2.cpp arg1 arg2 arg3 -g -lglfw -lGLEW -lOpenGL

For very large integers for N (such as 5000000), the program can take a while to load. Give it a few seconds, it should still pop up however.

## Spline Tool

This program is a graphical spline editing tool designed for creating splines which uses GLFW, GLEW, vector, cmath, and standard io library in which nodes can be added and dragged and control points can also be dragged. The end nodes have 1 control point and the intermediate nodes have 2 control points in which these control points are horizontally positioned where the top control point is 50 units above end/intermediate node and bottom control point is 50 unit below. In this program, new nodes can be used by clicky on any empty space and move existing nodes by dragging them. Control points can be adjusted to modify the curvature of spline. Spline curves, points, handles and nodes are rendered and can clear the screen by clicking the 'E' keyboard. In this program, to add the first node of spline, in the window, it can be clicked anywhere then continue adding nodes by clicking in which each node has control points for adjusting curve. Nodes can be dragged as well as control points to modify spline. In the window, must left-click area and if click on white area new node added if cliick close to a node then closer end point now become intermediate node and new node be new end point. 

If using codeblocks, build and run to compile or enter this command (while in the spline-tool folder): g++ a3.cpp -g -lglfw -lGLEW -lOpenGL
