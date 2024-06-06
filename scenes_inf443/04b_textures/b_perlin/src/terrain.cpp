#include "terrain.hpp"
using namespace cgp;

mesh create_terrain_mesh()
{
	int const terrain_sample = 200;
	mesh terrain = mesh_primitive_grid({-1,-1,0},{1,-1,0},{1,1,0},{-1,1,0},terrain_sample,terrain_sample);

	
	// Number of samples in each direction (assuming a square grid)
	int const N = std::sqrt(terrain.position.size());

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {
			
			// Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

			int const idx = ku*N+kv;

			float persistence = 0.5f;
			float frequency_gain = 1.5f;
			int octave = 4.f;

			float const noise = noise_perlin({u, v}, octave, persistence, frequency_gain);
			//terrain.position[idx].z = noise*50.f;
			terrain.position[idx].z =  (noise-0.7f)*0.3f;

		}
	}
	return terrain;
}
