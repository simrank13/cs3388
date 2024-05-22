// include headers for glfw, openGL, math, vector, I/O stream, string stream, and standard i/o library
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

// defining math constant pi
const double pi = 3.14159265358979323846;

// using standard namespace instead of using prefix std::
using namespace std;

int main(void) {

    // declare pointer to GLFW window structure
    GLFWwindow* window;

    // initialize the GLFW library and exit with failure if it fails  */
    if (!glfwInit())
        return -1;

    // create a windowed mode window and its OpenGL context with 850 by 850 dimensions and title
    window = glfwCreateWindow(850, 850, "Exercise 1 - Spinning Dogs ", NULL, NULL);

    if (!window){
        // terminate GLFW if window creation fails
        glfwTerminate();
        return -1;
    }

    // make the window's current context to newly created window
    glfwMakeContextCurrent(window);

    // declare vectors to store angle and file date
    vector<int> angleArr;
    vector<float> data;

    // open file named "dog.txt" so we can read the file
    ifstream file("dog.txt");

    // declare variable which will represent the lines in the file
    string line;

    // declare variable which will represent rotation angle
    float rotation;

    // initialize angle at which dog will be positions (8 angles for 8 dogs)
    angleArr = {0, 45, 90, 135, 180, 225, 270, 315};

    // read each line in file
    while(getline(file, line)) {

        // using string stream to read each line
        istringstream string_stream(line);

        // declare variable representing floating point number in file
        float number;

        // get each floating point number from the line and add to the data vector
        while(string_stream >> number) {
            data.push_back(number);
        }

    }

    // main loop that continues until window is closed
    while (!glfwWindowShouldClose(window))
    {
        // poll for and process events
        glfwPollEvents();

        // set background color to white and clear color buffer
        glClearColor(1, 1, 1, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        // set up 2d orthographic projection matrix by defining viewing volume: (0,60,0,60,-1,1)
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, 60, 0, 60, -1, 1);

        // reset modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();

        // set drawing color to black
        glColor3f(0, 0, 0);

        // iterate through each angle
        for (size_t i = 0; i < angleArr.size(); ++i) {

            // calculating position (x and y) based on current angle and storing it in x and y variable
            float x = cos(angleArr[i] * (pi/180));
            float y = sin(angleArr[i] * (pi/180));

            // position object at specific location in window depending on calculated x and y position
            // then keep rotating dog around center ((30,30)) where radius is 25
            glTranslatef(25*x + 30, 25*y + 30, 0);
            glRotatef(rotation, 0, 0, 1);

            // start to draw line strip based on the data points
            glBegin(GL_LINE_STRIP);

            // iterate through each data vector, where every 2 elements in the vector
            //are taken as x and y coordinates for drawing the vertices
            for (size_t j = 0; j < data.size(); j+=2) {
                glVertex2f(data[j], data[j + 1]);
            }

            // end drawing (line strip)
            glEnd();
            // reset transformations for next dog shape
            glLoadIdentity();
        }
        // increment rotation by 1 each time
        rotation += 1;
        // swap front and back buffers to update window's contents
        glfwSwapBuffers(window);
    }
    // terminate GLFW after exiting main loop
    glfwTerminate();
    return 0;
}
