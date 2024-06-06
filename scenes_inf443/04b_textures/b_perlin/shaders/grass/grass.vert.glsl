#version 330 core

// Vertex shader - this code is executed for every vertex of the shape

// Inputs coming from VBOs
layout (location = 0) in vec3 vertex_position; // vertex position in local space (x,y,z)
layout (location = 1) in vec3 vertex_normal;   // vertex normal in local space   (nx,ny,nz)
layout (location = 2) in vec3 vertex_color;    // vertex color      (r,g,b)
layout (location = 3) in vec2 vertex_uv;       // vertex uv-texture (u,v)
layout (location = 4) in vec3 instance_position;  // instance position  (x,y,z)

// Output variables sent to the fragment shader
out struct fragment_data
{
    vec3 position; // vertex position in world space
    vec3 normal;   // normal position in world space
    vec3 color;    // vertex color
    vec2 uv;       // vertex uv
} fragment;
out vec4 ioEyeSpacePosition;

// Uniform variables expected to receive from the C++ program
uniform mat4 model; // Model affine transform matrix associated to the current shape
uniform mat4 view;  // View matrix (rigid transform) of the camera
uniform mat4 projection; // Projection (perspective or orthogonal) matrix of the camera
uniform float time;



float hash(){
        return max(0.2,min(0.4,1.0 + sin(gl_InstanceID * 415.92653)));
    
}
void main()
{
    // Animation parameters
    float waveAmplitude = 0.08f; // Amplitude of the wave
    float waveFrequency = 1.0f; // Frequency of the wave
    float wavePhase = time * 2.0f; // Phase, controlled by time to animate

    // Scaling factor depending on the vertex z-position (0 at the bottom, 1 at the top)
    float heightFactor = vertex_position.z; // Assuming z ranges from 0.0 to 1.0

    // Apply the wave transformation
    float wave = waveAmplitude * heightFactor * heightFactor * sin(waveFrequency * vertex_position.z + wavePhase);
    vec3 pos = vertex_position;
    pos.z *= hash();
    pos.y *= hash();
    pos.x *= hash();
    vec3 animatedPosition = (model*vec4(pos,1.0)).xyz;
    animatedPosition.x += wave; // Apply wave effect along the x-axis

    // Transform position and normal into world space
    //vec4 worldPosition = model * vec4(animatedPosition, 1.0);

    vec4 worldPosition =  vec4(animatedPosition + instance_position, 1.0);
    vec4 worldNormal = transpose(inverse(model)) * vec4(vertex_normal, 0.0);
    ioEyeSpacePosition = view * worldPosition;




    // Fill the parameters sent to the fragment shader
    fragment.position = worldPosition.xyz;
    fragment.normal   = worldNormal.xyz;
    fragment.color    = vertex_color;
    fragment.uv       = vertex_uv;

    // gl_Position is a built-in variable which is the expected output of the vertex shader
    gl_Position = projection * ioEyeSpacePosition;


}
