// include headers for GLFW, cstdlib for standard library , ctime for time functions, iostream, sstream, and string for I/O
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>

// define frand() function to generate random floating-point number between -1.0 and 1.0.
static inline double frand() {

    // normalize rand() to [0, 1]
    double x = ((double) rand()) / (double) RAND_MAX;

    // randomly negate the value to get range of [-1, 1]
    if (rand() % 2) {
        x *= -1.0;
    }
    // return random floating point number from range [-1,1]
    return x;
}

int main(int argc, char * argv[]) {

    // initialize GLFW library and f it fails, exit the program.
    if (!glfwInit()) {
        return -1;
    }
    // get the command-line arguments for the number of points and screen width and height.

    // initialization of variable storing N points to plot
    int N = atoi(argv[1]);
    // initialization of variable storing width of window
    int width = atoi(argv[2]);
    // initialization of variable storing height of window
    int height = atoi(argv[3]);

    // create a windowed mode window and its OpenGL context with width by height dimensions and title
    GLFWwindow* window = glfwCreateWindow(width, height, "Exercise 2 - Dot Plot", NULL, NULL);

    if (!window) {
        // terminate GLFW if window creation fails
        glfwTerminate();
        return -1;
    }
    // make the window's current context to newly created window
    glfwMakeContextCurrent(window);

    // set background color to white and clear color buffer
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // set up 2d orthographic projection matrix by defining viewing volume: (-1.1,1.1,-1.1,1.1,-1.0,1.0)
    glOrtho(-1.1, 1.1, -1.1, 1.1, -1.0, 1.0);

    // set drawing color to black
    glColor3f(0.0, 0.0, 0.0);

     // poll for and process events
    glfwPollEvents();

    // initialize starting point with random coordinates
    double pointArr[2] = {frand(), frand()};

    // initialize variable storing random starting corner index
    int corner1 = rand() % 4;

    // define coordinates for 4 corners of the sqaure
    double cornerArr[4][2] = {{-1.0, -1.0}, {1.0, -1.0}, {1.0, 1.0}, {-1.0, 1.0}};

    // set size of point that need be drawn
    glPointSize(2.0f);

    // start specifying the points that need to be drawn
    glBegin(GL_POINTS);

    // iterate through N points, generating and plotting them on the window
    for (int i = 1; i <= N; ++i) {

        // declare variable that will store index for next corner
        int corner2;

        // using do while
        do {
            // initialize variable storing random new corner index
            // ensure new corner is not directly opposite of the last corner
            corner2 = rand() % 4;
        } while (corner2 % 4 == (corner1 + 2) % 4);

        // calculates midpoint between current point and the chosen corner
        double x = (pointArr[0] + cornerArr[corner2][0]) / 2.0;
        double y = (pointArr[1] + cornerArr[corner2][1]) / 2.0;

        //plot the vertex
        glVertex2f(x, y);

        // update the current point and corner index for next iteration
        pointArr[0] = x;
        pointArr[1] = y;
        corner1 = corner2;
    }
    // finish specifying points
    glEnd();
    // swap front and back buffers to update window's contents
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
    // terminate GLFW after exiting main loop
    glfwTerminate();
    return 0;
}
