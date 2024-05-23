Skullar5 - 251292956

---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
I have submitted a zipped folder which contains:                                                                                                                                 |
1. files folder - folder containing the mesh (PLY) and texture (BMP) files                                                                                                       |
2. a4.cpp - main program                                                                                                                                                         |
3. README.txt                                                                                                                                                                    |
                                                                                                                                                                                 |
Screenshot 1, 2, and 3 are submitted independently                                                                                                                               |
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

***********************************************************************************************************************************************************************************
How to compile and run the program                                                                                                                                                *
----------------------------------                                                                                                                                                *
1. First download the zipped folder and extract it (without changing folder's structure)                                                                                          *
2. I used codeblocks which required me to just build and run, however, if using a different c++ compiler  then run this command (make sure you are in the same folder as a4.cpp): *
   g++ -g a4.cpp -o a4 -lGL -lglfw -lGLEW (or g++ a4.cpp -lGLEW -lglfw -lGL)                                                                                                      *
3. then run                                                                                                                                                                       *
***********************************************************************************************************************************************************************************


Screenshot Steps
----------------

Screenshot 1
- - - - -  -
1. Go back 8 steps OR 1 step before crossing the wall
2. Go left 5 steps OR number of steps leading to the window above bed being nearly a bit more to the right of the middle in the window

Screenshot 2
- - - - - - -
1. Go left 9 steps OR number of steps leading to camera facing towards bed and window
2. Go forward 10 steps OR number of steps leading to camera looking outside window
3. Go right 14 steps OR number of steps leading to the door being in middle of screen

Screenshot 3
- - - - - - -
1. Go left 6 presses OR number of steps leading to camera facing area between the bed and sink
2. Go forward 7 presses OR number of steps leading to camera facing the wall - 1 step before crossing the wall
3. Go right 14 presses OR number of steps leading to tree trunk in middle of screen


Known bugs
----------
In readPLYfile, doesnt check if it is a valid ply file. TexturedMesh constructor in TexturedMesh class doesnt check if successfully read the texture and mesh files or if the files
are in wrong path or do not exist.

Description of Program
----------------------
This program uses OpenGL, GLFW, GLEW, (and GLM) and renders textured meshes loaded from PLY files with tectures applied from BMP images, it creates a window showing these
different textures objects with forward, backward, right rotation, left rotation camera control.


Explanation of my code/ program
-------------------------------

Structures Used
- - - - - - - -
1) VertexData
   This structure consists of the vertex's positions, normal vectors, colour, and texture coordinates.

2) TriData
   This structure stores the vertices' indices


Class Used
- - - - - -
1) TexturedMesh
   This class represents a textured mesh which consists of a list of VertexData and TriData objects, read from the mesh files (PLY). In this class, consists of
   a pointer pointing to the texture mesh data, read from the texture files (BMP). This class consists of Vertex Buffer Objects (VBOs) IDs, Vertex Array Object (VAO) ID,
   Element Buffer Object (EBO) ID/ Index Buffer ID, texture object ID, and a shader program ID (which will also be used in drw function)


Functions Used
--------------

1) loadARGB_BMP Function
   This function reads data from BMP file at given path into data pointer

2) readPLYfile Function
   This function reads mesh data from PLY files. It starts by opening the file given by the user. If it finds a line starting with "element", it keeps track of how many elements are
   mentioned because it'll need this for reading vertex and face data later. It looks specifically for the words "vertex" or "face" after "element". When it finds "end_header", it
   stops looking for headers. Then, it reads lines equal to the number of vertices mentioned earlier. Each line should have 8 numbers (these are floating-point numbers), which are
   packed into a new VertexData object and added to a list. After that, it does the same for faces, where each line should describe a face with 3 points (indices). It takes the first
   3 points from each line and makes a new TriData object, adding it to another list.

3) TexturedMesh Constructor
   This part sets up the mesh and texture files. It makes a Vertex Array Object (VAO) and binds it. Then, it creates Vertex Buffer Objects (VBOs) for vertex positions using the list
   of vertices it got before. It sets the space between each piece of data (stride) based on the size of a VertexData object. It does the same for texture data, using the same list but
   focusing on the texture part. Then it makes a VBO for the points that make up faces and unbinds the VAO. After that, it sets up the shader program, attaching and then removing the
   shaders once everything is connected. Finally, it makes a texture object with the BMP file data and then unbinds this texture object.

4) draw Function
   This function renders the TexturedMesh object by enable the blending. Then, get shader program id's uniform matrix location and set the matrix as MVP in its parameters.
   Then enable 2D texturing and bind the texture. Next, bind the VAO and draw the object using triangles. Then unbind the VAO and clear out attributes

5) main Function
   The main function initialized window and GLEW library, and creates all the TexturedMesh objects using the BMP and PLY files stored in the files folder. This function
   initialized depth testing, depth comparision, clear color for color buffer, camera position, camera direction, and up vector. When entering the rendering loop,
   the camera will move based on the key (forward, backward, left rotation, right rotation) pressed and will draw all the TexturedMesh objects until window is closed.











