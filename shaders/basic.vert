#version 450

// vertex buffer input
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 inColor;

//output to fragment buffer
layout (location = 0) out vec4 fragColor;

void main(){
    gl_Position = vec4(inPosition, 1.0);
    fragColor = vec4(inColor, 1.0f);
}