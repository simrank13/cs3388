// Including standard and C++ libraries for input/output, file operations, and data structures.
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <array>
#include <string.h> // For legacy C string functions

// Including a custom header, presumably for lookup tables used in the Marching Cubes algorithm.
#include "TriTable.hpp"

// Including GLEW to manage OpenGL extensions, and GLFW for window and input handling.
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Including GLM for OpenGL mathematics, like vectors and matrices operations.
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Defining constants for identifying vertices in a cube, used in the Marching Cubes algorithm.
#define BOTTOM_BACK_LEFT	1
#define BOTTOM_BACK_RIGHT	2
#define BOTTOM_FRONT_RIGHT	4
#define BOTTOM_FRONT_LEFT	8
#define TOP_BACK_LEFT		16
#define TOP_BACK_RIGHT		32
#define TOP_FRONT_RIGHT		64
#define TOP_FRONT_LEFT		128

// Using namespaces to simplify syntax (avoiding std:: and glm:: prefixes).
using namespace glm;
using namespace std;

// Variables for tracking mouse input state and position.
double dragging;
double lastXPos, lastYPos;

// Functions to define scalar fields, used as input for the Marching Cubes algorithm.
float f1(float x, float y, float z) {
    return pow(x,2) + pow(y,2) + pow(z,2);
}

float f2(float x, float y, float z) {
	return y - sin(x)*cos(z);
}

float f3(float x, float y, float z) {
	return pow(x,2) - pow(y,2) - pow(z,2) - z;
}

// Camera class definition for managing 3D camera position and orientation.
class Camera {
    private:
        vec3 pos; // Camera position
        vec3 lookAtVector; // Point the camera is looking at
        float theta; // Azimuthal angle in spherical coordinates
        float phi; // Polar angle in spherical coordinates
        float r; // Radius in spherical coordinates

    public:
        // Constructor sets initial camera position and computes orientation based on the 'lookAtVector' and 'pos'.
        Camera(vec3 lookAtVector, vec3 pos) {
            this->lookAtVector = lookAtVector; // Fixed: Should use 'this->' to differentiate between the parameter and the member variable.
            this->pos = pos;
            // Compute spherical coordinates from Cartesian coordinates.
            this->r = length(this->pos - lookAtVector);
            this->theta = acos(pos.z / this->r);

            // Determine the sign of the 'y' coordinate for correct angle calculation.
            float ycoord = 0.0f;
            if (pos.y == 0) {
                ycoord = 0.0f;
            } else if (pos.y > 0) {
                ycoord = 1.0f;
            } else if (pos.y < 0) {
                ycoord = -1.0f;
            }

            // Calculate 'phi', taking care of division by zero if 'pos' is directly above or below 'lookAtVector'.
            float d = pow(pos.x, 2) + pow(pos.y, 2);
            float distance = sqrt(d);
            float x = pos.x;
            float angle = acos(x / distance);
            this->phi = ycoord * acos(angle);
        }
        // Updates camera's spherical coordinates and recalculates position.
        void updateOrientation(float theta, float phi) {
            // Increment the camera's azimuthal and polar angles by the provided values
            this->theta += theta;
            this->phi += phi;

            // Define minimum and maximum limits for the polar angle to prevent the camera from flipping over
            float minPhi = 0.1f; // Slightly above 0 to avoid looking directly up, which can cause disorientation
            float maxPhi = glm::pi<float>() - 0.1f; // Slightly less than Pi to avoid looking directly down

            // Clamp the polar angle within the defined limits to maintain a comfortable viewing range
            if (this->phi < minPhi) {
                this->phi = minPhi;
            } else if (this->phi > maxPhi) {
                this->phi = maxPhi;
            }

            // Calculate the camera's new position relative to the lookAt point using spherical to Cartesian coordinates conversion
            // The new position is determined based on the updated angles and the current radius (distance from the lookAt point)
            float relativePosX = this->r * sin(this->phi) * cos(this->theta);
            float relativePosY = this->r * cos(this->phi);
            float relativePosZ = this->r * sin(this->phi) * sin(this->theta);

            // Combine the calculated relative position components into a 3D vector
            vec3 relativePos(relativePosX, relativePosY, relativePosZ);

            // Update the camera's position by adding the relative position to the lookAt point's coordinates
            // This effectively moves the camera while keeping it pointed towards the lookAt point
            this->pos = lookAtVector + relativePos;
        }

        // This function generates the view matrix for the camera.
        // The view matrix is used in rendering to transform world coordinates to the camera's view space.
        mat4 getViewMatrix() {
            vec3 eyeVector = this->pos; // The camera's current position.
            vec3 centerVector = lookAtVector; // The point the camera is looking at.
            vec3 upVector = vec3(0,1,0); // The up direction in world space, usually set to the world's 'up'.

            // The glm::lookAt function creates a view matrix that simulates a camera looking from 'eye' towards 'center' with 'up' as the up direction.
            return lookAt(eyeVector, centerVector, upVector);
        }

        // This function updates the camera's distance from its lookAt point, effectively zooming in or out.
        void updateRadius(float deltaRadius) {
            this->r += deltaRadius; // Increment the camera's radius by the given delta.
            if (this->r < 0) {
                this->r = 0.1f; // Prevent the radius from becoming negative, which would place the camera behind the lookAt point, flipping the view.
            }

            // Recalculate the camera's position using spherical coordinates, based on the updated radius and current angles.
            float relativePosX = this->r * sin(this->phi) * cos(this->theta);
            float relativePosY = this->r * cos(this->phi);
            float relativePosZ = this->r * sin(this->phi) * sin(this->theta);

            // Combine the new position components into a 3D vector.
            vec3 relativePos(relativePosX, relativePosY, relativePosZ);

            // Update the camera's position to be the new relative position added to the lookAt point.
            // This maintains the camera's orientation while changing its distance from the lookAt point.
        this->pos = lookAtVector + relativePos;
    }

};
// The camera is initialized to look at the origin (0, 0, 0) from the position (5, 5, 5).
Camera camera(vec3(0, 0, 0), vec3(5, 5, 5));

// Declaration of the 'marching_cubes' function, which applies the Marching Cubes algorithm to generate a mesh.
vector<float> marching_cubes(
function<float(float, float, float)> f,
float isovalue,
float min,
float max,
float stepsize);

// Defines the marching_cubes function that takes a scalar field function 'f', an isovalue for the isosurface,
// the minimum and maximum bounds of the volume to be sampled, and the step size for sampling.
vector<float> marching_cubes(function<float(float, float, float)> f, float isovalue, float min, float max, float stepsize) {

    // Initializes a vector to store the vertices of the resulting mesh.
    vector<float> vertices;

    // Calculates the number of samples along one axis based on the volume bounds and step size.
    int num = static_cast<int>((max - min) / stepsize);

    // Iterates through each point in the 3D volume based on the calculated number of samples.
    for (int i = 0; i < num; i++) {
        for (int j = 0; j < num; j++) {
            for (int k = 0; k < num; k++) {

                // An array to hold the scalar values at the vertices of the current cube being evaluated.
                array<float, 8> vertArr;

                // Evaluates the scalar field function 'f' at each vertex of the cube.
                // The indices 0 to 7 correspond to the eight vertices of a cube in 3D space.
                vertArr[0] = f((min + i * stepsize), (min + j * stepsize), (min + k * stepsize));
                vertArr[1] = f((min + i * stepsize) + stepsize, (min + j * stepsize), (min + k * stepsize));
                vertArr[2] = f((min + i * stepsize) + stepsize, (min + j * stepsize), (min + k * stepsize) + stepsize);
                vertArr[3] = f((min + i * stepsize), (min + j * stepsize), (min + k * stepsize) + stepsize);
                vertArr[4] = f((min + i * stepsize), (min + j * stepsize) + stepsize, (min + k * stepsize));
                vertArr[5] = f((min + i * stepsize) + stepsize, (min + j * stepsize) + stepsize, (min + k * stepsize));
                vertArr[6] = f((min + i * stepsize) + stepsize, (min + j * stepsize) + stepsize, (min + k * stepsize) + stepsize);
                vertArr[7] = f((min + i * stepsize), (min + j * stepsize) + stepsize, (min + k * stepsize) + stepsize);

                // An integer used as a bitmask to represent the cube configuration based on the isovalue.
                int vertIndices = 0;

                // Determines the configuration of the cube by comparing each vertex value to the isovalue
                // and setting the corresponding bit in 'cubeindex'.
                if (vertArr[0] < isovalue) vertIndices |= BOTTOM_BACK_LEFT;
                if (vertArr[1] < isovalue) vertIndices |= BOTTOM_BACK_RIGHT;
                if (vertArr[2] < isovalue) vertIndices |= BOTTOM_FRONT_RIGHT;
                if (vertArr[3] < isovalue) vertIndices |= BOTTOM_FRONT_LEFT;
                if (vertArr[4] < isovalue) vertIndices |= TOP_BACK_LEFT;
                if (vertArr[5] < isovalue) vertIndices |= TOP_BACK_RIGHT;
                if (vertArr[6] < isovalue) vertIndices |= TOP_FRONT_RIGHT;
                if (vertArr[7] < isovalue) vertIndices |= TOP_FRONT_LEFT;

                // An array to temporarily hold the vertices generated from the current cube configuration.
                array<array<float, 3>, 8> verts;

                // Iterates over the edges of the cube that intersect the isosurface, based on the lookup table.
                // 'marching_cubes_lut' is a predefined table that maps cube configurations to intersecting edges.
                for (int v = 0; marching_cubes_lut[vertIndices][v] != -1; v += 3) {
                    // Retrieves the indices of the vertices that form each intersecting edge.
                    int edge0 = marching_cubes_lut[vertIndices][v];
                    int edge1 = marching_cubes_lut[vertIndices][v + 1];
                    int edge2 = marching_cubes_lut[vertIndices][v + 2];

                    // Calculates the positions of the vertices on the intersecting edges and adds them to the 'vertices' vector.
                    // The positions are interpolated based on the scalar values at the ends of each edge.
                    vertices.push_back((min + i * stepsize) + vertTable[edge0][0] * stepsize);
                    vertices.push_back((min + j * stepsize) + vertTable[edge0][1] * stepsize);
                    vertices.push_back((min + k * stepsize) + vertTable[edge0][2] * stepsize);

                    vertices.push_back((min + i * stepsize) + vertTable[edge1][0] * stepsize);
                    vertices.push_back((min + j * stepsize) + vertTable[edge1][1] * stepsize);
                    vertices.push_back((min + k * stepsize) + vertTable[edge1][2] * stepsize);

                    vertices.push_back((min + i * stepsize) + vertTable[edge2][0] * stepsize);
                    vertices.push_back((min + j * stepsize) + vertTable[edge2][1] * stepsize);
                    vertices.push_back((min + k * stepsize) + vertTable[edge2][2] * stepsize);
                }
            }
        }
    }
    // Returns the vector containing all the vertices that form the mesh of the isosurface.
    return vertices;
}

// Function to compute normals for a set of vertices, where each group of 9 floats (3 vertices) represents a triangle.
vector<float> compute_normals(const vector<float>& vertices) {
    // Create a vector to store the normals.
    std::vector<float> normals;

    // Reserve space in the normals vector to improve memory allocation efficiency.
    // The size is the same as the input vertices because each vertex will have a corresponding normal.
    normals.reserve(vertices.size());

    // Loop through the vertices vector in steps of 9 floats (3 vertices per triangle).
    for (size_t i = 0; i < vertices.size(); i += 9) {
        // Construct vec3 objects for each vertex of the triangle.
        vec3 vertex1(vertices[i], vertices[i + 1], vertices[i + 2]);
        vec3 vertex2(vertices[i + 3], vertices[i + 4], vertices[i + 5]);
        vec3 vertex3(vertices[i + 6], vertices[i + 7], vertices[i + 8]);

        // Calculate the vectors representing two edges of the triangle.
        vec3 edge1 = vertex2 - vertex1; // Vector from vertex1 to vertex2
        vec3 edge2 = vertex3 - vertex1; // Vector from vertex1 to vertex3

        // Compute the normal of the triangle using the cross product of the two edge vectors.
        // The cross product yields a vector that is perpendicular to the plane of the triangle.
        // The normalize function ensures that the resulting normal vector has a unit length.
        vec3 normal = normalize(cross(edge1, edge2));

        // Assign the computed normal vector to each of the three vertices of the triangle.
        // This is done because in a smooth shaded mesh, vertices are shared by adjacent triangles,
        // and the vertex normal is typically an average of the normals of the faces adjacent to that vertex.
        // For simplicity, this code assigns the face normal to all three vertices.
        for (int j = 0; j < 3; ++j) {
            normals.insert(normals.end(), {normal.x, normal.y, normal.z});
        }
    }

    // Return the vector containing all the computed normals.
    return normals;
}

// Function to write the vertices and normals of a 3D mesh into a PLY file.
void writePLY(const vector<float>& vertices, const vector<float>& normals, const string& fileName) {
    // Open or create a file with the provided file name.
    ofstream file(fileName);

    // Check if the file was successfully opened/created.
    if(file.fail()) {
        printf("ERROR: Can't create file :(");
        return; // Exit the function if file creation/opening fails.
    }

    // Calculate the number of vertices and faces.
    // Each vertex is represented by 3 floats (x, y, z), so the total number of vertices is the size of the vertices vector divided by 3.
    // Each face is represented by 9 floats (3 vertices per face), so the total number of faces is the size of the vertices vector divided by 9.
    int verticesNum = vertices.size() / 3;
    int facesNum = vertices.size() / 9;

    // Write the PLY file header with format specifications and element properties.
    file << "ply" << "\n";
    file << "format ascii 1.0" << "\n";
    file << "element vertex " << verticesNum << "\n";
    file << "property float x" << "\n";
    file << "property float y" << "\n";
    file << "property float z" << "\n";
    file << "property float nx" << "\n"; // x-component of the normal
    file << "property float ny" << "\n"; // y-component of the normal
    file << "property float nz" << "\n"; // z-component of the normal
    file << "element face " << facesNum << "\n";
    file << "property list uchar uint vertex_indices" << "\n"; // Face property indicating the indices of the vertices that form the face
    file << "end_header" << "\n";

    // Write vertex positions and normals to the file.
    for (size_t i = 0; i < vertices.size(); i += 3) {
        // For each vertex, write its position (x, y, z) followed by its normal (nx, ny, nz).
        file << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << " ";
        file << normals[i] << " " << normals[i + 1] << " " << normals[i + 2] << "\n";
    }

    // Write face data to the file. Each face is defined by 3 vertices.
    for (size_t i = 0; i < vertices.size(); i += 9) {
        // The '3' indicates that each face is a triangle composed of 3 vertices.
        // The following numbers are the indices of the vertices that form each face.
        file << "3 " << i / 3 << " " << i / 3 + 1 << " " << i / 3 + 2 << "\n";
    }

    // Close the file.
    file.close();
}
// The 'render' function is responsible for rendering 3D geometry.
void render (std::vector<float> vertices, std::vector<float> normalVertices, glm::mat4 MVP) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    // Read the Vertex Shader code from the file
    std::string VertexShaderCode = "\
    	#version 330 core\n\
		// Input vertex data, different for all executions of this shader.\n\
		layout(location = 0) in vec3 vertexPosition;\n\
		layout(location = 1) in vec2 uv;\n\
		// Output data ; will be interpolated for each fragment.\n\
		out vec2 uv_out;\n\
		// Values that stay constant for the whole mesh.\n\
		uniform mat4 MVP;\n\
		void main(){ \n\
			// Output position of the vertex, in clip space : MVP * position\n\
			gl_Position =  MVP * vec4(vertexPosition,1);\n\
			// The color will be interpolated to produce the color of each fragment\n\
			uv_out = uv;\n\
		}\n";

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode = "\
		#version 330 core\n\
		in vec2 uv_out; \n\
		uniform sampler2D tex;\n\
		void main() {\n\
			gl_FragColor = texture(tex, uv_out);\n\
		}\n";

    // Compile the vertex shader.
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer , NULL);
    glCompileShader(VertexShaderID);

    // Compile the fragment shader.
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer , NULL);
    glCompileShader(FragmentShaderID);

    // Link the compiled vertex and fragment shaders into a shader program.
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Detach and delete the shaders now that they are linked into the program; they are no longer needed.
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    // Create a Vertex Array Object (VAO) to store the vertex attribute configuration.
    GLuint VAOID;
    glGenVertexArrays(1, &VAOID);
    glBindVertexArray(VAOID);

    // Create a Vertex Buffer Object (VBO) and upload the vertex data to it.
    GLuint VBOID;
    glGenBuffers(1, &VBOID);
    glBindBuffer(GL_ARRAY_BUFFER, VBOID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

    // Configure the vertex attribute pointer for vertex positions.
    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(float),
                          (void*)0
    );
    glEnableVertexAttribArray(0);

    // Create a buffer for normals and upload the normal data.
    GLuint NBOID;
    glGenBuffers(1, &NBOID);
    glBindBuffer(GL_ARRAY_BUFFER, NBOID);
    glBufferData(GL_ARRAY_BUFFER, normalVertices.size() * sizeof(float), normalVertices.data(), GL_DYNAMIC_DRAW);

    // Configure the vertex attribute pointer for normals.
    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          3 * sizeof(float),
                          (void*)0
    );
    glEnableVertexAttribArray(1);
}

// Function to load, compile, and link vertex and fragment shaders.
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path) {
    // Create shader objects.
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the vertex shader code from the provided file path.
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if(VertexShaderStream.is_open()) {
        // Use a string stream to read the file's contents into a string.
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str(); // Store the shader code in a string.
        VertexShaderStream.close(); // Close the file stream.
    } else {
        // Print an error message if the file couldn't be opened and exit the function.
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0; // Return 0 to indicate failure.
    }

    // Read the fragment shader code from the provided file path, similar to the vertex shader code reading process.
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if(FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE; // Variable to store compilation/linking success status.
    int InfoLogLength; // Variable to store the length of the log message.

    // Compile the vertex shader.
    printf("Compiling shader : %s\n", vertex_file_path);
    char const * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check and print any compile errors.
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile the fragment shader, following the same steps as for the vertex shader.
    printf("Compiling shader : %s\n", fragment_file_path);
    char const * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check and print any compile errors for the fragment shader.
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the compiled vertex and fragment shaders into a shader program.
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check and print any linking errors.
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    // Detach and delete the shader objects now that they are linked into the program, as they are no longer needed.
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    // Return the ID of the linked shader program.
    return ProgramID;
}

// Callback function for handling cursor position changes.
void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    // Check if the mouse is being dragged (mouse button is pressed).
    if (dragging) {
        // Calculate the change in cursor position since the last event.
        double deltaX = xpos - lastXPos;
        double deltaY = ypos - lastYPos;

        // Update the last known cursor position.
        lastXPos = xpos;
        lastYPos = ypos;

        // Convert the cursor movement into rotation angles.
        float theta = deltaX * 0.01f; // Horizontal movement affects the azimuth angle.
        float phi = -deltaY * 0.01f; // Vertical movement affects the elevation angle.

        // Update the camera's orientation based on the cursor movement.
        camera.updateOrientation(theta, phi);
    }
}

// Callback function for handling keyboard events.
void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Zoom in the camera when the UP key is pressed.
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        camera.updateRadius(-0.1f); // Decrease the camera's radius to zoom in.
    }
    // Close the window when the ESC key is pressed.
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE); // Signal that the window should close.
    }
    // Zoom out the camera when the DOWN key is pressed.
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        camera.updateRadius(0.1f); // Increase the camera's radius to zoom out.
    }
}

// Callback function for handling mouse button events.
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Check for left mouse button events.
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        // Stop dragging when the left mouse button is released.
        if (action == GLFW_RELEASE) {
            dragging = false;
        }
        // Start dragging when the left mouse button is pressed.
        else if (action == GLFW_PRESS) {
            dragging = true;
            // Capture the cursor position at the start of the drag.
            glfwGetCursorPos(window, &lastXPos, &lastYPos);
        }
    }
}

int main() {

    // Set an isovalue for the marching cubes algorithm. This value determines the threshold at which the surface is created.
    float isoval = 1;

    // Define the minimum and maximum values of the scalar field in which we'll compute the surface.
    float min = -5.5f;
    float max = 5.5f;

    // Define the step size for the marching cubes algorithm. This affects the resolution of the generated mesh.
    float stepsize = 0.1f;

    // Initialize GLFW, a library for creating windows, contexts, and managing input and events.
    if( !glfwInit() ) {
        getchar(); // Wait for user input before closing, in case of initialization failure.
        return -1; // Return an error code.
    }

    GLFWwindow* window; // Declare a pointer to a GLFWwindow structure.

    // Set GLFW window hint, specifically the number of samples to use for multisampling. This can help with making the edges smoother.
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a GLFW window with dimensions 1400x900, titled "Assignment 5". The last two parameters are for window sharing and are usually set to NULL.
    window = glfwCreateWindow(1400, 900, "dynamic mesh generation", NULL, NULL);

    // Check if the window was successfully created.
    if( window == NULL ){
        getchar(); // Wait for user input before closing, in case the window creation fails.
        glfwTerminate(); // Terminate GLFW, freeing any resources allocated by GLFW.
        return -1; // Return an error code.
    }

    // Make the OpenGL context of the window the current context on the calling thread.
    glfwMakeContextCurrent(window);

    // Initialize GLEW, a library to load OpenGL extensions.
    if (glewInit() != GLEW_OK) {
        getchar(); // Wait for user input before closing, in case GLEW initialization fails.
        glfwTerminate(); // Terminate GLFW, freeing any resources allocated by GLFW.
        return -1; // Return an error code.
    }

    // Set the clear color for OpenGL. This color will be used when clearing the color buffer.
    glClearColor(0.2f, 0.2f, 0.3f, 0.0f);

    // Enable depth testing, which allows OpenGL to decide which objects are in front and which are at the back.
    glEnable(GL_DEPTH_TEST);

    // Set the depth function to GL_LESS, which means that closer objects will obscure further ones.
    glDepthFunc(GL_LESS);

    // Call the marching cubes algorithm to generate vertices for a 3D shape based on a scalar field, using the parameters defined earlier.
    std::vector<float> vertices = marching_cubes(
        f3, // Scalar field function or data
        -1.5, // Number of divisions along each axis. Higher numbers increase resolution but also computational cost.
        min, // Minimum value of the scalar field
        max, // Maximum value of the scalar field
        stepsize // Step size for the algorithm
    );

    // Calculate normals for the vertices of the mesh. Normals are essential for lighting calculations in 3D graphics.
    std::vector<float> normals = compute_normals(vertices);

    // Write the vertices and their normals to a PLY (Polygon File Format) file. This format is commonly used for storing 3D data.
    writePLY(vertices, normals, "output3.ply");

    // Declare a 4x4 matrix for the Model-View-Projection transformation, which is used to transform vertices from model space to screen space.
    mat4 mvp;

    // Load vertex and fragment shaders from files and compile them into a shader program. Shaders are small programs that run on the GPU to control the rendering pipeline.
    GLuint shaderProgram = LoadShaders("shader.vert", "shader.frag");

    // Define the color of the model as cyan (RGB: 0, 1, 1). This value can be used in the fragment shader to color the model.
    vec3 modelColor = vec3(0.0f, 1.0f, 1.0f);

    // Set the direction of the light source. Normalizing the vector ensures it has a length of 1, making it a direction vector.
    vec3 lightDir = normalize(vec3(5.0f, 5.0f, 5.0f));

    // Declare a Vertex Array Object (VAO). VAOs store pointers to vertex buffer objects and the configuration of vertex attributes.
    GLuint VAO;

    // Declare a Vertex Buffer Object (VBO) for storing vertex data (positions, colors, etc.) in GPU memory for efficient access by the vertex shader.
    GLuint VBO;

    // Declare a Normal Buffer Object (NBO) for storing vertex normals. Normals are used in lighting calculations to determine how light interacts with the surface of the model.
    GLuint NBO;


	// Continuously check if the window should close or if the ESC key is pressed. If neither is true, the loop continues.
    while(glfwWindowShouldClose(window) == 0 && glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS) {
        // Clear the color and depth buffers to reset the frame and prepare for new drawing.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Store the current cursor positions for use in camera or object manipulations.
        double currentXPos = lastXPos;
        double currentYPos = lastYPos;

        // Set the mouse button callback function for handling mouse button events.
        glfwSetMouseButtonCallback(window, mouseButtonCallback);

        // Set the cursor position callback function for handling cursor movement events.
        glfwSetCursorPosCallback(window, cursorPositionCallback);

        // Set the keyboard callback function for handling key press events.
        glfwSetKeyCallback(window, keyboardCallback);

        // Obtain the view matrix from the camera, which defines the position and orientation of the camera.
        mat4 v = camera.getViewMatrix();

        // Create a perspective projection matrix with a 45-degree field of view, aspect ratio of 1400/900,
        // and near and far clipping planes at 0.1f and 1000.0f, respectively.
        mat4 projectionMatrix = perspective(radians(45.0f), 1400.0f / 900.0f, 0.1f, 1000.0f);

        // Set the current matrix mode to projection to apply subsequent matrix operations to the projection matrix stack.
        glMatrixMode(GL_PROJECTION);
        glPushMatrix(); // Push the current projection matrix onto the stack.
        glLoadMatrixf(value_ptr(projectionMatrix)); // Replace the current projection matrix with the one we computed.

        // Set the current matrix mode to modelview to apply subsequent matrix operations to the modelview matrix stack.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix(); // Push the current modelview matrix onto the stack.
        glLoadMatrixf(value_ptr(v)); // Replace the current modelview matrix with the camera's view matrix.

        // Compute the Model-View-Projection (MVP) matrix by multiplying the projection, view, and model matrices.
        // Here, the model matrix is an identity matrix, implying no transformation to the model coordinates.
        mvp = projectionMatrix * v * (mat4(1.0f));

        // Generate a single Vertex Array Object (VAO). The VAO stores the layout of our vertex data and is bound before drawing.
        glGenVertexArrays(1, &VAO);

        // Generate a Vertex Buffer Object (VBO) to store the vertex data.
        glGenBuffers(1, &VBO);

        // Generate a Normal Buffer Object (NBO) to store the normals for the vertices.
        glGenBuffers(1, &NBO);

        // Bind the VAO as the current vertex array to be used.
        glBindVertexArray(VAO);

        // Bind the VBO as the current GL_ARRAY_BUFFER. Subsequent buffer data operations will affect this VBO.
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        // Upload the vertex data to the VBO. The data is dynamic, as it might change (hence GL_DYNAMIC_DRAW).
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        // Define an array of generic vertex attribute data. The parameters specify the location (0), number of components (3, for x/y/z),
        // type of data (GL_FLOAT), normalization flag (GL_FALSE), stride (distance between consecutive vertex attributes),
        // and the offset within the buffer (0 in this case).
        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              3 * sizeof(float),
                              (void*)0
        );

        // Enable the vertex attribute array for location 0, making it available for rendering.
        glEnableVertexAttribArray(0);

        // Bind the NBO as the current GL_ARRAY_BUFFER to upload normal data.
        glBindBuffer(GL_ARRAY_BUFFER, NBO);

        // Upload the normal data to the NBO. This data is also dynamic, hence GL_DYNAMIC_DRAW.
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(float), normals.data(), GL_DYNAMIC_DRAW);

        // Define another array of generic vertex attribute data for normals, using location 1.
        glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              3 * sizeof(float),
                              (void*)0
        );

        // Enable the vertex attribute array for location 1, making the normals available for rendering.
        glEnableVertexAttribArray(1);

        // Activate the shader program to be used in rendering.
        glUseProgram(shaderProgram);

        // Retrieve the location of the "MVP" uniform variable within the shader program.
        GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
        // Pass the MVP matrix to the shader program.
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &mvp[0][0]);

        // Retrieve the location of the "V" (View matrix) uniform variable.
        GLuint ViewID = glGetUniformLocation(shaderProgram, "V");
        // Pass the view matrix to the shader program.
        glUniformMatrix4fv(ViewID, 1, GL_FALSE, &v[0][0]);

        // Retrieve the location of the "LightDir" uniform variable.
        GLuint LightID = glGetUniformLocation(shaderProgram, "LightDir");
        // Pass the light direction vector to the shader program.
        glUniform3fv(LightID, 1, &lightDir[0]);

        // Retrieve the location of the "modelColor" uniform variable.
        GLuint ModelColorID = glGetUniformLocation(shaderProgram, "modelColor");
        // Pass the model color vector to the shader program.
        glUniform3fv(ModelColorID, 1, &modelColor[0]);

        // Retrieve the location of the "ambientColor" uniform variable and set its value.
        GLuint ambientColorID = glGetUniformLocation(shaderProgram, "ambientColor");
        glUniform3f(ambientColorID, 0.2f, 0.2f, 0.2f); // Set ambient color.

        // Retrieve the location of the "specularColor" uniform variable and set its value.
        GLuint specularColorID = glGetUniformLocation(shaderProgram, "specularColor");
        glUniform3f(specularColorID, 1.0f, 1.0f, 1.0f); // Set specular color.

        // Retrieve the location of the "shininess" uniform variable and set its value.
        GLuint shininessID = glGetUniformLocation(shaderProgram, "shininess");
        glUniform1f(shininessID, 64.0f); // Set shininess factor.

        // Enable lighting by setting the corresponding uniform variable in the shader.
        glUniform1i(glGetUniformLocation(shaderProgram, "enableLighting"), 1);

        // Bind the VAO containing the vertex attributes for drawing.
        glBindVertexArray(VAO);

        // Draw the vertices as triangles starting from index 0.
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 3);

        // Clean up by deleting the VAO and VBOs after drawing is done.
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &NBO);

        // Disable lighting after drawing is complete.
        glUniform1i(glGetUniformLocation(shaderProgram, "enableLighting"), 0);

        // Reset to the default shader program.
        glUseProgram(0);

        // Enable blending for subsequent drawing commands.
        glEnable(GL_BLEND);
        // Set the blending function to interpolate the foreground and background.
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Disable updates to the depth buffer.
        glDepthMask(GL_FALSE);

        // Set the width of lines and size of points for drawing.
        glLineWidth(1.0f);
        glPointSize(1.0f);

        // Set the color for drawing lines, with an alpha value for transparency.
        glColor4f(1.0f, 1.0f, 1.0f, 0.5f);

        // Begin drawing lines.
        glBegin(GL_LINES);

        // Define vertices for the bottom face of the bounding box
        glVertex3f(min, min, min);
        glVertex3f(min, min, max);
        glVertex3f(max, min, min);
        glVertex3f(max, min, max);
        glVertex3f(max, max, min);
        glVertex3f(max, max, max);
        glVertex3f(min, max, min);
        glVertex3f(min, max, max);

        // Define vertices for the edges connecting the bottom and top faces of the bounding box
        glVertex3f(min, min, max);
        glVertex3f(max, min, max);
        glVertex3f(max, min, max);
        glVertex3f(max, max, max);
        glVertex3f(max, max, max);
        glVertex3f(min, max, max);
        glVertex3f(min, max, max);
        glVertex3f(min, min, max);

        // Define vertices for the top face of the bounding box
        glVertex3f(min, min, min);
        glVertex3f(max, min, min);
        glVertex3f(max, min, min);
        glVertex3f(max, max, min);
        glVertex3f(max, max, min);
        glVertex3f(min, max, min);
        glVertex3f(min, max, min);
        glVertex3f(min, min, min);

        glEnd(); // End of line drawing for the bounding box

        // Disable depth mask to draw semi-transparent coordinate axes on top of the rendered scene
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);

        // Set up variables representing the direction of each coordinate axis
        vec3 xpoint = vec3(1.0f, 0.0f, 0.0f); // X-axis direction
        vec3 ypoint = vec3(0.0f, 1.0f, 0.0f); // Y-axis direction
        vec3 zpoint = vec3(0.0f, 0.0f, 1.0f); // Z-axis direction

        // Save the current modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();

        // Increase the line width for drawing the coordinate axes
        glLineWidth(3.5f);
        glBegin(GL_LINES);

        // Draw the Z-axis (in blue)
        glColor3f(zpoint.x, zpoint.y, zpoint.z);
        // Starting point of the Z-axis
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y, vec3(min, min, min).z);
        // Ending point of the Z-axis, extending from the starting point along the Z dimension
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y, vec3(min, min, min).z + vec3(max - min, max - min, max - min).z);

        // Draw the first part of the arrowhead for the Z-axis at the end point
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y, vec3(min, min, min).z + vec3(max - min, max - min, max - min).z);
        glVertex3f(vec3(min, min, min).x+0.1, vec3(min, min, min).y, vec3(min, min, min).z + vec3(max - min, max - min, max - min).z);

        // Draw the second part of the arrowhead for the Z-axis at the end point
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y, vec3(min, min, min).z + vec3(max - min, max - min, max - min).z);
        glVertex3f(vec3(min, min, min).x-0.1, vec3(min, min, min).y, vec3(min, min, min).z + vec3(max - min, max - min, max - min).z);

        // Draw the Y-axis (in green)
        glColor3f(ypoint.x, ypoint.y, ypoint.z);
        // Starting point of the Y-axis
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y, vec3(min, min, min).z);
        // Ending point of the Y-axis, extending from the starting point along the Y dimension
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y + vec3(max - min, max - min, max - min).y, vec3(min, min, min).z);

        // Draw the first part of the arrowhead for the Y-axis at the end point
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y + vec3(max - min, max - min, max - min).y, vec3(min, min, min).z);
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y + vec3(max - min, max - min, max - min).y, vec3(min, min, min).z+0.1);

        // Draw the second part of the arrowhead for the Y-axis at the end point
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y + vec3(max - min, max - min, max - min).y, vec3(min, min, min).z);
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y + vec3(max - min, max - min, max - min).y, vec3(min, min, min).z-0.1);

        // Draw the X-axis (in red)
        glColor3f(xpoint.x, xpoint.y, xpoint.z);
        // Starting point of the X-axis
        glVertex3f(vec3(min, min, min).x, vec3(min, min, min).y, vec3(min, min, min).z);
        // Ending point of the X-axis, extending from the starting point along the X dimension
        glVertex3f(vec3(min, min, min).x + vec3(max - min, max - min, max - min).x, vec3(min, min, min).y, vec3(min, min, min).z);

        // Draw the first part of the arrowhead for the X-axis at the end point
        glVertex3f(vec3(min, min, min).x + vec3(max - min, max - min, max - min).x, vec3(min, min, min).y, vec3(min, min, min).z);
        glVertex3f(vec3(min, min, min).x + vec3(max - min, max - min, max - min).x, vec3(min, min, min).y, vec3(min, min, min).z+0.1);

        // Draw the second part of the arrowhead for the X-axis at the end point
        glVertex3f(vec3(min, min, min).x + vec3(max - min, max - min, max - min).x, vec3(min, min, min).y, vec3(min, min, min).z);
        glVertex3f(vec3(min, min, min).x + vec3(max - min, max - min, max - min).x, vec3(min, min, min).y, vec3(min, min, min).z-0.1);

        glEnd(); // End the current drawing operation

        glPopMatrix(); // Restore the previous modelview matrix state
        glPopMatrix(); // Restore the previous modelview matrix state again, assuming nested transformations were used

        glMatrixMode(GL_PROJECTION); // Switch to projection matrix mode to revert projection settings
        glPopMatrix(); // Restore the previous projection matrix state

        glMatrixMode(GL_MODELVIEW); // Switch back to modelview matrix mode for subsequent operations

        glfwPollEvents(); // Process all pending GLFW events to maintain window responsiveness

        glfwSwapBuffers(window); // Swap the front and back buffers to display the rendered frame

    } // End of the rendering loop

    glfwTerminate(); // Clean up and terminate GLFW, freeing any resources

    return 0; // Exit the program successfully

} // End of the function or program
