#include "sky.hpp"
using namespace cgp;
#define M_PI 3.14159265358979323846


mesh create_half_sphere_mesh(int latitudeDivisions, int longitudeDivisions, float radius) {
    mesh sky;

    // Pole vertex
    sky.position.push_back({0, 0, radius});
    sky.normal.push_back({0, 0, -1}); // Normal pointing inward

    // Generate vertices
    for (int i = 1; i <= latitudeDivisions; ++i) {
        float theta = M_PI * i / (2 * latitudeDivisions);
        for (int j = 0; j <= longitudeDivisions; ++j) {
            float phi = 2 * M_PI * j / longitudeDivisions;
            float x = radius * sin(theta) * cos(phi);
            float y = radius * sin(theta) * sin(phi);
            float z = radius * cos(theta);

            sky.position.push_back({x, y, z});
            sky.normal.push_back({-sin(theta) * cos(phi), -sin(theta) * sin(phi), -cos(theta)}); // Normals pointing inward
        }
    }

    int totalVertices = sky.position.size();

    // Create triangles with adjusted winding order
    for (int i = 0; i < latitudeDivisions - 1; ++i) {
        for (int j = 0; j < longitudeDivisions; ++j) {
            int first = 1 + i * (longitudeDivisions + 1) + j;
            int second = first + longitudeDivisions + 1;
            int third = first + 1;
            int fourth = second + 1;

            if (second < totalVertices && fourth < totalVertices) {
                sky.connectivity.push_back({first, third, second}); // Adjusted winding order
                sky.connectivity.push_back({second, third, fourth}); // Adjusted winding order
            }
        }
    }

    // Pole triangles with adjusted winding order
    for (int j = 1; j <= longitudeDivisions; ++j) {
        int next = (j % longitudeDivisions) + 1;
        if (next + 1 < totalVertices) {
            sky.connectivity.push_back({0, next + 1, j + 1}); // Adjusted winding order
        }
    }
    sky.fill_empty_field();
    return sky;
}
