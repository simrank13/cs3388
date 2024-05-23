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

## Linked House Navigator
This project is a C++ and OpenGL implementation of a textured triangle mesh renderer with camera control, aiming to familiarize users with working with triangular meshes and manipulating the camera in world space. The program allows users to load triangle mesh data and textures from files, render them with VBOs and VAOs, and explore the rendered world by moving the camera using arrow keys. The key features include manipulation of the view matrix for camera movement, loading triangle mesh data and textures, and rendering textured triangle meshes. Users can navigate the rendered world by moving the camera around in the environment

Download the zipped folder and extract it (without changing folder's structure). If useing codeblocks, build and run to compile. However, if using a different c++ compiler then run this command (make sure you are in the same folder as a4.cpp): g++ -g a4.cpp -o a4 -lGL -lglfw -lGLEW (or g++ a4.cpp -lGLEW -lglfw -lGL) 

## OpenGL Marching Cubes and Phong Shading/ Mesh Generation
This is a C++ project that uses OpenGL, GLFW, GLEW, & GLM and the marching cubes algorithm to create a 3D representation of a mathematical function, and applies Phong shading to render the object with realistic lighting.


### Build Instructions
To build this project, you will need a C++ compiler. Follow these steps:

Clone the repository. Navigate to the project directory and run `make` and run `./a.out [screen_width] [screen_height] [stepsize] [min] [max]`

screen_width: Width of the window, default is 1400.
screen_height: Height of the window, default is 900.
stepsize: The step size for the marching cubes algorithm, default is 0.05.
min: The minimum value for the coordinate axes, default is -5.
max: The maximum value for the coordinate axes, default is 5.
For example, run the program with default parameters:

`./a.out`

Or with custom parameters:

`./a.out 800 600 0.1 -3 3`

### Camera Controls
- Up Arrow: Zoom the camera closer to the origin.
- Down Arrow: Zoom the camera away from the origin.
- Mouse movement: Rotate the camera.

### Project Structure
- meshgen.cpp: The main file of the project, which initializes GLFW and GLEW, sets up the window and OpenGL context, and runs the main loop.
- Camera: Class that controls the users mouse movement to rotate the scene.
- shaders.hpp: Header file containing utility functions for loading and compiling shaders.
- TriTable.hpp: Header file containing the triangle lookup table for the marching cubes algorithm.
- verticeshader.vert: Vertex shader file for Phong shading.
- fragmentshader.frag: Fragment shader file for Phong shading.
  
### Features
Implements the marching cubes algorithm to generate 3D geometry from a mathematical function. Applies Phong shading to render the object with realistic lighting. Exports the generated geometry as a PLY file for further use. Provides a customizable camera for viewing the scene. Supports user-defined window size, step size, and coordinate range.


## OpenGL Water Simulation
This project is a 3D water simulation using OpenGL. It features a plane mesh that simulates water and various 3D objects such as a boat, head, and eyes that interact with the water surface.

### Prerequisites
To build and run the project, you will need:

- C++ compiler with support for C++11
- OpenGL 3.3 or higher
- GLFW
- GLEW
- GLM
- BMapLoader.hpp
- PlaneMesh.hpp
- LoadBitmap.cpp

A set of sample assets (boat.ply, boat.bmp, head.ply, head.bmp, eyes.ply, and eyes.bmp)
### Build Instructions
- Install the necessary libraries and tools for your platform.
- Clone the repository and navigate to the project directory.
- Compile the project using your preferred C++ compiler and linker settings, ensuring that all required libraries are included.
- Recommended approach: Navigate to the project directory, type `make` in the terminal, then `./a.out`
### Running the Simulation
- Run the compiled executable.
- The simulation window will open, displaying the water surface and the 3D objects.
- Use the camera controls to navigate around the scene.
- Press the ESC key to close the window and exit the simulation.
### Camera Controls
- Up Arrow: Zoom the camera closer to the origin.
- Down Arrow: Zoom the camera away from the origin.
- Mouse movement: Rotate the camera.
### 3D Object Controls
- W, A, S, D: Move the object forward, left, backward, and right corresponding to which direction it is facing
- Q, E: Rotate the object counter-clockwise and clockwise
- Z, X: Move the object up and down
### Features
- Plane mesh water surface that simulates water movement and reflections
- 3D objects (boat, head, and eyes) that interact with the water surface (WIP - 3D objects are able to move around the scene, but currently cannot interact with the PlaneMesh water)
- Dynamic camera control for navigating the scene
- Realistic lighting and shading
### Code Overview
- The main components of the project are:

- main(): Initializes OpenGL, GLFW, and GLEW, creates the window, sets up shaders, loads assets, and enters the main loop.
- Camera: A class that handles the different viewing angles for the user, updating based on user input.
- PlaneMesh: A class that handles the creation, rendering, and updating of the plane mesh water surface.
- TexturedMesh: A class that handles the creation, rendering, and updating of 3D objects (boat, head, and eyes).
- Shaders: Custom vertex, tessellation, geometry, and fragment shaders for rendering the water surface and 3D objects with realistic lighting and shading.
