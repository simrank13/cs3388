// include headers for glew, glfw, math, vector, and standard i/o library
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include <math.h>

using namespace std;

// initialization of screenWidth, screenHeight variable storing the screen width and height
int screenWidth, screenHeight;

// structure representing 2d point with x and y coordinates
struct Point {
    // variables represent x (horizontal positioning) and y coordinate (vertical positioning)
    float x;
    float y;
};

// structure representing node in spline inherited from Point
struct Node : Point {
    // variables representing if node has control handles
    bool hasHandle1, hasHandle2 ;
    // variable representing if node has second control handle
    bool isHandle2;
    // first control point associated with this node
    Point handle1 ;
    // second control point associated with this node
    Point handle2 ;
};

// pointers to record node and control handles clicked by user
vector <Node> nodes;
// points to currently clicked node
Node *clickedNode = NULL;
// points to first handle of clicked node
Node *clickedHandle1 = NULL;
// points to second handle of clicked node
Node *clickedHandle2 = NULL;

// function to calculate distance between two points given their x and y coordinates
float distancePoint(float x1, float y1, float x2, float y2) {
    // using distance formula to determine distance between two points
    return sqrt(((x2 - x1) * (x2 - x1)) + ((y2 - y1)* (y2 - y1)));
}

// function to calculate distance between two nodes
float distance(Node node1, Node node2) {
    // using distance formula to determine distance between two nodes
    return sqrt(((node2.x - node1.x) * (node2.x - node1.x)) + ((node2.y - node1.y) * (node2.y - node1.y)));
}

// function handles mouse button callback events
void mouseCallBack(GLFWwindow* window, int button, int actionTaken, int modifications) {
    // check if left mouse button was pressed
    if (button == GLFW_MOUSE_BUTTON_LEFT && actionTaken == GLFW_PRESS) {
        // variables storing current mouse position
        double mouseX, mouseY;
        // retrieve current position of mouse cursor
        glfwGetCursorPos(window, &mouseX, &mouseY);
        // convert mouseY to coordinate system used by opengl where 90,0) is at bottom left corner, and so invert y coordinate
        mouseY = screenHeight - mouseY;

        // variable representing if node or control point has been clicked by mouse click
        bool nodeClicked = false;

        // iterate through all existing nodes to check if any clicked
        for (size_t i = 0; i < nodes.size(); i++) {
            // check if click was on node itself by comparing click position with nodes position
            if (distancePoint(nodes[i].x, nodes[i].y, mouseX, mouseY) <= 5) {
                // mark node as clicked
                nodeClicked = true;
                // set clicked node for dragging
                clickedNode = &nodes[i];
                // since node was clicked, mark control handles as null to ensure no control handles marked as clicked
                clickedHandle1 = NULL;
                clickedHandle2 = NULL;
                // stop loop since already found clicked item
                break;
            }
            // if not node, then check if first control point (handle1) of node was clicked
            else if (distancePoint(nodes[i].handle1.x, nodes[i].handle1.y, mouseX, mouseY) <= 5) {
                // mark first control point as clicked
                nodeClicked = true;
                // mark this node's first control handle as clicked
                clickedHandle1 = &nodes[i];
                // since first control handle was clicked, mark second control handle and node as null to ensure second control handle and node are not marked as clicked
                clickedNode = NULL;
                clickedHandle2 = NULL;
                // stop loop since already found clicked item
                break;
            }
            // if node has second control point then check if it is clicked
            else if (nodes[i].hasHandle2 && distancePoint(nodes[i].handle2.x, nodes[i].handle2.y, mouseX, mouseY) <= 5) {
                // mark second control point as clicked
                nodeClicked = true;
                // mark this node's second control handle as clicked
                clickedHandle2 = &nodes[i];
                // since second control handle was clicked, mark first control handle and node as null to ensure first control handle and node are not marked as clicked
                clickedNode = NULL;
                clickedHandle1 = NULL;
                // stop loop since already found clicked item
                break;
            }
        }
        // if no node or control point was selected, means click was in empty space so add new node at that position
        if (!nodeClicked) {
            // create new node
            Node newNode;
            // set node's position to click position
            newNode.x = mouseX;
            newNode.y = mouseY;
            // new node start with single control point hence set as true
            newNode.hasHandle1 = true;
            // place first control point directly 50 units above new node
            newNode.handle1.x = mouseX;
            newNode.handle1.y = mouseY + 50;
            // new nodes start without second control point hence set as false
            newNode.hasHandle2 = false;

            // if there are already nodes in screen, update last node to have two control points
            if (!nodes.empty()) {
                // update last node's first handle to be 50 units above it
                nodes.back().handle1.y = nodes.back().y + 50;

                // position second handle for last node
                // mark second handle as active (set true)
                nodes.back().hasHandle2 = true;
                // align second handle horizontally with node
                nodes.back().handle2.x = nodes.back().x;
                // set second handle 50 units below node
                nodes.back().handle2.y = nodes.back().y - 50;
            }
            // add new node that was created to list of nodes
            nodes.push_back(newNode);
        }
    }
    // if left mouse button was released then clear selections
    if (button == GLFW_MOUSE_BUTTON_LEFT && actionTaken == GLFW_RELEASE) {
        // reset clicked node and control points to indicate nothing is currently clicked
        clickedNode = NULL;
        clickedHandle1 = NULL;
        clickedHandle2 = NULL;
    }
}
// callback function to handle keyboard input events
void keyCallBack(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // check if 'E' key is pressed and if so clear all nodes from vector resetting spline by removed all nodes
    if (key == GLFW_KEY_E && action == GLFW_PRESS){
        nodes.clear();
    }
}


// function to drag selected node and its control handles
void dragNode(GLFWwindow* window, int screenHeight) {
    // check if node has been clicked and is clicked for dragging
    if (clickedNode) {
        // variables storing x and y coordinates
        double x, y;
        // get current mouse position
        glfwGetCursorPos(window, &x, &y);
        // adjust mouseY to origin at bottom-left
        double mouseY = screenHeight - y;

        //calcualte horizontal and vertical movement by finding difference between current mouse position and node's current position
        // horizontal movement
        float moveX = x - clickedNode->x;
        // vertical movement
        float moveY = mouseY - clickedNode->y;

        // apply calculated movement to update node's position
        clickedNode->x += moveX;
        clickedNode->y += moveY;

        // apply same movement to node;s first control handle, keeping the control handle relative position with node
        clickedNode->handle1.x += moveX;
        clickedNode->handle1.y += moveY;

        // if node has second control hand then apply same movement to this handle too
        if (clickedNode->isHandle2) {
            clickedNode->handle2.x += moveX;
            clickedNode->handle2.y += moveY;
        }
    }
}

// function to calculate a point on cubic bezier curve at given parameter t
float bezierCurve(float p0, float p1, float p2, float p3, float t) {
    // b is complement of t, representing distance from curve's end
    float b = 1.0f - t;

    // calculate cubic and square powers of b and t
    float b2 = b * b;
    float t2 = t * t;
    float b3 = b * b * b;
    float t3 = t * t * t;

    // calculate bezier curve point using bernstien polynomials for cubic bezier curves
    float p_t = (b3 * p0) + (3 * b2 * t * p1) + (3 * b * t2 * p2) + (t3 * p3);

    // return calculated point on curve
    return p_t;

}

// function to drag first control handle of selected node
void dragHandle1(GLFWwindow* window, int screenHeight){
    // variables storing x and y coordinates
    double x, y;
    // get current mouse position
    glfwGetCursorPos(window, &x, &y);
    // adjust mouseY to origin at bottom-left
    double mouseY = screenHeight - y;

    // set first handle to mouse position
    clickedHandle1->handle1.x = x;
    clickedHandle1->handle1.y = mouseY;

    // adjust second handle so that its across node being in relative position with the first handle
    clickedHandle1->handle2.x = clickedHandle1->x + (clickedHandle1->x - clickedHandle1->handle1.x);
    clickedHandle1->handle2.y = clickedHandle1->y + (clickedHandle1->y - clickedHandle1->handle1.y);
}

// function to drag second control handle of selected node
void dragHandle2(GLFWwindow* window, int screenHeight){
    // variables storing x and y coordinates
    double x, y;
    // get current mouse position
    glfwGetCursorPos(window, &x, &y);
    // adjust mouseY to origin at bottom-left
    double mouseY = screenHeight - y;

    // set second handle to mouse position
    clickedHandle2->handle2.x = x;
    clickedHandle2->handle2.y = mouseY;

    // adjust first handle so that its across node being in relative position with the second handle
    clickedHandle2->handle1.x = clickedHandle2->x + (clickedHandle2->x - clickedHandle2->handle2.x);
    clickedHandle2->handle1.y = clickedHandle2->y + (clickedHandle2->y - clickedHandle2->handle2.y);
}

// function to render Bezier curves connecting the nodes
void renderCurves(){
    // set line width to 2.5f
    glLineWidth(2.5f);
    //begin drawing lines
    glBegin(GL_LINES);

    // loop through each pair of nodes to draw curve between them
    for (size_t i = 0; i < nodes.size() - 1; i++) {
        // draw each segment of curve using 200 subdivisions for smoothness
        for (size_t j = 0; j < 200; ++j) {
            // calculate the parameter t for Bezier curve, ranging from 0 to 1
            float t = (float)j;
            t /= 200;

            // for all points except very first, connect current point to previosu one creatin continous line forming curve
            if (j > 1) {
                glVertex2f(bezierCurve(nodes[i].x, nodes[i].handle1.x, nodes[i + 1].handle2.x, nodes[i + 1].x, t - 1.0f / 200),
                           bezierCurve(nodes[i].y, nodes[i].handle1.y, nodes[i + 1].handle2.y, nodes[i + 1].y, t - 1.0f / 200));
            }
            // calculate position of current point on curve using Bezier curve function, taking positions of current node, its handle, next node's handle and next node
            glVertex2f(bezierCurve(nodes[i].x, nodes[i].handle1.x, nodes[i + 1].handle2.x, nodes[i + 1].x, t),
                       bezierCurve(nodes[i].y, nodes[i].handle1.y, nodes[i + 1].handle2.y, nodes[i + 1].y, t));
        }
    }
    // finish drawing lines
    glEnd();
}

// function to render points of spline
void renderPoints(){
    // set size of points to 13
    glPointSize(13);
    //begin drawing points
    glBegin( GL_POINTS );
    // set drawing color to blue
    glColor3f(0.0f, 0.0f, 1.0f);

    // iterate through all nodes in spline
    for (size_t i = 0; i <nodes.size(); i++) {
        // set position for each node point using its x and y coordinates
        glVertex2f(nodes[i].x,nodes[i].y);
    }
    // finish drawing points
    glEnd ();
}



// function to render control handles for each node
void renderHandles() {
    // begin drawing points on screen
    glBegin(GL_POINTS);

    // iterate through all nodes in spline
    for (size_t i = 0; i < nodes.size(); ++i) {
        // check if current node has first control handle
        if (nodes[i].hasHandle1) {
            // begin drawing a polygon to represent control handle
            glBegin(GL_POLYGON);
            // draw circle for first control handle using 20 segments
            for (int j = 0; j < 20; ++j) {
                // calculate angle for current segment
                float theta = 2.0f * M_PI * j / 20;
                // calculate and set vertex position for this segment of the circle
                glVertex2f(nodes[i].handle1.x + 5.0f * cosf(theta),
                           nodes[i].handle1.y + 5.0f * sinf(theta));
            }
            // finish drawing polygon
            glEnd();
        }
        // check if current node (exclusding first and last) has second control handle
        if (i > 0 && i < nodes.size() - 1 && nodes[i].hasHandle2) {
            // begin drawing polygon for second control handle
            glBegin(GL_POLYGON);
            // draw circle for second control handle using 20 segments, similar to first
            for (int j = 0; j < 20; ++j) {
                // calculate angle for current segment
                float theta = 2.0f * M_PI * j / 20;
                // calculate and set vertex position for this segment of circle
                glVertex2f(nodes[i].handle2.x + 5.0f * cosf(theta),
                           nodes[i].handle2.y + 5.0f * sinf(theta));
            }
            // finish drawing polygon
            glEnd();
        }
    }
    // finish drawing points on screen
    glEnd();
}

// function to render stippled lines connecting nodes to their control handles
void renderStipple() {
    // enabling line stippling and render stippled lines connecting nodes to control points
    glEnable (GL_LINE_STIPPLE);
    // display line stipple pattern with factor of 1
    glLineStipple (1, 0x00FF);
    // begin drawing lines
    glBegin ( GL_LINES );
    // set color to teal
    glColor3f(0.0f,0.8f,1.0f);

    // iterate through all nodes to draw lines from each node to its control points
    for (size_t i = 0; i < nodes.size(); ++i) {
        // draw a line from node to its first control point where starting point is at node and ending point is at first control handle
        glVertex2f(nodes[i].x, nodes[i].y);
        glVertex2f(nodes[i].handle1.x, nodes[i].handle1.y);

        // check if current node is not end node and has second control handle
        if (i > 0 && i < nodes.size() - 1 && nodes[i].hasHandle2) {
            // draw line from node to its second control point for intermediate nodes where starting point is at node and ending point is at second control handle
            glVertex2f(nodes[i].x, nodes[i].y);
            glVertex2f(nodes[i].handle2.x, nodes[i].handle2.y);
        }
    }
    // finish drawing lines
    glEnd ();
    // disable stipple pattern
    glDisable( GL_LINE_STIPPLE );
}


// main function
int main(int argc, char** argv) {
    // initialize GLFW library, if it fails, exit program
    if (!glfwInit()) {
        return -1;
    }
    // request to enable 4 times anti-aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);

    // check if program recieved enough command-line arguments for screen dimensions and exit if not enough arguments
    if(argc < 3) {
        return -1;
    }

    // convert command-line arguments to integers and set as screen width and height
    screenWidth = atoi(argv[1]);
    screenHeight = atoi(argv[2]);

    // create GLFW window with specified dimwnsions and title, A Spline Tool
    GLFWwindow *window = glfwCreateWindow(screenWidth, screenHeight, "A Spline Tool", NULL, NULL);

    // if window create failed, terminate GLFW and exit
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // make created window's OpenGL context current
    glfwMakeContextCurrent(window);
    // set current matrix mode to projection to set up the camera projection
    glMatrixMode(GL_PROJECTION);
    // reset projection amtrix
    glLoadIdentity();
    // enable multisampling for smoother lines and edges
    glEnable(GL_MULTISAMPLE);

    // define orthographic projection matrix mapping OpenGL coordinates directly to window pizels
    glOrtho(0, screenWidth, 0, screenHeight, -1, 1);
    // set viewport to cover entire window
    glViewport(0, 0, screenWidth, screenHeight);

    // set mouse button and key callbacks for GLFW window interaction
    glfwSetMouseButtonCallback(window, mouseCallBack);
    glfwSetKeyCallback(window, keyCallBack);

    // main loop that runs until window is closed
    while (!glfwWindowShouldClose(window)) {

        // check for user interaction with node and drag them if clicked
        if (clickedNode != NULL) {
            dragNode(window, screenHeight);
        }
        // check for user interaction with first control handle nd drag them if clicked
        if (clickedHandle1 != NULL) {
            dragHandle1(window, screenHeight);
        }
        // check for user interaction with second control handle and drag them if clicked
        if (clickedHandle2 != NULL) {
            dragHandle2(window, screenHeight);
        }

        // clear color buffer with white color
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        // set drawing color to black
        glColor3f(0.0f,0.0f,0.0f);

        //enable line and point smoothing for better visibility
        glEnable (GL_LINE_SMOOTH );
        glEnable (GL_BLEND );
        glBlendFunc (GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

        // if there are enough nodes, render curves connecting them
        if (nodes.size() > 1) {
            // render curves
            renderCurves();
        }
        // disable line smoothing after rendering curves
        glDisable(GL_LINE_SMOOTH);
        glDisable(GL_BLEND);

        // render stipple lines
        renderStipple();

        // disable multisampling
        glDisable(GL_MULTISAMPLE);

        // render nodes and control handles as points on screen
        renderPoints();

        // enable multisampling for smoother lines and edges
        glEnable(GL_MULTISAMPLE);

        // set drawing color to black
        glColor3f(0.0f, 0.0f, 0.0f);

        // enable line and point smoothing for better visibility
        glEnable(GL_POINT_SMOOTH );
        glEnable(GL_BLEND );
        glBlendFunc(GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

        // render control handles
        renderHandles();

        // disable line smoothing after rendering curves
        glDisable(GL_POINT_SMOOTH);
        glDisable(GL_BLEND);

        // swap front and back buffers to update window's content and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // terminate GLFW before exiting program
    glfwTerminate();
    return 0;
}
