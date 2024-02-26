#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"
#include "health_system.hpp"

using Clock = std::chrono::high_resolution_clock;

int window_width_px = 1500;
int window_height_px = 1000;

// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	//HealthSystem health;
	AISystem ai;

	// Initializing window
	glfwInit();
	GLFWmonitor* monitor = glfwGetPrimaryMonitor(); //get primary monitor
	const GLFWvidmode* vidmode = glfwGetVideoMode(monitor); //get video mode
	window_width_px = vidmode->width;
	window_height_px = vidmode->height;
	
#ifdef __APPLE__
	window_width_px /= 2; // I think this will deal with retina displays, but I don't think it would work if you have a regular display connected...
	window_height_px /= 2;
#endif

	GLFWwindow* window = world.create_window(window_width_px, window_height_px);
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window_width_px, window_height_px, window);
	world.init(&renderer);

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become
		// unresponsive

		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		if (!world.pause && !delay.zoomPause) {
			physics.step(elapsed_ms, window_width_px, window_height_px);
			ai.step(elapsed_ms);
			world.handle_creation();
			world.handle_texts(elapsed_ms);
		}
		if (!world.pause) {
			world.step(elapsed_ms);
			world.handle_collisions();
			world.updateTimer(elapsed_ms);
		}
		if (delay.zoomMode && delay.zoomPause) {
			//physics.zoomModePositions(elapsed_ms);
			physics.step(elapsed_ms, window_width_px, window_height_px);
		}
		world.handle_animation(elapsed_ms);
		renderer.draw();

	}
	return EXIT_SUCCESS;
}