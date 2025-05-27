#version 450

// camera matrix
layout(set = 0, binding = 0) uniform Camera {
    mat4 view_proj;
} camera;

// vertex buffer input
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

//output to fragment buffer
layout (location = 0) out vec4 fragColor;

void main(){
    gl_Position = camera.view_proj * vec4(inPosition, 1.0);
    fragColor = vec4(inColor, 1.0f);
}