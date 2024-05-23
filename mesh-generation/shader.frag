#version 400

// Interpolated values from the vertex shaders
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

// Output color
out vec4 color;

// Uniforms
uniform vec3 modelColor, ambientColor, specularColor;
uniform float shininess;
uniform bool enableLighting;

void main () {
    if (enableLighting) {
        // Calculate diffuse color using Lambert's law
        vec3 diffuseColor = modelColor * max(dot(Normal_cameraspace, LightDirection_cameraspace), 0.0);

        // Calculate specular color using Phong's model
        vec3 reflectionDirection = reflect(-LightDirection_cameraspace, Normal_cameraspace);
        vec3 viewDirection = normalize(EyeDirection_cameraspace);
        float specular = pow(max(dot(reflectionDirection, viewDirection), 0.0), shininess);
        vec3 specularColorFinal = specularColor * specular;

        // Combine ambient, diffuse, and specular colors
        color = vec4(ambientColor + diffuseColor + specularColorFinal, 1.0);
    }
}