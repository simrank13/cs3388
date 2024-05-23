#include <stdio.h>  // Standard I/O operations
#include <stdlib.h>  // Standard library functions
#include <cmath>  // Mathematical functions

#include <GL/glew.h>  // OpenGL Extension Wrangler Library
#include <GLFW/glfw3.h>  // Window management and input

#include <glm/glm.hpp>  // OpenGL Mathematics library
#include <glm/gtc/type_ptr.hpp>  // GLM utility functions
#include <glm/gtx/string_cast.hpp>  // GLM string conversion functions
#include <glm/gtc/matrix_transform.hpp>  // GLM transformation functions

#include <iostream>  // Input-output stream
#include <fstream>  // File stream operations
#include <sstream>  // String stream operations
#include <vector>  // Dynamic array
#include <map>  // Associative arrays
#include "PlaneMesh.hpp"  // Custom class for plane mesh operations

// Additional utilities for file and string operations
#include <string.h>

using namespace glm;  // Simplifies access to GLM types and functions
using namespace std;  // Simplifies access to standard library types and functions

bool dragging = false;  // Indicates whether an object is being dragged (for user interaction)
double lastXPos, lastYPos;  // Stores the last known positions of the cursor (for dragging operations)

// Defines a structure for storing the indices of vertices that form a triangle.
struct TriData {
    GLuint vertex_indices[3]; // Array of 3 GLuints to hold the indices of the vertices forming the triangle.
};

// Defines a structure for storing vertex data, including position, normal, color, and texture coordinates.
struct VertexData {
    vec3 pos; // Stores the x, y, z coordinates of the vertex position.
    vec3 normal;   // Stores the x, y, z components of the vertex normal vector.
    vec3 color;    // Stores the r, g, b values for the vertex color.
    vec2 texturePoints; // Stores the u, v texture coordinates for the vertex.

    // Constructor to initialize vertex data with given values.
    VertexData(vec3 pos, vec3 norm, vec3 col, vec2 tex) :
        pos(pos),        // Initializes position with pos argument.
        normal(norm),         // Initializes normal with norm argument.
        color(col),           // Initializes color with col argument.
        texturePoints(tex)    // Initializes texturePoints with tex argument.
    {}
};


// Definition of the Plane class
class Plane {
public:
    enum class PLANE_WHICH { X, Y, Z };  // Enum to specify the plane's orientation (along X, Y, or Z axis)


private:
    PLANE_WHICH plane = PLANE_WHICH::Y;  // Default orientation of the plane is along the Y axis
    vec4 color = vec4(0.9f, 0.9f, 0.9f, 0.1f);  // Default color of the plane with some transparency (RGBA)
    GLfloat size;  // Size of the plane
    GLuint MID;  // Uniform location for the Model matrix in the shader
    GLuint VID;  // Uniform location for the View matrix in the shader
    GLuint LightPosID;  // Uniform location for the light position in the shader
    GLfloat offset = 0;  // Offset of the plane from the origin, initialized to 0
    GLuint texID = 0;  // Texture ID for applying a texture to the plane, initialized to 0 (no texture)
    GLuint programID;  // ID for the shader program used to render the plane
    GLuint colorID;  // Uniform location for the plane's color in the shader
    GLuint alphaID;  // Uniform location for the plane's alpha (transparency) in the shader
    GLuint MVPID;  // Uniform location for the Model-View-Projection matrix in the shader

public:
    // Constructor for the Plane class that initializes a plane with a specific size and texture
    Plane(GLfloat sz, string textureFile) : size(sz) {  // size of the plane, 'textureFile' is the path to the texture image
        unsigned char* data;  // Pointer to hold the raw texture data
        unsigned int width, height;  // Variables to hold the dimensions of the texture image

        loadBMP(textureFile.c_str(), &data, &width, &height);  // Load the bitmap image and get the texture data along with its dimensions
        fprintf(stderr, "w: %d, h: %d\n", width, height);

        glGenTextures(1, &texID);  // Generate one texture object and store its ID in 'texID'
        glBindTexture(GL_TEXTURE_2D, texID);  // Bind the newly created texture as the current 2D texture
        // Placeholder for texture data loading; actual loading happens after 'loadBMP'
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);  // Allocate memory for the texture
        glGenerateMipmap(GL_TEXTURE_2D);  // Generate mipmaps for the texture
        glBindTexture(GL_TEXTURE_2D, 0);  // Unbind the texture


        programID = LoadShaders( "WaterShader.vertexshader", "WaterShader.fragmentshader" ); // Load and compiler shaders

        MID = glGetUniformLocation(programID, "M");  // Get the location of the Model matrix uniform variable in the shader
        VID = glGetUniformLocation(programID, "V");  // Get the location of the View matrix uniform variable in the shader
        colorID = glGetUniformLocation(programID, "modelcolor");  // Get the location of the model color uniform variable in the shader
        alphaID = glGetUniformLocation(programID, "alpha");  // Get the location of the alpha (transparency) uniform variable in the shader
        LightPosID = glGetUniformLocation(programID, "LightPosition_worldspace");  // Get the location of the light position uniform variable in the shader
        programID = LoadShaders("WaterShader.vertexshader", "WaterShader.fragmentshader");  // Load and compile the vertex and fragment shaders
        MVPID = glGetUniformLocation(programID, "MVP");  // Get the location of the MVP uniform variable in the shader

        glUseProgram(programID);  // Use the shader program
        glUniformMatrix4fv(MID, 1, GL_FALSE, &mat4(1.0f)[0][0]);  // Set the Model matrix to an identity matrix in the shader
    }
    void draw() {
        // Set up the modelview matrix.
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        // Enable blending to support transparency.
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Begin drawing quads.
        glBegin(GL_QUADS);
        // Set the color and transparency of the plane.
        glColor4f(color.r, color.g, color.b, color.a);

        // Draw the plane according to its orientation.
        if (plane == PLANE_WHICH::Y) {
            // Y-oriented plane (horizontal plane)
            glVertex3f(-size, offset, -size);
            glVertex3f(size, offset, -size);
            glVertex3f(size, offset, size);
            glVertex3f(-size, offset, size);
        } else if (plane == PLANE_WHICH::Z) {
            // Z-oriented plane (vertical plane, depth-wise)
            glVertex3f(-size, -size, offset);
            glVertex3f(size, -size, offset);
            glVertex3f(size, size, offset);
            glVertex3f(-size, size, offset);
        } else if (plane == PLANE_WHICH::X) {
            // X-oriented plane (vertical plane, width-wise)
            glVertex3f(offset, -size, -size);
            glVertex3f(offset, size, -size);
            glVertex3f(offset, size, size);
            glVertex3f(offset, -size, size);
        }

        // End drawing.
        glEnd();

        // Restore the previous modelview matrix.
        glPopMatrix();
        // Disable blending.
        glDisable(GL_BLEND);
    }

    void draw(vec3 lightPos, mat4 M, mat4 V, mat4 P, vec4 color, float alpha) {
        // Normal vectors for each vertex, pointing up.
        static float normals[] = {
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
        };

        // Texture coordinates for mapping the texture onto the quad.
        static float uv[] = {
            0.0f, 0.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        // Vertex positions defining the quad corners.
        static float verts[] = {
            -size, offset, -size,  // Bottom Left
            size, offset, -size,   // Bottom Right
            size, offset, size,    // Top Right
            -size, offset, size    // Top Left
        };

        // Calculate the Model-View-Projection matrix for transforming the quad.
        glm::mat4 MVP = P * V * M;
        // Activate the shader program.
        glUseProgram(programID);
        // Pass MVP matrix to the vertex shader.
        glUniformMatrix4fv(MVPID, 1, GL_FALSE, value_ptr(MVP));
        // Pass the view matrix to the vertex shader.
        glUniformMatrix4fv(VID, 1, GL_FALSE, value_ptr(V));
        // Pass the light position to the shader.
        glUniform3f(LightPosID, lightPos.x, lightPos.y, lightPos.z);
        // Pass the transparency (alpha) value to the shader.
        glUniform1f(alphaID, alpha);
        // Bind the texture to be used.
        glBindTexture(GL_TEXTURE_2D, texID);

        // Enable and set the vertex positions attribute.
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, verts);
        // Enable and set the normals attribute.
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, normals);
        // Enable and set the texture coordinates attribute.
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, uv);

        // Draw the quad.
        glDrawArrays(GL_QUADS, 0, 4);
        // Unbind the texture.
        glBindTexture(GL_TEXTURE_2D, 0);
        // Deactivate the shader program.
        glUseProgram(0);
    }

};


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
            float maxPhi = pi<float>() - 0.1f; // Slightly less than Pi to avoid looking directly down

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
        // This function gets the camera's position
        vec3 getPosition() {
            // get camera position
            return this->pos;
        }

};

Camera camera(vec3(0, 0, 0), vec3(5, 5, 5));// Initializes a camera at the origin looking towards (5, 5, 5)

void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
    // Adjusts camera orientation based on cursor movement.
    if (dragging) { // Check if dragging is in progress.
        double deltaX = xpos - lastXPos; // Calculate change in x-position.
        double deltaY = ypos - lastYPos; // Calculate change in y-position.
        lastXPos = xpos; // Update last x-position.
        lastYPos = ypos; // Update last y-position.
        float theta = deltaX * 0.01f; // Determine horizontal angle change.
        float phi = -deltaY * 0.01f; // Determine vertical angle change.
        camera.updateOrientation(theta, phi); // Update camera orientation based on angle changes.
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    // Handles mouse button events for camera control.
    if (button == GLFW_MOUSE_BUTTON_LEFT) { // Check if the left mouse button is pressed or released.
        if (action == GLFW_PRESS) { // Start dragging on mouse press.
            dragging = true; // Set dragging flag.
            glfwGetCursorPos(window, &lastXPos, &lastYPos); // Store cursor position at start of drag.
        } else if (action == GLFW_RELEASE) { // Stop dragging on mouse release.
            dragging = false; // Clear dragging flag.
        }
    }
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Handles keyboard events for camera and application control.
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) { // Zoom in when UP key is pressed.
        camera.updateRadius(-0.1f); // Decrease camera radius to zoom in.
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) { // Close window on ESC key press.
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) { // Zoom out when DOWN key is pressed.
        camera.updateRadius(0.1f); // Increase camera radius to zoom out.
    }
}

void processInput(GLFWwindow* window) {
    // Sets mouse and keyboard input callbacks.
    glfwSetMouseButtonCallback(window, mouseButtonCallback); // Set mouse button callback.
    glfwSetCursorPosCallback(window, cursorPositionCallback); // Set cursor position callback.
    glfwSetKeyCallback(window, keyboardCallback); // Set keyboard callback.
}

// Function to read vertex and face data from a PLY file
void readPLYFile(const std::string& fname, std::vector<VertexData>& vertices, std::vector<TriData>& faces) {
    // Attempt to open the PLY file
    std::ifstream file(fname);
    if (!file) { // If file not found or inaccessible, log an error and exit
        std::cerr << "Error opening file: " << fname << std::endl;
        return;
    }

    std::string line; // Variable to store each line read from the file
    int numVerts = 0, numFaces = 0; // Variables to store the count of vertices and faces
    std::vector<std::string> properties; // Vector to store the properties of each vertex

    // Loop through the header lines to parse metadata
    while (std::getline(file, line) && line != "end_header") {
        std::istringstream stream(line); // Stream to parse each line
        std::string token; // Variable to store each word in a line
        stream >> token; // Read the first word to determine the line type

        if (token == "element") { // If the line defines an element...
            stream >> token; // Read the type of element (vertex or face)
            if (token == "vertex") stream >> numVerts; // If element is a vertex, store its count
            else if (token == "face") stream >> numFaces; // If element is a face, store its count
        } else if (token == "property") { // If the line defines a property...
            stream >> token; // Skip the data type of the property
            stream >> token; // Read the actual property name
            properties.push_back(token); // Store the property name for later use
        }
    }

    // Loop through each vertex in the file
    for (int i = 0; i < numVerts; ++i) {
        std::getline(file, line); // Read a line of vertex data
        std::istringstream stream(line); // Stream to parse the vertex data line

        // Variables to store vertex properties. Default color to white.
        vec3 position, normal, color(1.0f);
        vec2 textureCoords;

        // Loop through each property of the vertex
        for (size_t i = 0; i < properties.size(); ++i) {
            const auto& prop = properties[i];
            if (prop == "x") stream >> position.x; // Extract x-coordinate
            else if (prop == "red") {
                float r;
                stream >> r;
                color.r = r / 255.0f; // Extract red color component and convert to range [0, 1]
            }
            else if (prop == "green") {
                float g;
                stream >> g;
                color.g = g / 255.0f; // Extract green color component and convert to range [0, 1]
            }
            else if (prop == "ny") stream >> normal.y; // Extract y-coordinate of normal
            else if (prop == "y") stream >> position.y; // Extract y-coordinate
            else if (prop == "nx") stream >> normal.x; // Extract x-coordinate of normal
            else if (prop == "z") stream >> position.z; // Extract z-coordinate
            else if (prop == "blue") {
                float b;
                stream >> b;
                color.b = b / 255.0f; // Extract blue color component and convert to range [0, 1]
            }
            else if (prop == "u") stream >> textureCoords.x; // Extract u-coordinate of texture
            else if (prop == "nz") stream >> normal.z; // Extract z-coordinate of normal
            else if (prop == "v") stream >> textureCoords.y; // Extract v-coordinate of texture
        }

        // Use the parsed data to construct a VertexData object and add it to the vertices vector
        vertices.emplace_back(position, normal, color, textureCoords);
}


    // Parse face data
    for (int i = 0; i < numFaces; ++i) {
        getline(file, line); // Read a line of face data
        istringstream stream(line); // Stream to parse the face data line
        unsigned int numVertices;
        stream >> numVertices;

        TriData tri;
        for (int j = 0; j < 3; ++j) { // Assume triangular faces, parse each vertex index
            stream >> tri.vertex_indices[j];
        }
        faces.push_back(tri); // Add the parsed face to the faces vector
    }
    file.close(); // Close the file once parsing is complete
}

class TexturedMesh {
private:
    vector<VertexData> vertices; // Stores the vertices of the mesh, each containing position, normal, color, and texture coordinates.
    vector<TriData> faces; // Stores triangles of the mesh, each containing indices to three vertices.
    GLuint vboNormalPos; // ID for the buffer storing vertices' normal vector data.
    GLuint vboVertexPos; // ID for the buffer storing vertices' position data.
    GLuint vboTexturePoints; // ID for the buffer storing vertices' texture coordinate data.
    GLuint eboFacesIndices; // ID for the buffer storing indices that define each triangle (face) of the mesh.
    GLuint textureID; // ID for the texture to be applied on the mesh.
    GLuint shaderID; // ID for the shader program used for rendering the mesh.
    GLuint vao; // ID for the Vertex Array Object that encapsulates vertex buffer object bindings and vertex attribute configurations.

public:

    // Renders the textured mesh with a given transformation matrix
    void draw(mat4 MVP) {
        // Enable alpha blending for transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use the shader program and set the MVP matrix uniform
        glUseProgram(shaderID);
        GLuint MatrixID = glGetUniformLocation(shaderID, "MVP");
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Enable texture mapping and bind the mesh's texture
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Bind the mesh's vertex array object (VAO) to set up drawing states
        glBindVertexArray(vao);

        // Draw the mesh as a series of triangles using the indices in the element buffer and unbind the VAO to clean up
        size_t num_faces = faces.size() * 3;
        glDrawElements(GL_TRIANGLES, num_faces, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Reset state by unbinding the shader program and texture, and disable blending
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);
    }

    // Constructor for the TexturedMesh class
    TexturedMesh(const char* plyFilePath, const char* filePath, GLuint shaderID) {
        // Prepare vectors for OpenGL attribute data
        vector<GLfloat> vertexPos;
        vector<GLfloat> vertexTexturePoints;
        vector<GLfloat> normalPos;

        // Load vertex and triangle data from a PLY file
        readPLYFile(plyFilePath, vertices, faces);

        // Store the shader ID
        this->shaderID = shaderID;

        // Load texture data from a BMP file
        unsigned char* data;
        GLuint width, height;
        loadARGB_BMP(filePath, &data, &width, &height);

        // Convert texture coordinates from VertexData to a flat vector
        vertexTexturePoints.reserve(vertices.size() * 2);
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertexTexturePoints.push_back(vertices[i].texturePoints.x);
            vertexTexturePoints.push_back(vertices[i].texturePoints.y);
        }

        // Convert vertex normals from VertexData to a flat vector
        for (size_t i = 0; i < vertices.size(); ++i) {
            normalPos.push_back(vertices[i].normal.x);
            normalPos.push_back(vertices[i].normal.y);
            normalPos.push_back(vertices[i].normal.z);
        }

        // Convert vertex positions from VertexData to a flat vector
        vertexPos.reserve(vertices.size() * 3);
        for (size_t i = 0; i < vertices.size(); ++i) {
            vertexPos.push_back(vertices[i].pos.x);
            vertexPos.push_back(vertices[i].pos.y);
            vertexPos.push_back(vertices[i].pos.z);
        }

        // Generate and set up the texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);

        // Bind texture to the shader
        glUseProgram(shaderID);
        glUniform1i(glGetUniformLocation(shaderID, "texture"), 0); // Texture unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUseProgram(0);

        // Setup the vertex array object (VAO)
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        // Setup and enable the texture coordinate attribute
        size_t num_vertexTexPoints = vertexTexturePoints.size();
        glGenBuffers(1, &vboTexturePoints);
        glBindBuffer(GL_ARRAY_BUFFER, vboTexturePoints);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertexTexPoints, vertexTexturePoints.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Setup and enable the vertex position attribute
        size_t num_vertexPositions = vertexPos.size();
        glGenBuffers(1, &vboVertexPos);
        glBindBuffer(GL_ARRAY_BUFFER, vboVertexPos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_vertexPositions, vertexPos.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Setup and bind the element buffer for face indices
        size_t num_faces = faces.size();
        glGenBuffers(1, &eboFacesIndices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboFacesIndices);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 3 * num_faces , faces.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);

        // Setup and enable the normal position attribute
        size_t num_normalPositions = normalPos.size();
        glGenBuffers(1, &vboNormalPos);
        glBindBuffer(GL_ARRAY_BUFFER, vboNormalPos);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * num_normalPositions, normalPos.data(), GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 0, (void*)0);
    }

};


//////////////////////////////////////////////////////////////////////////////
// Main
//////////////////////////////////////////////////////////////////////////////

int main( int argc, char* argv[]) {

	///////////////////////////////////////////////////////
	// Screen width and height
	float screenW = 1400;
	float screenH = 900;
	float stepsize = 1.0f;

    // Step size, and x-range
    //(Note: do not set any value lower than -10 and 10 as that will print blank screen)
	float xmin = -12;
	float xmax = 12;

    // Command-line arguments override default values if provided
	if (argc > 1 ) {
		screenW = atoi(argv[1]); // Set screen width
	}
	if (argc > 2) {
		screenH = atoi(argv[2]); // Set screen height
	}
	if (argc > 3) {
		stepsize = atof(argv[3]); // Set step size
	}
	if (argc > 4) {
		xmin = atof(argv[4]); // Set minimum x-coordinate
	}
	if (argc > 5) {
		xmax = atof(argv[5]); // Set maximum x-coordinate
	}


	///////////////////////////////////////////////////////

	// Initialise GLFW
	if(!glfwInit()) {
		getchar();
		return -1;
	}
    // Set GLFW to use 4x Multisample Anti-Aliasing
    glfwWindowHint(GLFW_SAMPLES, 4);

	// Open a window and create its OpenGL context
	 GLFWwindow* window = glfwCreateWindow( screenW, screenH, "Assignment 6", NULL, NULL);
	if( window == NULL ){
		getchar();
		glfwTerminate();
		return -1;
	}
	// Make the OpenGL context of the specified window current
    glfwMakeContextCurrent(window);

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		getchar();
		glfwTerminate();
		return -1;
	}
    // Load and compile the shaders to create a shader program
    GLuint shaderProgram =  LoadShaders("WaterShader.vertexshader", "WaterShader.geoshader", "WaterShader.fragmentshader");

    // Set up the projection matrix using perspective projection
    mat4 Projection = perspective(radians(45.0f), (float)screenW / (float)screenH, 0.1f, 1000.0f);

    // Push the current projection matrix onto the stack and load the new projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(value_ptr(Projection));

    // Push the current modelview matrix onto the stack and load the camera's view matrix
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(value_ptr(camera.getViewMatrix()));

    // Calculate the model-view-projection matrix
    mat4 MVP = Projection * mat4(1.0f) * camera.getViewMatrix();

    // Define variables for eye position, light position, color, and texture offset
    vec3 lightpos(5.0f, 30.0f, 5.0f);
    vec2 texOffset(0.1f, 0.1f);

    vec4 color1(1.0f, 1.0f, 1.0f, 1.0f);
    vec3 eye = {0.0f, 3.0f, 5.0f};

    // Initialize a vector representing the direction of the light
    vec3 lightDir = normalize(camera.getPosition());

    // Get the uniform locations for the MVP, view, model, light position, and color variables
    GLuint MatrixID = glGetUniformLocation(shaderProgram, "MVP");
    GLuint ViewID = glGetUniformLocation(shaderProgram, "V");
    GLuint ModelID = glGetUniformLocation(shaderProgram, "M");
    GLuint LightID = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint ambientColorID = glGetUniformLocation(shaderProgram, "modelColor");

    // Set the uniform variables for MVP, view, model, light position, and color
    glUniform3fv(LightID, 1, &lightpos[0]);
    glUniform4fv(ambientColorID, 1, &color1[0]);
    glUniformMatrix4fv(ViewID, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    glUniformMatrix4fv(ModelID, 1, GL_FALSE, &mat4(1.0f)[0][0]);
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

    // Load the shader program for the PLY object
    GLuint plyShader = LoadShaders("WaterShader.vertexshader", "WaterShader.fragmentshader");

    // Set uniform variables for PLY shader program
    GLuint plyspecularColorID = glGetUniformLocation(plyShader, "specularColor");
    glUniform4f(plyspecularColorID, 0.0f, 1.0f, 1.0f, 0.5f);

    GLuint plydiffuseID = glGetUniformLocation(plyShader, "diffuse");
    glUniform4f(plydiffuseID, 0.0f, 1.0f, 1.0f, 0.5f);

    // Set shininess uniform variable for the main shader program
    GLuint shininessID = glGetUniformLocation(shaderProgram, "alpha");
    glUniform1f(shininessID, 64.0f);

    // Initialize the PlaneMesh object with given parameters
    PlaneMesh plane(xmin, xmax, stepsize, shaderProgram);

    // Load the projection matrix onto the projection matrix stack
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadMatrixf(value_ptr(Projection));

    // Load the view matrix onto the modelview matrix stack
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(value_ptr(camera.getViewMatrix()));

    // Use the shader program for the PLY object
    glUseProgram(plyShader);

    // Set up uniform variables for the PLY shader program
    GLuint plyLightID = glGetUniformLocation(plyShader, "lightPos");
    glUniform3fv(plyLightID, 1, &lightpos[0]);

    GLuint plyModelID = glGetUniformLocation(plyShader, "M");
    glUniformMatrix4fv(plyModelID, 1, GL_FALSE, &mat4(1.0f)[0][0]);

    GLuint plyViewID = glGetUniformLocation(plyShader, "V");
    glUniformMatrix4fv(plyViewID, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);

    GLuint plyambientColorID = glGetUniformLocation(plyShader, "modelColor");
    glUniform4fv(plyambientColorID, 1, &color1[0]);

    GLuint plyshininessID = glGetUniformLocation(plyShader, "alpha");
    glUniform1f(plyshininessID, 64.0f);

    GLuint plyMatrixID = glGetUniformLocation(plyShader, "MVP");
    glUniformMatrix4fv(plyMatrixID, 1, GL_FALSE, &MVP[0][0]);

    // Initialize LightDir vector
    lightDir = normalize(camera.getPosition());

    // Create TexturedMesh objects for the boat, eyes, and head
    TexturedMesh boat("Assets/boat.ply", "Assets/boat.bmp", plyShader);
    TexturedMesh head("Assets/head.ply", "Assets/head.bmp", plyShader);
    TexturedMesh eyes("Assets/eyes.ply", "Assets/eyes.bmp", plyShader);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set the background color to dark blue
    glClearColor(0.2f, 0.2f, 0.3f, 0.0f);
    // Set the current color to white
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Enable depth testing to ensure correct rendering order based on depth
    glEnable(GL_DEPTH_TEST);
    // Disable back-face culling to render both front and back faces of polygons
    glDisable(GL_CULL_FACE);
    // Specify depth comparison function
    glDepthFunc(GL_LESS);

	do{
		// Clear the screen and processes user input
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		processInput(window);

        // Loads the projection matrix into projection matrix
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadMatrixf(value_ptr(perspective(radians(45.0f), 1400.0f / 900.0f, 0.1f, 1000.0f)));

        // Loads the view matrix into modelview matrix
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadMatrixf(value_ptr(camera.getViewMatrix()));

        glUseProgram(plyShader);

        // Set the value of enableLighting to true for rendering the object
        glUniform1i(glGetUniformLocation(plyShader, "enableLighting"), 1);

        // Calculate the model-view-projection (MVP) matrix using the perspective projection matrix,
        // view matrix, and identity model matrix.
        mat4 MVP = perspective(radians(45.0f), 1400.0f / 900.0f, 0.1f, 1000.0f) * camera.getViewMatrix() * mat4(1.0f);

        // Set the uniform variable for the view matrix in the shader program.
        glUniformMatrix4fv(plyViewID, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);

        // Set the uniform variable for the MVP matrix in the shader program.
        glUniformMatrix4fv(plyMatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Draw boat, eyes, and mesh
		boat.draw(MVP);
		head.draw(MVP);
		eyes.draw(MVP);

        // Draw the plane
        plane.draw(glfwGetTime(), perspective(radians(45.0f), 1400.0f / 900.0f, 0.1f, 1000.0f), camera.getViewMatrix(), inverse(mat4(1.0f)), lightpos, vec2(1.0f, 1.0f), vec2(0.0f, 0.0f), 16.0f, 16.0f, 0.0f);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Terminate GLFW and end program
	glfwTerminate();
	return 0;
}
