#pragma once

#include "cgp/cgp.hpp"
#include "environment.hpp"
#include "sky.hpp"
#include <vector>
#include "terrain.hpp"
using cgp::mesh_drawable;


struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
};

// The structure of the custom scene
struct scene_structure : cgp::scene_inputs_generic {

	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //
	//camera_controller_orbit_euler camera_control;
	camera_controller_2d_displacement camera_control;
	camera_projection_perspective camera_projection;
	window_structure window;

	mesh_drawable global_frame;          // The standard global frame
	environment_structure environment;   // Standard environment controler
	input_devices inputs;                // Storage for inputs status (mouse, keyboard, window dimension)
	gui_parameters gui;                  // Standard GUI element storage
	
	// ****************************** //
	// Elements and shapes of the scene
	// ****************************** //

	cgp::mesh terrain_mesh;
	cgp::mesh_drawable terrain_drawable;
	perlin_noise_parameters parameters;
	cgp::mesh sky;
	cgp::mesh_drawable sky_drawable;
	cgp::mesh_drawable grass;
	cgp::mesh_drawable water;
	

	cgp::timer_basic timer;
	//std::vector<vec3> vec;
	numarray<vec3> vec;
	cgp::opengl_fbo_structure fbo_pass_1;
	cgp::opengl_fbo_structure fbo_pass_2;
	vec3 const background_color = { 1,1,1 };
	// ****************************** //
	// Functions
	// ****************************** //

	void initialize();    // Standard initialization to be called before the animation loop
	void display_frame(); // The frame display to be called within the animation loop
	void display_gui();   // The display of the GUI, also called within the animation loop


	void mouse_move_event();
	void mouse_click_event();
	void keyboard_event();
	void idle_frame();

	void display_info();
	
    GLuint reflectionFBO;
    GLuint reflectionTexture;
    GLuint refractionFBO;
    GLuint refractionTexture;
    GLuint reflectionDepthBuffer;
    GLuint refractionDepthBuffer;

    // Helper functions
    void setupReflectionFramebuffer();
    void setupRefractionFramebuffer();
    void display_water();
    void renderScene(const vec4& clipPlane);
    void firstDrawCall(const vec4& clipPlane);


	void clear_screen(cgp::vec3 const background_color = { 1,1,1 }) const;


};





