/*#include "scene.hpp"


using namespace cgp;




void scene_structure::initialize()
{
	camera_control.initialize(inputs, window); // Give access to the inputs and window global state to the camera controler
	camera_control.set_rotation_axis_z();
	camera_control.look_at({ 1.0f, 1.0f, 1.0f }, {0,0,0}, {0,0,1});
	display_info();
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());
	terrain_mesh = create_terrain_mesh();
	terrain_drawable.initialize_data_on_gpu(terrain_mesh);
	terrain_drawable.model.scaling = 7;
	terrain_drawable.material.phong.specular = 0.0f; // non-specular terrain material

	terrain_drawable.shader.load("./shaders/mesh/mesh.vert.glsl", "./shaders/mesh/mesh.frag.glsl");
	// Added //
	terrain_drawable.texture.load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/360_F_281844711_sVcjGVwk1ZniFbk2IsJj0dg66wpnjsZS.jpg",GL_REPEAT,GL_REPEAT);
	// granite-texture-background.jpg
	//terrain_drawable.supplementary_texture["image_texture_2"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/inf443_lab_code/scenes_inf443/03b_modeling/sand_grass_02.jpg",GL_REPEAT,GL_REPEAT);

	terrain_drawable.supplementary_texture["image_texture_2"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/low.jpg",GL_REPEAT,GL_REPEAT);
    terrain_drawable.supplementary_texture["image_texture_3"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/a2f6ea81a80cbe48f54575613a418e53.jpg",GL_REPEAT,GL_REPEAT);

	//sky.initialize_data_on_gpu(mesh_load_file_obj("/users/eleves-a/2022/yessin.moakher/sphere.obj"));
	sky = create_half_sphere_mesh(20,40,1.0f);
	sky_drawable.initialize_data_on_gpu(sky);
	sky_drawable.model.scaling = 9;
	sky_drawable.model.translation = { 0.0f, 0.f, -3.f };
	sky_drawable.shader.load("./shaders/sky/sky.vert.glsl", "./shaders/sky/sky.frag.glsl");
	//sky_drawable.texture.load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/sky-with-clouds-texture.jpg",GL_REPEAT,GL_REPEAT);
	sky_drawable.supplementary_texture["tint"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/tint.png",GL_REPEAT,GL_REPEAT);
    sky_drawable.supplementary_texture["tint2"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/tint2.png",GL_REPEAT,GL_REPEAT);
    sky_drawable.supplementary_texture["clouds1"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/clouds1.png",GL_REPEAT,GL_REPEAT);
    sky_drawable.supplementary_texture["sun"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/sun.png",GL_REPEAT,GL_REPEAT);

	mesh quad = mesh_primitive_quadrangle({ -0.5f,0.0f,0.0f }, { 0.5f,0.0f,0.0f }, { 0.5f,0.0f,1.0f }, { -0.5f,0.0f,1.0f });
	mesh quad2 = quad; quad2.rotate({ 0,0,1 }, Pi / 2.0f);
	quad.push_back(quad2);
	grass.initialize_data_on_gpu(quad);
	grass.material.phong = { 0.9,0,0,0.9 };
	grass.texture.load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/grass.png");
	grass.shader.load("./shaders/grass/grass.vert.glsl", "./shaders/grass/grass.frag.glsl");
	float persistence = 0.5f;
	float frequency_gain = 1.5f;
	int octave = 4;
	//float u = 0.3f;
	//float v = 0.2f;
	//int const N = std::sqrt(terrain_mesh.position.size()); // Get the size of the terrain
	//float x = (u - 0.5f) * 2; // Convert parametric u to x in world coordinates [-1,1]
	//float y = (v - 0.5f) * 2; // Convert parametric v to y in world coordinates [-1,1]
	//float const noise_height = noise_perlin({u, v}, octave, persistence, frequency_gain);
 	//grass.model.scaling = 0.3f;
	//grass.model.translation = vec3(x*7.f, y*7.f, (noise_height-0.7f)*0.3f*7.0f); // Raise slightly above the terrain

	int nb = 20000;
	for (int i = 0; i < nb; i++) {
			float u = rand_uniform(0, 1);
			float v = rand_uniform(0, 1);
			//int const N = std::sqrt(terrain_mesh.position.size()); // Get the size of the terrain
			float x = (u - 0.5f) * 2; // Convert parametric u to x in world coordinates [-1,1]
			float y = (v - 0.5f) * 2; // Convert parametric v to y in world coordinates [-1,1]
			float const noise_height = noise_perlin({u, v}, octave, persistence, frequency_gain);
			float z = (noise_height-0.7f)*0.3f;

			vec3 new_pos = {x*7.0f, y*7.0f, z*7.0f};
			
			if (z>0.f && z<0.1f) {
				vec.push_back(new_pos);
			}
	}
	grass.initialize_supplementary_data_on_gpu(vec, /*location*/ //4, /*divisor: 1=per instance, 0=per vertex*/ 
    /*1);

	water.initialize_data_on_gpu(mesh_primitive_quadrangle({ -7,-7,0 }, { 7,-7,0 }, { 7,7,0 }, { -7,7,0 }));
	water.shader.load("./shaders/water/water.vert.glsl", "./shaders/water/water.frag.glsl");

	fbos.initialize(1920, 1080);

}
*/
/*

void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();

	 
	
	if (gui.display_frame)
		draw(global_frame, environment);
	
	fbos.bindReflectionFrameBuffer();
	fbos.bindRefractionFrameBuffer();
	fbos.unbindCurrentFrameBuffer();

	timer.update();

	draw(terrain_drawable, environment);
	draw(sky_drawable, environment);
	draw(water, environment);
	uniform_generic_structure additional_uniforms = uniform_generic_structure();
	additional_uniforms.uniform_float["time"]=timer.t;

	/*
	for (auto& position : vec){
		grass.model.translation = position;
		draw(grass, environment, 1,true, additional_uniforms ,GL_TRIANGLES);
		if (gui.display_wireframe){
			draw_wireframe(grass, environment, {0,0,1}, 1, true, additional_uniforms);
		}
		//grass.model.translation = -position;
	}*/
	
	/*draw(grass, environment, vec.size(),true, additional_uniforms ,GL_TRIANGLES);
	if (gui.display_wireframe){
		draw_wireframe(terrain_drawable, environment);
		draw_wireframe(sky_drawable, environment);
		//draw_wireframe(grass, environment, {0,0,1}, vec.size(), true, additional_uniforms);
		
	}
	
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);

}

void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move(environment.camera_view);
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click(environment.camera_view);
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard(environment.camera_view);
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame(environment.camera_view);
}

void scene_structure::display_info()
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
} */