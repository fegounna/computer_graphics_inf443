#version 330 core

// Inputs from VBOs
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec3 vertex_normal;
layout (location = 2) in vec3 vertex_color;
layout (location = 3) in vec2 vertex_uv;
layout (location = 4) in vec3 instance_position;

// Output variables to the fragment shader
out struct fragment_data {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 uv;
} fragment;
out vec4 ioEyeSpacePosition;

// Uniform transformation matrices
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main() {
    // Position in world space
    vec4 position = model * vec4(vertex_position + instance_position, 1.0);

    // Transform normal
    mat4 modelNormal = transpose(inverse(model));
    vec4 normal = modelNormal * vec4(vertex_normal, 0.0);

    // Position in eye space
    ioEyeSpacePosition = view * position;
    // Position in clip space
    gl_Position = projection * ioEyeSpacePosition;

    // Passing to fragment shader
    fragment.position = position.xyz;
    fragment.normal = normal.xyz;
    fragment.color = vertex_color;
    fragment.uv = vertex_uv;
}
