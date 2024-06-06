#version 330 core

// Vertex shader - this code is executed for every vertex of the shape

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)

// Output variables sent to the fragment shader
out vec3 pos;
out vec3 sun_norm;

// Uniform variables expected to receive from the C++ program
uniform mat4 model;       // Model affine transform matrix associated to the current shape
uniform mat4 view;        // View matrix (rigid transform) of the camera
uniform mat4 projection;  // Projection (perspective or orthogonal) matrix of the camera
//uniform vec3 sun_pos;     // Position of the sun in world space

void main()
{
    // The position of the vertex in the world space
    vec4 position = model * vec4(vertex_position, 1.0);

    // The projected position of the vertex in the normalized device coordinates:
    vec4 position_projected = projection * view * position;

    // gl_Position is a built-in variable which is the expected output of the vertex shader
    gl_Position = position_projected; // gl_Position is the projected vertex position (in normalized device coordinates)

    // Output the transformed position to the fragment shader
    pos = vec3(position);

    // Normalize and output the sun position to the fragment shader
    sun_norm = normalize(vec3(0.0f, 1.0f, 50.0f));
}
