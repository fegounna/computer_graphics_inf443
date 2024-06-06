#include "scene.hpp"


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
	terrain_drawable.texture.load_and_initialize_texture_2d_on_gpu("./360_F_281844711_sVcjGVwk1ZniFbk2IsJj0dg66wpnjsZS.jpg",GL_REPEAT,GL_REPEAT);
	// granite-texture-background.jpg
	//terrain_drawable.supplementary_texture["image_texture_2"].load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/inf443_lab_code/scenes_inf443/03b_modeling/sand_grass_02.jpg",GL_REPEAT,GL_REPEAT);

	terrain_drawable.supplementary_texture["image_texture_2"].load_and_initialize_texture_2d_on_gpu("./low.jpg",GL_REPEAT,GL_REPEAT);
    terrain_drawable.supplementary_texture["image_texture_3"].load_and_initialize_texture_2d_on_gpu("./a2f6ea81a80cbe48f54575613a418e53.jpg",GL_REPEAT,GL_REPEAT);

	//sky.initialize_data_on_gpu(mesh_load_file_obj("/users/eleves-a/2022/yessin.moakher/sphere.obj"));
	sky = create_half_sphere_mesh(20,40,1.0f);
	sky_drawable.initialize_data_on_gpu(sky);
	sky_drawable.model.scaling = 9;
	sky_drawable.model.translation = { 0.0f, 0.f, -3.f };
	sky_drawable.shader.load("./shaders/sky/sky.vert.glsl", "./shaders/sky/sky.frag.glsl");
	//sky_drawable.texture.load_and_initialize_texture_2d_on_gpu("/users/eleves-a/2022/yessin.moakher/sky-with-clouds-texture.jpg",GL_REPEAT,GL_REPEAT);
	sky_drawable.supplementary_texture["tint"].load_and_initialize_texture_2d_on_gpu("./tint.png",GL_REPEAT,GL_REPEAT);
    sky_drawable.supplementary_texture["tint2"].load_and_initialize_texture_2d_on_gpu("./tint2.png",GL_REPEAT,GL_REPEAT);
    sky_drawable.supplementary_texture["clouds1"].load_and_initialize_texture_2d_on_gpu("./clouds1.png",GL_REPEAT,GL_REPEAT);
    sky_drawable.supplementary_texture["sun"].load_and_initialize_texture_2d_on_gpu("./sun.png",GL_REPEAT,GL_REPEAT);

	mesh quad = mesh_primitive_quadrangle({ -0.5f,0.0f,0.0f }, { 0.5f,0.0f,0.0f }, { 0.5f,0.0f,1.0f }, { -0.5f,0.0f,1.0f });
	mesh quad2 = quad; quad2.rotate({ 0,0,1 }, Pi / 2.0f);
	quad.push_back(quad2);
	grass.initialize_data_on_gpu(quad);
	grass.material.phong = { 0.9,0,0,0.9 };
	grass.texture.load_and_initialize_texture_2d_on_gpu("./grass.png");
	grass.shader.load("./shaders/grass/grass.vert.glsl", "./shaders/grass/grass.frag.glsl");
	float persistence = 0.5f;
	float frequency_gain = 1.5f;
	int octave = 4;


	int nb = 20000;
	for (int i = 0; i < nb; i++) {
			float u = rand_uniform(0, 1);
			float v = rand_uniform(0, 1);
			float x = (u - 0.5f) * 2; // Convert parametric u to x in world coordinates [-1,1]
			float y = (v - 0.5f) * 2; // Convert parametric v to y in world coordinates [-1,1]
			float const noise_height = noise_perlin({u, v}, octave, persistence, frequency_gain);
			float z = (noise_height-0.7f)*0.3f;

			vec3 new_pos = {x*7.0f, y*7.0f, z*7.0f};
			
			if (z>0.f && z<0.1f) {
				vec.push_back(new_pos);
			}
	}
	grass.initialize_supplementary_data_on_gpu(vec, /*location*/ 4, /*divisor: 1=per instance, 0=per vertex*/ 1);

	water.initialize_data_on_gpu(mesh_primitive_quadrangle({ -7,-7,0 }, { 7,-7,0 }, { 7,7,0 }, { -7,7,0 }));
	water.shader.load("./shaders/water/water.vert.glsl", "./shaders/water/water.frag.glsl");

	fbo_pass_1.initialize();
	fbo_pass_2.initialize();

}


void scene_structure::clear_screen(vec3 const background_color) const
{
	glViewport(0, 0, fbo_pass_1.width, fbo_pass_1.height);
	glClearColor(background_color.x, background_color.y, background_color.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
}


void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
	environment.light = camera_control.camera_model.position();
	uniform_generic_structure additional_uniforms_terrain = uniform_generic_structure();
	uniform_generic_structure additional_uniforms = uniform_generic_structure();

	
	fbo_pass_1.update_screen_size(window.width, window.height);
	water.texture = fbo_pass_1.texture; 
	fbo_pass_1.bind();
	clear_screen(background_color);
	float distance = camera_control.camera_model.position_camera.z;
	camera_control.camera_model.position_camera.z -= distance *2;
	auto & c = camera_control.camera_model;
	//c.position_camera.y=-c.position_camera.y;
	//c.position_camera.x=-c.position_camera.x;
	float r = c.roll, p = c.pitch , y = c.yaw;
	camera_control.camera_model.manipulator_rotate_roll_pitch_yaw(0,-2*p,0);
	//camera_control.camera_model.pitch = -camera_control.camera_model.pitch;
	idle_frame();
	
	additional_uniforms_terrain.uniform_vec4["u_clippingPlane"]=vec4(0,0,1,0);
	//draw(terrain_drawable, environment,1,true, additional_uniforms_terrain ,GL_TRIANGLES);
	draw(sky_drawable, environment);
	additional_uniforms.uniform_float["time"]=timer.t;
	//draw(grass, environment,vec.size(),true, additional_uniforms ,GL_TRIANGLES);
	fbo_pass_1.unbind();
	clear_screen(background_color);

	camera_control.camera_model.position_camera.z += distance *2;
	camera_control.camera_model.manipulator_rotate_roll_pitch_yaw(0,2*p,0);
	//c.position_camera.y=-c.position_camera.y;
	//c.position_camera.x=-c.position_camera.x;
	idle_frame();
	//camera_control.camera_model.pitch = -camera_control.camera_model.pitch;
	
	/*
	fbo_pass_2.update_screen_size(window.width, window.height);
	//water.supplementary_texture["supp"] = fbo_pass_2.texture;
	water.texture = fbo_pass_2.texture;
	fbo_pass_2.bind();
	clear_screen(background_color);
	additional_uniforms_terrain.uniform_vec4["u_clippingPlane"]=vec4(0,0,-1,0);
	draw(terrain_drawable, environment,1,true, additional_uniforms_terrain ,GL_TRIANGLES);
	fbo_pass_2.unbind();
	clear_screen(background_color);*/
	
	
	
	glDisable(GL_DEPTH_TEST);
	draw(water, environment, false);
	glEnable(GL_DEPTH_TEST);
	additional_uniforms_terrain.uniform_vec4["u_clippingPlane"]=vec4(0,0,1,0);
	draw(terrain_drawable, environment,1,true, additional_uniforms_terrain ,GL_TRIANGLES);
	draw(sky_drawable, environment);
	//uniform_generic_structure additional_uniforms = uniform_generic_structure();
	//additional_uniforms.uniform_float["time"]=timer.t;
	draw(grass, environment,vec.size(),true, additional_uniforms ,GL_TRIANGLES);
	if (gui.display_frame)
		draw(global_frame, environment);
	
	timer.update();




	//draw(sky_drawable, environment);
	//draw(water, environment);
	
	


	
	//draw(grass, environment, vec.size(),true, additional_uniforms ,GL_TRIANGLES);
	/*if (gui.display_wireframe){
		draw_wireframe(terrain_drawable, environment);
		draw_wireframe(sky_drawable, environment);
		//draw_wireframe(grass, environment, {0,0,1}, vec.size(), true, additional_uniforms);
		
	}*/
	
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
}

