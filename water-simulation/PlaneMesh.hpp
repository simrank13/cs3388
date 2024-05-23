#include <stdio.h> // For file and console I/O
#include <string> // For string manipulation
#include <vector> // For dynamic arrays
#include <iostream> // For standard I/O streams
#include <fstream> // For file I/O operations
#include <algorithm> // For common algorithms like sort, search
#include <sstream> // For string stream operations
#include <stdlib.h> // For general utilities like memory management
#include <string.h> // For C string manipulation functions

using namespace std;
using namespace glm;

// Load and compile shaders from files
GLuint LoadShaders(const char * vertex_file_path, const char * fragment_file_path) {
    // Create shader objects
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Load vertex shader code from file
    string VertexShaderCode;
    ifstream VertexShaderStream(vertex_file_path, ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf(); // Read file buffer into string stream
        VertexShaderCode = sstr.str(); // Convert string stream to string
        VertexShaderStream.close(); // Close file stream
    } else {
        // Error handling if file can't be opened
        printf("Impossible to open %s. Check directory and FAQ.\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Load fragment shader code from file
    string FragmentShaderCode;
    ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    // Compile vertex shader
    printf("Compiling shader: %s\n", vertex_file_path);
    const char * VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check vertex shader for errors
    GLint Result = GL_FALSE;
    int InfoLogLength;
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile fragment shader
    printf("Compiling shader: %s\n", fragment_file_path);
    const char * FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check fragment shader for errors
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the vertex and fragment shaders into a shader program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the shader program for errors
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    // Clean up shaders now that they're linked into the program; they're no longer needed
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID; // Return the shader program ID
}

// Function to load, compile, and link shaders from provided file paths
GLuint LoadShaders(const char* vertex_file_path, const char* geo_file_path, const char* fragment_file_path) {
    // Creating shader objects for vertex, geometry, and fragment shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint GeometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Lambda function to read the shader's source code from a file
    auto readShaderCode = [](const char* file_path) -> string {
        ifstream ShaderStream(file_path, ios::in); // Open the file
        if (!ShaderStream.is_open()) { // Check if the file has been successfully opened
            cerr << "Impossible to open " << file_path << ". Are you in the right directory? Don't forget to read the FAQ!\n";
            getchar(); // Wait for a key press
            return ""; // Return an empty string in case of failure
        }
        stringstream sstr;
        sstr << ShaderStream.rdbuf(); // Read the entire file into a string stream
        ShaderStream.close(); // Close the file
        return sstr.str(); // Return the string stream's content as a string
    };

    // Lambda function to compile the shader and check for errors
    auto compileShader = [](GLuint shaderID, const string& shaderCode, const char* shaderName) {
        cout << "Compiling shader: " << shaderName << endl; // Log shader compilation
        char const* sourcePointer = shaderCode.c_str(); // Convert shader code string to C-style string
        glShaderSource(shaderID, 1, &sourcePointer, nullptr); // Set the shader's source code
        glCompileShader(shaderID); // Compile the shader

        // Check compilation result
        GLint result = GL_FALSE;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
        if (result == GL_FALSE) { // If compilation failed
            int infoLogLength;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength); // Get the length of the error log
            vector<char> shaderErrorMessage(infoLogLength); // Create a buffer for the error log
            glGetShaderInfoLog(shaderID, infoLogLength, nullptr, shaderErrorMessage.data()); // Retrieve the error log
            cerr << shaderErrorMessage.data() << endl; // Print the error log
        }
    };

    // Reading and compiling shaders using the above lambda functions
    string VertexShaderCode = readShaderCode(vertex_file_path);
    string GeometryShaderCode = readShaderCode(geo_file_path);
    string FragmentShaderCode = readShaderCode(fragment_file_path);
    compileShader(VertexShaderID, VertexShaderCode, "Vertex Shader");
    compileShader(GeometryShaderID, GeometryShaderCode, "Geometry Shader");
    compileShader(FragmentShaderID, FragmentShaderCode, "Fragment Shader");

    // Linking the compiled shaders into a shader program
    cout << "Linking program" << endl;
    GLuint ProgramID = glCreateProgram(); // Create a shader program
    glAttachShader(ProgramID, VertexShaderID); // Attach the vertex shader
    glAttachShader(ProgramID, GeometryShaderID); // Attach the geometry shader
    glAttachShader(ProgramID, FragmentShaderID); // Attach the fragment shader
    glLinkProgram(ProgramID); // Link the program

    // Check the linking result
    GLint result = GL_FALSE;
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &result);
    if (result == GL_FALSE) { // If linking failed
        int infoLogLength;
        glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &infoLogLength); // Get the length of the error log
        vector<char> programErrorMessage(std::max(infoLogLength, 1)); // Create a buffer for the error log
        glGetProgramInfoLog(ProgramID, infoLogLength, nullptr, programErrorMessage.data()); // Retrieve the error log
        cerr << programErrorMessage.data() << endl; // Print the error log
    }

    // Detach and delete the shaders as they're no longer needed after linking
    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, GeometryShaderID);
    glDetachShader(ProgramID, FragmentShaderID);
    glDeleteShader(VertexShaderID);
    glDeleteShader(GeometryShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID; // Return the ID
}

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

void loadBMP(const char* imagepath, unsigned char** data, unsigned int* width, unsigned int* height) {

    // printf("Reading image %s\n", imagepath);

    // Data read from the header of the BMP file
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    // Actual RGB data

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
        printf("Not a correct BMP file\n");
        fclose(file);
        return;
    }
    // A BMP files always begins with "BM"
    if ( header[0]!='B' || header[1]!='M' ){
        printf("Not a correct BMP file\n");
        fclose(file);
        return;
    }
    // Make sure this is a 24bpp file
    if ( *(int*)&(header[0x1E])!=0  ) {
    	printf("Not a correct BMP file\n");
	    fclose(file);
	    return;
	}
    if ( *(int*)&(header[0x1C])!=24 ) {
    	printf("Not a correct BMP file\n");
	    fclose(file);
	    return;
	}

    // Read the information about the image
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    *width      = *(int*)&(header[0x12]);
    *height     = *(int*)&(header[0x16]);

    // Some BMP files are misformatted, guess missing information
    if (imageSize==0)    imageSize=(*width)* (*height)*3; // 3 : one byte for each Red, Green and Blue component
    if (dataPos==0)      dataPos=54; // The BMP header is done that way

    // Create a buffer
    *data = new unsigned char [imageSize];

    // Read the actual data from the file into the buffer
    fread(*data,1,imageSize,file);

    fprintf(stderr, "Done reading!\n");

    // Everything is in memory now, the file can be closed.
    fclose (file);

}
class PlaneMesh {
private:
    // IDs for texture resources.
    GLuint waterTexture, displacementTexture;
    GLuint shaderProgram; // ID for the GPU shader program.
    // Uniform variables' locations in the shader.
    GLuint modelLoc, viewLoc, projectionLoc, lightPosLoc, cameraViewPos;
    GLuint waterTextureLoc, displacementTextureLoc, timeLoc;
    GLuint inverseID, texOffsetID, texScaleID, outerTessID, innerTessID;



    void setupTextures() {
        GLuint width, height;  // Variables to store texture dimensions.
        unsigned char* data;  // Pointer to hold texture data.

        // Load the water surface texture.
        loadBMP("Assets/water.bmp", &data, &width, &height);

        // Generate a texture ID and bind it as a 2D texture.
        glGenTextures(1, &waterTexture);
        glBindTexture(GL_TEXTURE_2D, waterTexture);

        // Set texture wrapping and filtering modes.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Specify the texture image, including level, internal format, dimensions, and pixel data.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);  // Automatically generate mipmaps for the texture.

        glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture.
        // Activate the shader program to start using it for rendering.
        glUseProgram(this->shaderProgram);

        // Retrieve the location of the uniform variable 'waterTexture' in the shader program.
        waterTextureLoc = glGetUniformLocation(this->shaderProgram, "waterTexture");

        // Set the texture unit 0 to be used for the 'waterTexture' uniform sampler.
        glUniform1i(waterTextureLoc, 0);

        // Activate texture unit 0 as the active texture unit.
        glActiveTexture(GL_TEXTURE0);

        // Bind the previously loaded and configured 'waterTexture' to the active texture unit.
        glBindTexture(GL_TEXTURE_2D, waterTexture);

        // Unbind the current texture from texture unit 0 for cleanup.
        glBindTexture(GL_TEXTURE_2D, 0);

        // Stop using the shader program.
        glUseProgram(0);

        // Free the memory allocated for the water texture data.
        free(data);

        // Load the displacement map texture data from a BMP file.
        loadBMP("Assets/displacement-map1.bmp", &data, &width, &height);

        // Generate a new OpenGL texture object and assign it to 'displacementTexture'.
        glGenTextures(1, &displacementTexture);

        // Bind the newly created 'displacementTexture' as the current 2D texture.
        glBindTexture(GL_TEXTURE_2D, displacementTexture);

        // Set texture wrapping mode on S axis to repeat.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // Set texture wrapping mode on T axis to repeat.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        // Set texture minification filter to linear interpolation with mipmaps.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // Set texture magnification filter to linear interpolation.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // Upload the texture data to the currently bound texture object.
        // Parameters: target, mipmap level, internal format, width, height, border, format, data type, data pointer.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
        // Automatically generate mipmaps for the currently bound texture.
        glGenerateMipmap(GL_TEXTURE_2D);

        // Unbind the current texture to prevent accidental modifications.
        glBindTexture(GL_TEXTURE_2D, 0);

        // Activate the shader program to set uniform variables.
        glUseProgram(this->shaderProgram);

        // Get the location of the 'disptex' uniform in the shader program.
        displacementTextureLoc = glGetUniformLocation(this->shaderProgram, "disptex");
        // Activate texture unit 1 for the displacement texture.
        glActiveTexture(GL_TEXTURE1);
        // Set the 'disptex' uniform to use texture unit 1.
        glUniform1i(displacementTextureLoc, 1);

        // Bind the displacement texture to texture unit 1.
        glBindTexture(GL_TEXTURE_2D, displacementTexture);
        // Unbind the texture to prevent accidental modifications.
        glBindTexture(GL_TEXTURE_2D, 0);

        // Stop using the shader program.
        glUseProgram(0);

        // Free the memory allocated for the displacement texture data.
        free(data);

    }

    void setupBuffers() {

        // Generate a vertex array object (VAO) to store the configuration.
        glGenVertexArrays(1, &VAO);
        // Generate buffer objects for vertex positions (VBO), normals (NBO), and element indices (EBO).
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &NBO);
        glGenBuffers(1, &EBO);

        // Bind the VAO to configure it.
        glBindVertexArray(VAO);

        // Position attribute setup:
        // Bind VBO as the current array buffer.
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // Copy vertex position data into VBO.
        glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float) * 3, &verts[0], GL_STATIC_DRAW);
        // Enable vertex attribute 0 (position attribute in shader).
        glEnableVertexAttribArray(0);
        // Define an array of generic vertex attribute data (position).
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Normal attribute setup:
        // Bind NBO as the current array buffer for normals.
        glBindBuffer(GL_ARRAY_BUFFER, NBO);
        // Copy normal data into NBO.
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(int) * 3, &normals[0], GL_STATIC_DRAW);
        // Enable vertex attribute 1 (normal attribute in shader).
        glEnableVertexAttribArray(1);
        // Define an array of generic vertex attribute data (normals).
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Element indices setup:
        // Bind EBO as the current element array buffer.
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // Copy index data into EBO.
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

        // Unbind VAO to avoid unintended modifications.
        glBindVertexArray(0);

    }

       // Method to initialize shader uniform locations.
    void setupUniforms() {

        // Get and store locations for tessellation related uniforms.
        outerTessID = glGetUniformLocation(shaderProgram, "outerTess");
        innerTessID = glGetUniformLocation(shaderProgram, "innerTess");

        // Get and store location for dynamic effects related uniforms.
        timeLoc = glGetUniformLocation(shaderProgram, "time");

        // Get and store locations for transformation matrix uniforms.
        modelLoc = glGetUniformLocation(shaderProgram, "model");
        viewLoc = glGetUniformLocation(shaderProgram, "view");
        projectionLoc = glGetUniformLocation(shaderProgram, "projection");

        // Get and store locations for texture manipulation uniforms.
        inverseID = glGetUniformLocation(shaderProgram, "inverse");
        texOffsetID = glGetUniformLocation(shaderProgram, "texOffset");
        texScaleID = glGetUniformLocation(shaderProgram, "texScale");

        // Get and store location for lighting and camera related uniforms.
        lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
        cameraViewPos = glGetUniformLocation(shaderProgram, "cameraView");

    }

public:
    // Vectors to store the vertex positions, normals, and indices of the mesh.
    vector<float> verts;
    vector<float> normals;
    vector<unsigned int> indices;

    // OpenGL IDs for the Vertex Array Object, Vertex Buffer Object, Element Buffer Object, and Normal Buffer Object.
    GLuint VAO, VBO, EBO, NBO;

    // Constructor for the PlaneMesh class.
    PlaneMesh(float min, float max, float stepsize, GLuint shaderProgram) {
        // Store the shader program ID passed during construction.
        this->shaderProgram = shaderProgram;

        // Generate the plane mesh geometry based on the given parameters.
        planeMeshQuads(min, max, stepsize);

        // Call setup functions to initialize OpenGL buffers, uniforms, and textures.
        setupBuffers();
        setupUniforms();
        setupTextures();
    }

    void planeMeshQuads(float min, float max, float stepsize) {
    // Initialize the starting point for the x-coordinate.
    float x = min;
    float y = 0; // The y-coordinate is constant as we're creating a flat plane.

    // First loop: Create a single row of vertices along the Z-axis starting from `min` to `max`.
    for (float z = min; z <= max; z += stepsize) {
        verts.push_back(x); // X-coordinate
        verts.push_back(y); // Y-coordinate (always 0 for a flat plane)
        verts.push_back(z); // Z-coordinate
        // Add normals pointing upwards (in the positive Y direction).
        normals.push_back(0);
        normals.push_back(1);
        normals.push_back(0);
    }

    // Second loop: Fill in the rest of the grid in the XZ plane.
    for (float x = min + stepsize; x <= max; x += stepsize) {
        for (float z = min; z <= max; z += stepsize) {
            verts.push_back(x);
            verts.push_back(y);
            verts.push_back(z);
            // Normals remain constant, pointing upwards.
            normals.push_back(0);
            normals.push_back(1);
            normals.push_back(0);
        }
    }

    // Generate indices for quad elements. Quads are defined in a clockwise manner.
    int i = 0, j = 0;
    for (float x = min; x < max; x += stepsize) {
        j = 0;
        for (float z = min; z < max; z += stepsize) {
            indices.push_back(i * ((max - min) / stepsize + 1) + j);
            indices.push_back(i * ((max - min) / stepsize + 1) + j + 1);
            indices.push_back((i + 1) * ((max - min) / stepsize + 1) + j + 1);
            indices.push_back((i + 1) * ((max - min) / stepsize + 1) + j);
            ++j;
        }
        ++i;
    }

    // Retrieve maximum allowed vertices, indices, and patches from OpenGL and output them.
    GLint max_vertices, max_indices, max_patches;
    glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &max_vertices);
    glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &max_indices);
    glGetIntegerv(GL_MAX_PATCH_VERTICES, &max_patches);
    }


	void draw(float time , mat4 P, mat4 V, mat4 inverse, vec3 lightPos, vec2 texScale, vec2 texOffset, float innerTess, float outerTess, float displacementScale) {
    // Activate and bind the water texture to texture unit 1.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, waterTexture);

    // Activate and bind the displacement texture to the same texture unit (possibly a mistake; usually, you'd use a different texture unit).
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, displacementTexture);

    // Bind the Vertex Array Object of the plane mesh and use the shader program.
    glBindVertexArray(VAO);
    glUseProgram(shaderProgram);

    // Calculate the Model-View-Projection matrix.
    mat4 MVP = (mat4(1.0f)) * V * P; // MVP matrix calculation.

    // Set shader uniforms for matrices, light position, time, texture offsets, tessellation levels, and displacement scale.
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &MVP[0][0]);
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(inverseID, 1, GL_FALSE, &inverse[0][0]);
    glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
    glUniform1f(timeLoc, time);
    glUniform2f(texOffsetID, texOffset.x, texOffset.y);
    glUniform2f(texScaleID, texScale.x, texScale.y);
    glUniform1f(outerTessID, outerTess);
    glUniform1f(innerTessID, innerTess);
    glUniform1f(displacementTextureLoc, displacementScale);

    // Bind the water texture to texture unit 0 and set the corresponding uniform.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, waterTexture);
    glUniform1i(waterTextureLoc, 0);

    // Bind the displacement texture to texture unit 1 and set the corresponding uniform.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, displacementTexture);
    glUniform1i(displacementTextureLoc, 1);

    // Set the patch parameter for tessellation.
    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // Draw the elements (quads) of the mesh.
    glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_INT, (void*)0);

    // Unbind the textures and reset the active texture unit to 0.
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE0);

    // Unbind the VAO and the shader program to clean up.
    glBindVertexArray(0);
    glUseProgram(0);
}


};
