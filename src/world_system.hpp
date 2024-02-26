#pragma once

// internal
#include "common.hpp"

// stlib
#include <vector>
#include <random>


#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_mixer.h>

#include "render_system.hpp"
#include "health_system.hpp"

// Container for all our entities and game logic. Individual rendering / update is
// deferred to the relative update() methods
class WorldSystem
{
public:
	WorldSystem();

	// Creates a window
	GLFWwindow *create_window(int width, int height);

	// starts the game
	void init(RenderSystem *renderer);

	// Releases all associated resources
	~WorldSystem();

	// Steps the game ahead by ms milliseconds
	bool step(float elapsed_ms);

	void updateTimer(float elapsed_ms_since_last_update);

	// Check for collisions
	void handle_collisions();

	void handle_creation();

	void handle_animation(float elapsed_ms_since_last_update);

	void handle_texts(float elapsed_ms);

	// Should the game be over ?
	bool is_over() const;

	void setCamera(Entity cam) { camera = cam; };

    void save();

	bool pause = false;
	int level_index = 0;

	// delay
	bool zoomPause = false;
	bool zoomMode = false;


private:
	// Input callback functions
	void on_key(int key, int, int action, int mod);
	void on_mouse_move(vec2 pos);
	void on_mouse_button(int button, int action, int mod);

	void cursor_movement();
	void cursor_render();
	void player_healthbar_motion_update();
	// Camera/Player edge detection
	void checkPlayerOnEdge(int screen_width, float elapsed_ms);
	void camera_movement();
	// void instaKill(Entity entity);
	void player_movement(int key, int action, int mod);
	void boss_spawn();
	
	void loadBestScores();
	void saveBestScores();
	void addScore(int i);
	std::string formatScore(int score);
	// restart level
	void restart_game();
	void CalculatingTheRequiredVelocityVector(Motion& motion,vec2 TargetPos);


	// OpenGL window handle
	GLFWwindow *window;

	// points and scoring
	unsigned int points;
	Entity scoreCounter;
	std::vector<unsigned int> bestScores;
	std::vector<std::string> bestTimes;


	// Help
	Entity help;
	Entity helpDoc;
    Entity gameDrop;
	bool helpOn = false;

	

	// tutorial
    Entity tutorial_instructionsDisplay;

	//prompt
	Entity prompts;

	// level2 instructions
	Entity level2Instructions;
	bool displayingLevel2Instructions = false;
	bool snailDetails = false;
	bool acidDetails = false;
	bool displayOnce = true;

	// levels
	Entity levelDisplay;

	// Game state
	RenderSystem *renderer;
	float current_speed;
	float timer;
	float next_bee_spawn;
	float next_snail_spawn;
	float next_ant_spawn;
	float acid_buildup_factor;

	Entity player_spider;
	Entity spider_mesh;
	Entity camera;
	Entity cursor;
	Entity acid_mode_text;
	vec2 mouse_window_position = {0, 0};
	vec2 cursor_position = {0, 0};
	bool bee_spawn = true;
	bool ant_spawn = true;
	bool snail_spawn = true;
	bool has_boss = false;
	bool nextLevel = false;
	bool gameFinished = false;
	bool mode_big_acid = false;
	bool mouse_hold = false;
	bool mouse_was_held = false;
	// Adding the health bar here
	Entity health_container;
	Entity health_fill;

	float Elapsed_collisionTimer;
	std::vector<std::string> levels = {
		"level_0.lvl",
		"level_1.lvl",
		"level_2.lvl"
	};


	// music references
	Mix_Music *background_music;
	Mix_Chunk *spider_eat_sound;
	Mix_Chunk *stinger_impact;
	Mix_Chunk *bee_hit;

	// C++ random number generator
	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist; // number between 0..1
};
