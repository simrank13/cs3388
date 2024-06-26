#version 400

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;

// Output data; will be interpolated for each fragment.
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

// Uniforms
uniform mat4 MVP;
uniform mat4 V;
uniform vec3 LightDir;
uniform bool enableLighting;

void main() {
    // Always set gl_Position, regardless of lighting.
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1);

    // Only compute lighting-related variables if lighting is enabled.
    if (enableLighting) {
        vec3 vertexPosition_cameraspace = (V * vec4(vertexPosition_modelspace, 1)).xyz;
        EyeDirection_cameraspace = -vertexPosition_cameraspace;
        LightDirection_cameraspace = normalize(LightDir);
        Normal_cameraspace = mat3(V) * vertexNormal_modelspace;
    }
}
