#version 330 core 

// Fragment shader - this code is executed for every pixel/fragment that belongs to a displayed shape
//
// Compute the color using Phong illumination (ambient, diffuse, specular) 
//  There is 3 possible input colors:
//    - fragment_data.color: the per-vertex color defined in the mesh
//    - material.color: the uniform color (constant for the whole shape)
//    - image_texture: color coming from the texture image
//  The color considered is the product of: fragment_data.color x material.color x image_texture
//  The alpha (/transparent) channel is obtained as the product of: material.alpha x image_texture.a
// 

// Inputs coming from the vertex shader
in struct fragment_data
{
    vec3 position; // position in the world space
    vec3 normal;   // normal in the world space
    vec3 color;    // current color on the fragment
    vec2 uv;       // current uv-texture on the fragment

} fragment;
in vec4 ioEyeSpacePosition;

// Output of the fragment shader - output color
layout(location=0) out vec4 FragColor;


// Uniform values that must be send from the C++ code
// ***************************************************** //

uniform sampler2D image_texture;   // Texture image identifiant
uniform sampler2D image_texture_2;
uniform sampler2D image_texture_3;



uniform mat4 view;       // View matrix (rigid transform) of the camera - to compute the camera position

uniform vec3 light; // position of the light




uniform vec4 u_clippingPlane;


// Coefficients of phong illumination model
struct phong_structure {
	float ambient;      
	float diffuse;
	float specular;
	float specular_exponent;
};

// Settings for texture display
struct texture_settings_structure {
	bool use_texture;       // Switch the use of texture on/off
	bool texture_inverse_v; // Reverse the texture in the v component (1-v)
	bool two_sided;         // Display a two-sided illuminated surface (doesn't work on Mac)
};

// Material of the mesh (using a Phong model)
struct material_structure
{
	vec3 color;  // Uniform color of the object
	float alpha; // alpha coefficient

	phong_structure phong;                       // Phong coefficients
	texture_settings_structure texture_settings; // Additional settings for the texture
}; 

uniform material_structure material;

















struct FogParameters
{
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	
	int equation;
};

float getFogFactor(FogParameters params, float fogCoordinate);

float getFogFactor(FogParameters params, float fogCoordinate)
{
	float result = 0.0;
	if(params.equation == 0)
	{
		float fogLength = params.linearEnd - params.linearStart;
		result = (params.linearEnd - fogCoordinate) / fogLength;
	}
	else if(params.equation == 1) {
		result = exp(-params.density * fogCoordinate);
	}
	else if(params.equation == 2) {
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}


























void main()
{
   // Clipping based on the plane
    float distance = dot(vec4(fragment.position, 1.0), u_clippingPlane);
    if (distance < 0.0) {
        discard;
    }
	// Compute the position of the center of the camera
	mat3 O = transpose(mat3(view));                   // get the orientation matrix
	vec3 last_col = vec3(view*vec4(0.0, 0.0, 0.0, 1.0)); // get the last column
	vec3 camera_position = -O*last_col;


	// Renormalize normal
	vec3 N = normalize(fragment.normal);

	// Inverse the normal if it is viewed from its back (two-sided surface)
	//  (note: gl_FrontFacing doesn't work on Mac)
	if (material.texture_settings.two_sided && gl_FrontFacing == false) {
		N = -N;
	}

	// Phong coefficient (diffuse, specular)
	// *************************************** //

	// Unit direction toward the light
	vec3 L = normalize(light-fragment.position);

	// Diffuse coefficient
	float diffuse_component = max(dot(N,L),0.0);

	// Specular coefficient
	float specular_component = 0.0;
	if(diffuse_component>0.0){
		vec3 R = reflect(-L,N); // reflection of light vector relative to the normal.
		vec3 V = normalize(camera_position-fragment.position);
		specular_component = pow( max(dot(R,V),0.0), material.phong.specular_exponent );
	}

	// Texture
	// *************************************** //



	// added //

	// Current uv coordinates
	vec2 uv_image = vec2(fragment.uv.x, fragment.uv.y);
	//if(material.texture_settings.texture_inverse_v) {
	//	uv_image.y = 1.0-uv_image.y;
	//}

    float fScale = fragment.position.z*2.f/7.0f;
	// Initialize the texture color
	vec4 color_image_texture = vec4(0.0);

    if (fScale < -0.05f)
        color_image_texture = texture(image_texture, uv_image);
    else if (fScale < 0.05f) {
       float blend = (fScale + 0.05) / 0.1;
		color_image_texture = (blend/2) * texture(image_texture, uv_image) + (1 - blend) * texture(image_texture_2, uv_image);
    }
	 else if (fScale < 0.1)
        color_image_texture = texture(image_texture_2, uv_image);
    else if (fScale < 0.3) {
        float blend = (fScale - 0.1) / 0.2;
        color_image_texture = (1-blend) * texture(image_texture_2, uv_image) + blend * texture(image_texture_3, uv_image);
    } else
        color_image_texture = texture(image_texture_3, uv_image);

	// end //

	
	// Compute Shading
	// *************************************** //

	// Compute the base color of the object based on: vertex color, uniform color, and texture
	vec3 color_object  = fragment.color * material.color * color_image_texture.rgb;

	// Compute the final shaded color using Phong model
	float Ka = material.phong.ambient;
	float Kd = material.phong.diffuse;
	float Ks = material.phong.specular;
	vec3 color_shading = (Ka + Kd * diffuse_component) * color_object + Ks * specular_component * vec3(1.0, 1.0, 1.0);
	
	// Output color, with the alpha component
	FragColor = vec4(color_shading, material.alpha * color_image_texture.a);


	FogParameters fogParams;

	// Fog color (could be similar to sky color or a greyish tone)
	fogParams.color = vec3(0.7, 0.7, 0.8);
	// Starting point of the fog, where it begins to appear
	fogParams.linearStart = 20.0f;

	// Ending point of the fog, where it becomes fully dense
	fogParams.linearEnd = 100.0f;

	// Density of the fog, higher values make thicker fog
	fogParams.density = 0.04f;

	// Equation type (0 = linear, 1 = exponential, 2 = exponential squared)
	fogParams.equation = 1; // Using linear for gradual fog effect

	float fogCoordinate = abs(ioEyeSpacePosition.z / ioEyeSpacePosition.w);
	FragColor = mix(FragColor, vec4(fogParams.color, 1.0), getFogFactor(fogParams, fogCoordinate));
}