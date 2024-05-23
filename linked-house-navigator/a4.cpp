// Include the input/output stream library for reading from and writing to streams
#include <iostream>
// Include the vector library for using dynamic array structures
#include <vector>
// Include the string library for using the string class
#include <string>
// Include the file stream library for reading from and writing to files
#include <fstream>
// Include the string stream library for reading from and writing to strings as streams
#include <sstream>

// Include the standard input/output library for function
#include <stdio.h>
// Include the standard library for functions
#include <stdlib.h>

// Include the GLEW library to manage OpenGL extensions
#include <GL/glew.h>
// Include the GLFW library for creating windows and managing user inputs
#include <GLFW/glfw3.h>

// Include the GLM library for mathematics, specifically for graphics transformations
#include <glm/glm.hpp>
// Include GLM's extension for accessing pointer to the matrix and vector data
#include <glm/gtc/type_ptr.hpp>
// Include GLM's extension for string conversions
#include <glm/gtx/string_cast.hpp>
// Include GLM's extension for matrix transformations
#include <glm/gtc/matrix_transform.hpp>

// Include the thread library for using multi-threading features.
#include <thread>
// Include the chrono library for dealing with time, such as durations, time points, and clocks.
#include <chrono>

// Allows to use library components while not using prefixes
using namespace std;
using namespace glm;
using namespace this_thread;
using namespace chrono;

// Define a structure for storing vertex data, including position (x, y, z), normal vector (nx, ny, nz), color (red, green, blue), and texture coordinates (u, v).
struct VertexData{
	float x, y, z;       // Position of the vertex
	float nx, ny, nz;    // Normal vector for the vertex, used in lighting calculations
	float red, green, blue; // Color of the vertex
	float u, v;          // Texture coordinates for the vertex
};

// Define a structure for storing triangle data, specifically the indices of the vertices that form a triangle.
struct TriData{
	GLuint vertIndex1, vertIndex2, vertIndex3; // Indices of the three vertices that make up the triangle
};


/**
 * Given a file path imagepath, read the data in that bitmapped image
 * and return the raw bytes of color in the data pointer.
 * The width and height of the image are returned in the weight and height pointers,
 * respectively.
 *
 * usage:
 *
 * unsigned char* imageData;
 * unsigned int width, height;
 * loadARGB_BMP("mytexture.bmp", &imageData, &width, &height);
 *
 * Modified from https://github.com/opengl-tutorials/ogl.
 */
void loadARGB_BMP(const char* imagepath, unsigned char** data, unsigned int* width, unsigned int* height) {

    printf("Reading image %s\n", imagepath);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    // Actual RGBA data

    // Open the file
    FILE * file = fopen(imagepath,"rb");
    if (!file){
        printf("%s could not be opened. Are you in the right directory?\n", imagepath);
        getchar();
        return;
    }

    // Read the header, i.e. the 54 first bytes

    // If less than 54 bytes are read, problem
    if ( fread(header, 1, 54, file)!=54 ){
        printf("Not a correct BMP file1\n");
        fclose(file);
        return;
    }

    // Read the information about the image
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    *width      = *(int*)&(header[0x12]);
    *height     = *(int*)&(header[0x16]);
    // A BMP files always begins with "BM"
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file2\n");
        fclose(file);
        return;
    }
    // Make sure this is a 32bpp file
    if ( *(int*)&(header[0x1E])!=3  ) {
        printf("Not a correct BMP file3\n");
        fclose(file);
        return;
    }
    // fprintf(stderr, "header[0x1c]: %d\n", *(int*)&(header[0x1c]));
    // if ( *(int*)&(header[0x1C])!=32 ) {
    //     printf("Not a correct BMP file4\n");
    //     fclose(file);
    //     return;
    // }

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=(*width)* (*height)*4; // 4 : one byte for each Red, Green, Blue, Alpha component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    *data = new unsigned char [imageSize];

    if (dataPos != 54) {
        fread(header, 1, dataPos - 54, file);
    }

    // Read the actual data from the file into the buffer
    fread(*data,1,imageSize,file);

    // Everything is in memory now, the file can be closed.
    fclose (file);
}

// Define a function to read vertex and triangle data from a PLY file
void readPLYfile(string fname, vector<VertexData>& vertices, vector<TriData>& faces){

    // Open the PLY file with the given filename.
    ifstream file(fname);
    string line, word; // Variables to hold lines and words from the file

    // Initialize counters for faces and vertices
    int faceCounter = 0;
    int verticesCounter = 0;

    // Read the first line of the file to skip the PLY header start line
    getline(file, line);
    // Loop through the file line by line until the end of the header is reached
    while (getline(file, line)){

        // Use a string stream to parse the line into words
        istringstream iss(line);

        // Store the words of the line in a vector
        vector<string> wordArr;

        // Split the line into words based on space as delimiter and add them to the vector
        while (getline(iss, word, ' ')){
            wordArr.push_back(word);
        }
        // Check if the end of the header is reached
        if(wordArr[0] == "end_header") {
            break;
        }
        // Check if the line indicates the number of elements
        else if (wordArr[0] == "element"){
            // If it's a line specifying the number of faces, store that number
            if(wordArr[1] == "face") {
                faceCounter = stoi(wordArr[2]);
            }
            // If it's a line specifying the number of vertices, store that number
            else if(wordArr[1] == "vertex") {
                verticesCounter = stoi(wordArr[2]);
            }
        }
    }

    // Read vertex data from the file based on the previously counted number of vertices
    for (int i = 0; i < verticesCounter; i++){
        getline(file, line); // Get the line containing vertex data
        istringstream iss(line);

        // Vector to hold the vertex values as floats
        vector<float> vertex_values;

        // Parse and convert vertex data from the line into floats
        while (getline(iss, word, ' ')){
            vertex_values.push_back(stof(word));
        }

        // Create a VertexData struct to store the vertex properties and add it to the vertices vector
        VertexData vertex_properties;

        vertex_properties.x = vertex_values[0];
        vertex_properties.y = vertex_values[1];
        vertex_properties.z = vertex_values[2];
        vertex_properties.nx = vertex_values[3];
        vertex_properties.ny = vertex_values[4];
        vertex_properties.nz = vertex_values[5];
        vertex_properties.u = vertex_values[6];
        vertex_properties.v = vertex_values[7];

        vertices.push_back(vertex_properties);
    }

    // Read triangle data from the file based on the previously counted number of faces
    for (int i = 0; i < faceCounter; i++){

        // Variable to hold the number of vertices per face (should be 3 for a triangle)
        int vertexVal = 0;

        // Vector to hold the vertex indices for a face
        vector<int> vertex_values;
        getline(file, line); // Get the line containing face data
        istringstream iss(line);

        // Parse the first value in the line which indicates the number of vertices for the face
        getline(iss, word, ' ');
        vertexVal = stoi(word);

        // Parse and convert vertex indices from the line into integers
        while (getline(iss, word, ' ')){
            vertex_values.push_back(stoi(word));
        }

        // Create a TriData struct to store the vertex indices of the triangle and add it to the faces vector
        TriData vertex_indices;

        vertex_indices.vertIndex1 = vertex_values[0];
        vertex_indices.vertIndex2 = vertex_values[1];
        vertex_indices.vertIndex3 = vertex_values[2];

        faces.push_back(vertex_indices);
    }
}

// A class designed to represent a textured 3D mesh within an OpenGL context and render the mesh with textures applied to it
class TexturedMesh {


    // OpenGL IDs for vertex buffer objects (VBOs), texture objects, and shader program associated with this textured mesh.

    GLuint vertexPosID; // ID for the VBO that stores vertex positions
    GLuint textureCoordinateID; // ID for the VBO that stores texture coordinates for each vertex
    GLuint faceIndicesID; // ID for the Element Buffer Object (EBO) or Index Buffer that stores indices of vertices that form each face (triangle) of the mesh
    GLuint textureObjID; // ID for the texture object that holds the texture image used on the mesh
    GLuint textureMeshID; // ID for the Vertex Array Object (VAO) that encapsulates the VBOs and EBO for the mesh
    GLuint shaderProgramID; // ID for the shader program used to render this mesh

	public:
        // Paths to the source files for the mesh and texture data.
        string plyFilePath; // Path to the PLY file containing the mesh data
        string bitmapImgFilePath; // Path to the bitmap image file used as a texture for the mesh

        // Vectors to hold the vertex and face data extracted from the PLY file.
        vector<VertexData> verticesDataObj;
        vector<TriData> facesDataObj;

        // Texture data and its dimensions.
        unsigned char* texture_data;
        unsigned int texture_width, texture_height;


    // Constructor for the TexturedMesh class that initializes a textured mesh object using the specified PLY and bitmap image file paths
    TexturedMesh(string plyfile_path, string bitmapimg_path){
        // Store the file paths for the PLY file and bitmap image to instance variables
        bitmapImgFilePath = bitmapimg_path;
        plyFilePath = plyfile_path;

        // Read the vertex and face (triangle) data from the specified PLY file and store it in the `verticesDataObj` and `facesDataObj` vectors
        readPLYfile(plyFilePath, verticesDataObj, facesDataObj);

        // Load the texture data from the specified bitmap image file
        loadARGB_BMP(bitmapImgFilePath.data(), &texture_data, &texture_width, &texture_height);

        // Create shader objects for the vertex and fragment shaders
        GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
        GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

        // Create vertex shader code:
        string VertexShaderCode = "\
            #version 330 core\n\
            layout(location = 0) in vec3 vertexPosition;\n\
            layout(location = 1) in vec2 uv;\n\
            out vec2 uv_out;\n\
            uniform mat4 MVP;\n\
            void main(){\n\
            gl_Position = MVP * vec4(vertexPosition, 1);\n\
            uv_out = uv;\n\
        }\n";

        // Create fragment shader code:
        string FragmentShaderCode = "\
            #version 330 core\n\
            in vec2 uv_out;\n\
            uniform sampler2D tex;\n\
            void main() {\n\
            gl_FragColor = texture(tex, uv_out);\n\
        }\n";

        // Generate a VBO for vertex positions and bind it
        glGenBuffers(1, &vertexPosID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexPosID);

        // Generate a Vertex Array Object (VAO) and bind it
        glGenVertexArrays(1, &textureMeshID);
        glBindVertexArray(textureMeshID);

        // Upload the vertex position data to the GPU and store it in the bound VBO
        glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 11 * verticesDataObj.size(), &(verticesDataObj[0]), GL_STATIC_DRAW);

        // Enable the vertex attribute at location 0
        glEnableVertexAttribArray(0);
        // Define the layout of the vertex position data in the VBO
        glVertexAttribPointer(
            0, // The index of the vertex attribute to configure
            3, // The number of components per vertex attribute
            GL_FLOAT, // The data type of each component
            GL_FALSE, // Whether the data should be normalized
            sizeof(GL_FLOAT) * 11, // The stride, or distance in bytes, between consecutive vertex attributes
            (void*) 0 // The first component of the first vertex attribute in the buffer
        );

        // Generate and bind a buffer for texture coordinates
        glGenBuffers(1, &textureCoordinateID);
        glBindBuffer(GL_ARRAY_BUFFER, textureCoordinateID);
        // Upload texture coordinate data for the vertices to the GPU, storing it in the currently bound buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(GL_FLOAT) * 11 * verticesDataObj.size(), &(verticesDataObj[0]), GL_STATIC_DRAW);
        // Enable the vertex attribute array for texture coordinates at location 1, as it will be used in the shader
        glEnableVertexAttribArray(1);

        // Specify how the texture coordinate data should be read by the vertex shader from the currently bound buffer
        glVertexAttribPointer(
            1, // Attribute index 1 in the shader, where the texture coordinate data will be accessed
            2, // Number of components per texture coordinate
            GL_FLOAT, // Type of the components
            GL_FALSE, // Normalization not required
            sizeof(GL_FLOAT) * 11, // Bytes between consecutive sets of texture coordinates in the buffer
            (void*) 36 // The first texture coordinate in the buffer
        );

        // Generate and bind a buffer for the indices of the mesh faces.
        glGenBuffers(1, &faceIndicesID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, faceIndicesID);
        // Upload the indices data to the GPU, storing it in the currently bound element buffer
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GL_UNSIGNED_INT) * 3 * facesDataObj.size(), &(facesDataObj[0]), GL_STATIC_DRAW);
        // Unbind the Vertex Array Object to avoid accidental modifications
        glBindVertexArray(0);

        // Convert vertex shader source code to a C-style string and set it as the source for the vertex shader object
        char const *VertexSourcePointer = VertexShaderCode.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID); // Compile the vertex shader

        // Convert fragment shader source code to a C-style string and set it as the source for the fragment shader object
        char const *FragmentSourcePointer = FragmentShaderCode.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID); // Compile the fragment shader

        // Create a shader program object and attach the compiled vertex and fragment shaders to it
        shaderProgramID = glCreateProgram();
        glAttachShader(shaderProgramID, VertexShaderID);
        glAttachShader(shaderProgramID, FragmentShaderID);
        glLinkProgram(shaderProgramID); // Link the attached shaders to create an executable program

        // Once linked, the shaders can be detached and deleted
        glDetachShader(shaderProgramID, VertexShaderID);
        glDetachShader(shaderProgramID, FragmentShaderID);
        glDeleteShader(VertexShaderID);
        glDeleteShader(FragmentShaderID);

        // Generate and bind a texture object to store the texture image.
        glGenTextures(1, &textureObjID);
        glBindTexture(GL_TEXTURE_2D, textureObjID);

        // Upload the texture data to the GPU and store it in the currently bound texture object
        glTexImage2D(
            GL_TEXTURE_2D, // Target texture type
            0, // Mipmap level, 0 is the base level
            GL_RGBA, // Format to store the texture on the GPU
            texture_width, // Texture width
            texture_height, // Texture height
            0, // Border, must be 0
            GL_BGRA, // Format of the pixel data (matches the loaded texture data)
            GL_UNSIGNED_BYTE, // Data type of the pixel data
            texture_data // Pointer to the texture data in memory
        );

        // Generate mipmaps for the texture to improve texture quality at various distances
        glGenerateMipmap(GL_TEXTURE_2D);

        // Unbind the texture object to avoid accidental modifications
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // The draw function takes a Model-View-Projection matrix (MVP) as input to control the positioning, rotation, and scaling of the mesh in the scene
    void draw(mat4 MVP){

        // Enable blending to allow for transparent textures
        glEnable(GL_BLEND);
        // Set blending function to interpolate pixel alpha values
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Activate the shader program to be used for rendering
        glUseProgram(shaderProgramID);
        // Get the location of the 'MVP' uniform variable within the shader program
        GLuint MatrixID = glGetUniformLocation(shaderProgramID, "MVP");
        // Pass the MVP matrix to the shader program
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Enable 2D texturing
        glEnable(GL_TEXTURE_2D);
        // Bind the texture object to be used for the mesh
        glBindTexture(GL_TEXTURE_2D, textureObjID);
        // Bind the Vertex Array Object (VAO) that contains our vertex attributes configuration
        glBindVertexArray(textureMeshID);

        // Draw call to render the mesh as a series of triangles
        // The number of elements is the total number of indices in the faces data, multiplied by 3 because each face is a triangle (3 vertices)
        glDrawElements(
            GL_TRIANGLES,             // Render triangles
            facesDataObj.size() * 3,  // Total number of vertices to render
            GL_UNSIGNED_INT,          // Type of the indices
            0                         // Offset in the EBO (Element Buffer Object)
        );
        // Unbind the VAO, shader program, and texture to leave a clean state
        glBindVertexArray(0);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);

    }

};

int main(){
    // Pointer to the GLFW window
    GLFWwindow* window;

    // Initialize GLFW library. If it fails, exit the application
	if (!glfwInit()){
		return -1;
	}

	// If window creation fails, terminate GLFW and exit the application
	if (window == NULL){
		glfwTerminate();
		return -1;
	}

    // Set GLFW window hint for antialiasing (4x multisampling)
	glfwWindowHint(GLFW_SAMPLES, 4);

    // Create a GLFW window with the specified dimensions and title
	window = glfwCreateWindow(1200.0f, 800.0f, "house navigator <3", NULL, NULL);

    // Make the context of the created window current
	glfwMakeContextCurrent(window);

    // Initialize GLEW library. If it fails, terminate GLFW and exit the application
	if (glewInit() != GLEW_OK){
		glfwTerminate();
		return -1;
	}

    // Vector to store multiple textured mesh objects
	vector<TexturedMesh> TextureMesh;

    // Populate the vector with textured mesh objects, specifying paths to PLY files for geometry and bitmap files for textures
    // Each `TexturedMesh` object loads its data and sets up OpenGL resources in its constructor
	TextureMesh.push_back(TexturedMesh("./files/Walls.ply", "./files/walls.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/WoodObjects.ply", "./files/woodobjects.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/Table.ply", "./files/table.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/WindowBG.ply", "./files/windowbg.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/Patio.ply", "./files/patio.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/Floor.ply", "./files/floor.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/Bottles.ply", "./files/bottles.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/DoorBG.ply", "./files/doorbg.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/MetalObjects.ply", "./files/metalobjects.bmp"));
	TextureMesh.push_back(TexturedMesh("./files/Curtains.ply", "./files/curtains.bmp"));

    // Enable depth testing to ensure proper rendering of 3D objects based on their distance from the camera
	glEnable(GL_DEPTH_TEST);
    // Specify the depth comparison function
	glDepthFunc(GL_LESS);
    // Set the clear color for the color buffer
	glClearColor(0.2f, 0.2f, 0.3f, 1.0f);

    // Define camera parameters
	vec3 cameraDir = {0.0f, 0.0f, -1.0f}; // Initial direction where the camera is pointing
	vec3 cameraPos = {0.5f, 0.4f, 0.5f}; // Initial position of the camera
	vec3 upVector = {0.0f, 1.0f, 0.0f}; // The up vector for the camera

    // Define constants for the field of view, camera movement speed, and camera rotation speed
	const float FIELD_OF_VIEW = 45.0f;
    const float CAMERA_FORWARD = 0.05f;
    const float CAMERA_ROTATION = 3.0f;

    // Set desired frame rate and calculate the duration of each frame
    const double desiredFrameRate = 30.0;
    const double frameDuration = 1.0 / desiredFrameRate;

    // Variables to manage frame timing and ensure consistent frame rate
    double previousTime = glfwGetTime();
    double currentTime = glfwGetTime();
    double elapsedTime = currentTime - previousTime;

    float key = 0.0f; // Variable to accumulate camera rotation

    // Main rendering loop, continues until the user closes the window
	while (!glfwWindowShouldClose(window)){
        // Process pending GLFW events.
		glfwPollEvents();

        // Handle user input to control the camera's position and orientation
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
			cameraPos = cameraPos + (cameraDir * CAMERA_FORWARD); // Move camera forward
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
			key = key - CAMERA_ROTATION; // Rotate camera left
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
			cameraPos = cameraPos - (cameraDir * CAMERA_FORWARD); // Move camera backward
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
			key = key + CAMERA_ROTATION; // Rotate camera right
		}

        // Calculate the new camera direction based on accumulated rotation (`key`)
        float x = cos(radians(key));
        float z = sin(radians(key));
		cameraDir = {x, 0.0f, z};

        // Set up the projection and view matrices for the camera
        vec3 center = cameraPos + cameraDir;
		mat4 projectionMatrix = perspective(radians(FIELD_OF_VIEW), ((1200.0f) / (800.0f)), 0.1f, 100.0f);
        mat4 viewMatrix = lookAt(cameraPos, center, upVector);

        // Combine the projection and view matrices for use in the shader
		mat4 MVP = projectionMatrix * viewMatrix;

        // Clear the color and depth buffers.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Render each textured mesh using the combined Model-View-Projection matrix.
		for (int i = 0; i < TextureMesh.size(); i++){
			TextureMesh[i].draw(MVP);
		}

        // Swap the front and back buffers, displaying the rendered frame.
		glfwSwapBuffers(window);

        // Implement a fixed time step for consistent rendering across different hardware.
        currentTime = glfwGetTime();
        elapsedTime = currentTime - previousTime;
        if (elapsedTime < frameDuration) {
            double remainingTime = frameDuration - elapsedTime;
            sleep_for(duration<double>(remainingTime));
        }
        previousTime += frameDuration;
	}

    // Exit the application successfully
	return 0;
}
