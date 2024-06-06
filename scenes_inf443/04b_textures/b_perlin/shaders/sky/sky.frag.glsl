#version 330 core
//---------IN------------
in vec3 pos;
in vec3 sun_norm;

//---------UNIFORM------------
uniform sampler2D tint;   // the color of the sky on the half-sphere where the sun is.
uniform sampler2D tint2;  // the color of the sky on the opposite half-sphere.
uniform sampler2D clouds1; // clouds texture (spherical UV projection)
uniform sampler2D sun;    // sun texture (radius x height)

//---------OUT------------
out vec3 color;

//---------MAIN------------
void main() {
    vec3 pos_norm = normalize(pos);
    float dist = dot(sun_norm, pos_norm);

    // Determine sky color based on sun's position
    vec3 color_wo_sun = texture(tint2, vec2((sun_norm.y + 1.0) / 2.0, max(0.01, pos_norm.y))).rgb;
    vec3 color_w_sun = texture(tint, vec2((sun_norm.y + 1.0) / 2.0, max(0.01, pos_norm.y))).rgb;
    color = mix(color_wo_sun, color_w_sun, dist * 0.5 + 0.5);

    // Compute u and v for the clouds texture (spherical projection)
    float u = 0.5 + atan(pos_norm.z, pos_norm.x) / (2 * 3.14159265);
    float v = -0.5 + asin(pos_norm.y) / 3.14159265;

    // Clouds color
    vec3 cloud_color = vec3(1.0);  // Constant cloud color for simplicity

    // Reading from the clouds map
    float transparency = texture(clouds1, vec2(u, v)).r;

    /* // Sun
    float radius = length(pos_norm - sun_norm);
    if (radius < 0.05) {  // If within the sun area
        float sunTime = sun_norm.y;  
        radius = radius / 0.05;
        if (radius < 1.0 - 0.001) {
            vec4 sun_color = texture(sun, vec2(radius, sunTime));
            color = mix(color, sun_color.rgb, sun_color.a);
        }
    } */

    // Final mix with clouds
    color = mix(color, cloud_color, transparency);
}
