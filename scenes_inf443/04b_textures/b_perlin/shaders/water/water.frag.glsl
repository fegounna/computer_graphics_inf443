#version 330 core


in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;

layout(location = 0) out vec4 FragColor;

uniform sampler2D image_texture;   
uniform sampler2D supp; 

void main()
{
	vec2 uv_image = vec2(fragment.uv.x, fragment.uv.y);
    FragColor = mix(texture(image_texture,uv_image),texture(image_texture,uv_image),0.5);
}
