// Header
#include "world_system.hpp"
#include "world_init.hpp"
#include "health_system.hpp"
#include "level_builder.hpp"

// stlib
#include <cassert>
#include <sstream>

#include "physics_system.hpp"
#include "load.hpp"
#include <iostream>
#include <math_helpers.hpp>
#include <spider_actions.hpp>
#include <ai_swarm.hpp>

// Game configuration
const size_t MAX_BEES = 15;
const size_t MAX_ANT = 5;
size_t MAX_SNAIL = 5;
const size_t BEES_DELAY_MS = 2000 * 3;
const size_t SNAIL_DELAY_MS = 3000 * 6;
const size_t ANT_DELAY_MS = 15000;
const int screen_width = 1200;
const int screen_height = 800;

//score values
const int STINGER_SCORE = 50;
const int HIT_SCORE = 25;
const int BEE_SCORE = 100;
const int TREE_SCORE = 250;
const int BOSS_SCORE = 500;

HealthSystem health;
Load load;


// Create the fish world
WorldSystem::WorldSystem()
	: points(0), next_bee_spawn(0.f), next_ant_spawn(0.f), next_snail_spawn(0.f)
{
	// Seeding rng with random device
	rng = std::default_random_engine(std::random_device()());
}

WorldSystem::~WorldSystem()
{
	// Destroy music components
	if (background_music != nullptr)
		Mix_FreeMusic(background_music);
	if (spider_eat_sound != nullptr)
		Mix_FreeChunk(spider_eat_sound);
	if (stinger_impact != nullptr)
		Mix_FreeChunk(stinger_impact);
	if (bee_hit != nullptr)
		Mix_FreeChunk(bee_hit);
	Mix_CloseAudio();

	// Destroy all created components
	registry.clear_all_components();

	// Close the window
	glfwDestroyWindow(window);
}

// Debugging
namespace
{
	void glfw_err_cb(int error, const char *desc)
	{
		fprintf(stderr, "%d: %s", error, desc);
	}
}

// World initialization
// Note, this has a lot of OpenGL specific things, could be moved to the renderer
GLFWwindow *WorldSystem::create_window(int width, int height)
{
	///////////////////////////////////////
	// Initialize GLFW
	glfwSetErrorCallback(glfw_err_cb);
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW");
		return nullptr;
	}

	//-------------------------------------------------------------------------
	// If you are on Linux or Windows, you can change these 2 numbers to 4 and 3 and
	// enable the glDebugMessageCallback to have OpenGL catch your mistakes for you.
	// GLFW / OGL Initialization
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#if __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, 0);
	//glfwWindowHint(GLFW_DECORATED, GL_FALSE);
	glfwSwapInterval(1); // vsync

	// Create the main window (for rendering, keyboard, and mouse input)
	window = glfwCreateWindow(width, height, "Itsy Bitsy", nullptr, nullptr);
	if (window == nullptr)
	{
		fprintf(stderr, "Failed to glfwCreateWindow");
		return nullptr;
	}
	

	// Setting callbacks to member functions (that's why the redirect is needed)
	// Input is handled using GLFW, for more info see
	// http://www.glfw.org/docs/latest/input_guide.html
	glfwSetWindowUserPointer(window, this);
	auto key_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2, int _3)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_key(_0, _1, _2, _3); };
	auto cursor_pos_redirect = [](GLFWwindow *wnd, double _0, double _1)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_move({_0, _1}); };
	auto mouse_button_redirect = [](GLFWwindow *wnd, int _0, int _1, int _2)
	{ ((WorldSystem *)glfwGetWindowUserPointer(wnd))->on_mouse_button(_0, _1, _2); };
	glfwSetKeyCallback(window, key_redirect);
	glfwSetCursorPosCallback(window, cursor_pos_redirect);
	glfwSetMouseButtonCallback(window, mouse_button_redirect);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	//////////////////////////////////////
	// Loading music and sounds with SDL
	if (SDL_Init(SDL_INIT_AUDIO) < 0)
	{
		fprintf(stderr, "Failed to initialize SDL Audio");
		return nullptr;
	}
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) == -1)
	{
		fprintf(stderr, "Failed to open audio device");
		return nullptr;
	}

	background_music = Mix_LoadMUS(audio_path("background_music.wav").c_str());
	spider_eat_sound = Mix_LoadWAV(audio_path("spider_eating.wav").c_str());
	stinger_impact = Mix_LoadWAV(audio_path("bee_stinger_impact.wav").c_str());
	bee_hit = Mix_LoadWAV(audio_path("bee_hit.wav").c_str());

	if (background_music == nullptr || spider_eat_sound == nullptr || stinger_impact == nullptr || bee_hit == nullptr)
	{
		fprintf(stderr, "Failed to load sounds\n %s\n %s\n %s\n %s\n  make sure the data directory is present",
				audio_path("background_music.wav").c_str(),
				audio_path("spider_eating.wav").c_str(),
				audio_path("bee_stinger_impact.wav").c_str(),
				audio_path("bee_hit.wav").c_str());
		return nullptr;
	}

	return window;
}

void WorldSystem::init(RenderSystem *renderer_arg)
{
	this->renderer = renderer_arg;
	// Playing background music indefinitely
	Mix_PlayMusic(background_music, -1);

	// Mix_PlayMusic(bees_flying, -1);
	fprintf(stderr, "Loaded music\n");

	loadBestScores();

	// Set all states to default
	restart_game();
}

void WorldSystem::loadBestScores() {
	std::ifstream inFile(data_path() + "/bestScores");
	std::stringstream inBuffer;
	inBuffer << inFile.rdbuf();
	// string delimiting taken from https://stackoverflow.com/questions/14265581/parse-split-a-string-in-c-using-string-delimiter-standard-c

	std::string s = inBuffer.str();
	std::string delimiter = ";";

	size_t pos = 0;
	size_t pos_comma = 0;
	std::string token;
	while ((pos = s.find(delimiter)) != std::string::npos) {
		token = s.substr(0, pos);
		pos_comma = s.find(",");
		bestScores.push_back(std::stoi(token.substr(0, pos_comma)));
		bestTimes.push_back(token.substr(pos_comma+1, std::string::npos));
		s.erase(0, pos + delimiter.length());
	}

	inFile.close();
}

void WorldSystem::saveBestScores() {
	bool changed = false;
	if (points > bestScores[level_index]) {
		changed = true;
		bestScores[level_index] = points;
	}
	float timer = registry.stopwatches.components[0].counter; //only get the first counter
	char* str = (char*)malloc(6); //size 6 because null terminator
	assert(str != 0); //make sure this has been properly allocated to suppress warnings
	sprintf(str, "%02d:%02d", (int)(timer / 60000.f), ((int)(timer / 1000.f)) % 60); //ugly as sin but it works lol
	//std::cout << "\rTime: " << str;
	std::string time(str);
	if (bestTimes[level_index] == "00:00" || time < bestTimes[level_index]) {
		changed = true;
		bestTimes[level_index] = time;
	}
	free(str);

	if (changed) {
		std::ofstream outFile(data_path() + "/bestScores");
		std::stringstream outBuffer;
		assert(bestScores.size() == bestTimes.size());
		for (int i = 0; i < bestScores.size(); i++) {
			outBuffer << bestScores[i] << "," << bestTimes[i] << ";";
		}
		outFile << outBuffer.rdbuf();
	}
}

void WorldSystem::addScore(int i) {
	//update value
	points += i;
	//update counter if there is one:
	if (level_index == 0)
		return;

	auto & counter = registry.textElements.get(scoreCounter);
	counter.content = formatScore(points);
}

//format an int into a string for display (6 digits with leading 0s).
std::string WorldSystem::formatScore(int score) {
	char* str = (char*)malloc(7); //size 7 because of null terminator;
	assert(str != 0);
	sprintf(str, "%06d", score);
	std::string ret(str);
	free(str);
	return ret;
}

// Update our game world
bool WorldSystem::step(float elapsed_ms_since_last_update)
{
	Elapsed_collisionTimer = elapsed_ms_since_last_update;

	//timer += elapsed_ms_since_last_update;

	//updateTimer(elapsed_ms_since_last_update);


	// Get the screen dimensions
	int screen_width, screen_height;
	// glfwGetFramebufferSize(window, &screen_width, &screen_height);
	renderer->getWindowSizeWorld(&screen_width, &screen_height);

	// Updating window title with points
	std::stringstream title_ss;
	title_ss << "Itsy Bitsy";
	glfwSetWindowTitle(window, title_ss.str().c_str());

	// Remove debug info from the last step
	while (registry.debugComponents.entities.size() > 0)
		registry.remove_all_components_of(registry.debugComponents.entities.back());

	// Removing out of screen entities
	auto &motions_registry = registry.motions;

	// Remove entities that leave the screen on the left side
	// Iterate backwards to be able to remove without unterfering with the next object to visit
	// (the containers exchange the last element with the current)
	for (int i = (int)motions_registry.components.size() - 1; i >= 0; --i)
	{
		Entity e = motions_registry.entities[i];
		if (registry.players.has(e))
			continue;
		Motion &motion = motions_registry.components[i];
		if (motion.position.x + abs(motion.scale.x) < 0.f)
		{
			if (!registry.cameras.has(motions_registry.entities[i]) &&
				!registry.particles.has(motions_registry.entities[i]))
				registry.remove_all_components_of(motions_registry.entities[i]);
		}
	}

	// Spawning new bees
	next_bee_spawn -= elapsed_ms_since_last_update * current_speed;
	if (level_index != 0 && !displayingLevel2Instructions) {
		if (bee_spawn && registry.enemy.components.size() <= MAX_BEES && next_bee_spawn < 0.f)
		{
			// Reset timer
			next_bee_spawn = (BEES_DELAY_MS / 2) + uniform_dist(rng) * (BEES_DELAY_MS / 2);
			Camera& cam = registry.cameras.get(camera);
			vec2 position = vec2(cam.expect_pos.x + screen_width + 100.f, -(uniform_dist(rng) * screen_height / 2) + screen_height / 1.5);


			Entity bee = createBee(renderer, position);
			// Setting random initial position and constant velocity
			Motion& bee_motion = registry.motions.get(bee);
			bee_motion.position = vec2(cam.expect_pos.x + screen_width + 100.f, -(uniform_dist(rng) * screen_height / 2) + screen_height / 1.5);
			bee_motion.velocity = vec2(-100.f, 0.f);

	}
		next_snail_spawn -= elapsed_ms_since_last_update * current_speed;
		if (level_index > 1) {
			if (snail_spawn && registry.indestructibleEnemy.components.size() <= MAX_SNAIL && next_snail_spawn < 0.f) {
				// Reset timer
				next_snail_spawn = (SNAIL_DELAY_MS / 2) + uniform_dist(rng) * (SNAIL_DELAY_MS / 2);

				Entity snail = createSnail(renderer, { 0, 0 });


				// Setting random initial position and constant velocity
				Motion& snail_motion = registry.motions.get(snail);
				Camera& cam = registry.cameras.get(camera);

				snail_motion.position = vec2(cam.expect_pos.x + screen_width + 100.f, 630.f);
				snail_motion.velocity = vec2(-50.f, 0.f);
			}
	}

		// Spawning new ant
		next_ant_spawn -= elapsed_ms_since_last_update * current_speed;
		if (ant_spawn && registry.friendly.components.size() <= MAX_ANT && next_ant_spawn < 0.f)
		{
			// Reset timer
			next_ant_spawn = (ANT_DELAY_MS / 2) + uniform_dist(rng) * (ANT_DELAY_MS / 2);
			Entity entity;

			float randomAntChooser = uniform_dist(rng); // TODO: will need to change this as we add more types of ants
			// printf("random: %u\n", randomAntChooser);
			if (randomAntChooser < 0.2)
			{
				entity = createRegularAnt(renderer, { 0, 0 });
			}
			else if (randomAntChooser < 0.4)
			{
				entity = createFastAnt(renderer, { 0, 0 });
			}
			else if (randomAntChooser < 0.6)
			{
				entity = createXSAnt(renderer, { 0, 0 });
			}
			else if (randomAntChooser < 0.75)
			{
				entity = createImmuneAnt(renderer, { 0, 0 });
			}
			else if (randomAntChooser < 0.9)
			{
				entity = createXLAnt(renderer, { 0, 0 });
			}
			else
			{
				entity = createRestoreHealthAnt(renderer, { 0, 0 });
			}

			// Setting random initial position and constant velocity
			Motion& motion = registry.motions.get(entity);
			Camera& cam = registry.cameras.get(camera);

			motion.position = vec2(cam.expect_pos.x + screen_width + 100.f, 650.f);
			motion.velocity = vec2(-200.f, 0.f);
		}
	}
	else { // in tutorial mode
		if (level_index == 0) {
			if (tutorial.bees) {
				if (bee_spawn && registry.enemy.components.size() <= MAX_BEES && next_bee_spawn < 0.f) {
					// Reset timer
					next_bee_spawn = (BEES_DELAY_MS / 2) + uniform_dist(rng) * (BEES_DELAY_MS / 2);
					
					Camera& cam = registry.cameras.get(camera);
					vec2 position = vec2(cam.expect_pos.x + screen_width + 100.f, (rand() % 200) + (int)SPIDER_DEFAULT_Y - 300);

					Entity bee = createBee(renderer, position);

					// Setting random initial position and constant velocity
					Motion& bee_motion = registry.motions.get(bee);
					
					bee_motion.velocity = vec2(-100.f, 0.f);
				}
			}
			else {
				// remove all bees from scene
				for (uint i = 0; i < registry.enemy.entities.size(); i++) {
					Entity enemy = registry.enemy.entities[i];
					registry.remove_all_components_of(enemy);
				}
			}
			if (tutorial.ants) {
				next_ant_spawn -= elapsed_ms_since_last_update * current_speed;
				if (ant_spawn && registry.friendly.components.size() <= MAX_ANT && next_ant_spawn < 0.f) {
					// Reset timer
					size_t ANT_DELAY_MS_TUTORIAL = 4000 * 3;
					next_ant_spawn = (ANT_DELAY_MS_TUTORIAL / 2) + uniform_dist(rng) * (ANT_DELAY_MS_TUTORIAL / 2);
					Entity entity;

					int randomAntChooser = rand() % 6; 
					if (randomAntChooser == 0)
					{
						entity = createRegularAnt(renderer, { 0, 0 });
					}
					else if (randomAntChooser == 1)
					{
						entity = createXLAnt(renderer, { 0, 0 });
					}
					else if (randomAntChooser == 2)
					{
						entity = createXSAnt(renderer, { 0, 0 });
					}
					else if (randomAntChooser == 3)
					{
						entity = createImmuneAnt(renderer, { 0, 0 });
					}
					else if (randomAntChooser == 4)
					{
						entity = createRestoreHealthAnt(renderer, { 0, 0 });
					}
					else if (randomAntChooser == 5)
					{
						entity = createFastAnt(renderer, { 0, 0 });
					}

					// Setting random initial position and constant velocity
					Motion& motion = registry.motions.get(entity);
					Camera& cam = registry.cameras.get(camera);

					motion.position = vec2(cam.expect_pos.x + screen_width + 100.f, SPIDER_DEFAULT_Y);
					motion.velocity = vec2(-200.f, 0.f);
				}

			}
			else {
				// remove all ants from scene
				for (uint i = 0; i < registry.friendly.entities.size(); i++) {
					Entity ants = registry.friendly.entities[i];
					registry.remove_all_components_of(ants);
				}
			}
		}
	}

	assert(registry.screenStates.components.size() <= 1);
	ScreenState &screen = registry.screenStates.components[0];

	// DEATH TIMER
	float min_counter_ms = 3000.f;
	for (Entity entity : registry.deathTimers.entities)
	{
		// progress timer
		DeathTimer &counter = registry.deathTimers.get(entity);
		counter.counter_ms -= elapsed_ms_since_last_update;
		if (counter.counter_ms < min_counter_ms)
		{
			min_counter_ms = counter.counter_ms;
		}

		// restart the game once the death timer expired
		if (counter.counter_ms < 0)
		{
			registry.deathTimers.remove(entity);
			screen.darken_screen_factor = 0;
			restart_game();
			return true;
		}
	}

	// LEVEL DISPLAY
	float level_counter = 3000.f;
	for (Entity entity : registry.levels.entities)
	{
		// progress timer
		Levels& counter = registry.levels.get(entity);
		counter.timer -= elapsed_ms_since_last_update;
		if (counter.timer < level_counter)
		{
			level_counter = counter.timer;
		}

		// restart the game once the death timer expired
		if (counter.timer < 0)
		{
			registry.remove_all_components_of(entity);
		}
	}

	// SPIDER EFFECT TIMER
	float spider_counter = 9000.f;
	for (Entity entity : registry.spiderEffect.entities)
	{
			// progress timer
			SpiderEffectTimer& counter = registry.spiderEffect.get(entity);
			counter.timer -= elapsed_ms_since_last_update;
			if (counter.timer < spider_counter)
			{
				spider_counter = counter.timer;
			}

			// restart the game once the death timer expired
			if (counter.timer < 0)
			{
				registry.spiderEffect.remove(entity);
				Motion& spider = registry.motions.get(entity);
				spider.scale = { 150, 100 };
				PlayerStatus& player_status = registry.playerStatus.get(entity);
				player_status.movespeed = 200;

				if (registry.immuneSpider.has(entity))
				{
					registry.immuneSpider.remove(entity);
				}
				if (registry.fastSpider.has(entity))
				{
					registry.fastSpider.remove(entity);
				}
				if (registry.xlSpider.has(entity)) {
					registry.xlSpider.remove(entity);
				}
				return true;
			}
	}
	auto &emitters = registry.emitters;
	auto &particles = registry.particles;
	std::vector<Entity> remove = {};
	for (int i = 0; i < emitters.size(); i++)
	{
		auto &emitter = emitters.components[i];
		emitter.lifetime -= elapsed_ms_since_last_update;
		if (emitter.lifetime < 0)
		{
			Entity entity = emitters.entities[i];
			for (Entity e : emitter.particles)
			{
				remove.push_back(e);
				// registry.remove_all_components_of(e);
			}
			remove.push_back(entity);
			// registry.remove_all_components_of(entity);
		}
	}
	for (Entity e : remove)
	{ // this is necessary to avoid altering an array as we iterate through it
		registry.remove_all_components_of(e);
	}
	for (int i = 0; i < particles.size(); i++)
	{
		particles.components[i].lifetime -= elapsed_ms_since_last_update;
		if (particles.components[i].lifetime < 0)
		{
			particles.components[i].enabled = false;
		}
	}

	if (mode_big_acid) {
		// TRYING OUT SOME STUFF HERE 
		if (mouse_hold) {
			// this limit corresponds to the maxFactor set in worldInit::createBigWebShoot
			// this was late night coding, prolly fix this when i'm not too tired
			float max_factor = 7;
			if (acid_buildup_factor < max_factor) {
				acid_buildup_factor += 0.05;
				std::cout << "acid buildup factor" << acid_buildup_factor << std::endl;
			}
			
		}
		if (!mouse_hold && mouse_was_held) {
			spider_shooting_big_acid(renderer, cursor_position, acid_buildup_factor);
			mouse_was_held = false;
		}
	}
	
	// Initializing the ACID_MODE_TEXT 
	if (level_index > 1) {
		Camera& cam = registry.cameras.get(camera);
		if (mode_big_acid) {
			registry.remove_all_components_of(acid_mode_text);
			acid_mode_text = createTEXT(TEXT_ASSETS::ACID_CHARGED, vec2(100 + cam.expect_pos.x, screen_height - 20), 0, -1);
		}
		else if (!mode_big_acid) {
			registry.remove_all_components_of(acid_mode_text);
			acid_mode_text = createTEXT(TEXT_ASSETS::ACID_DEFAULT, vec2(100 + cam.expect_pos.x, screen_height - 20), 0, -1);
		}
	}
	screen.darken_screen_factor = 1 - min_counter_ms / 3000;
	checkPlayerOnEdge(screen_width, elapsed_ms_since_last_update);
	cursor_movement();
	spider_web_shooting_update(elapsed_ms_since_last_update);

	if (level_index != 0) {
		boss_spawn();
	}
	return true;
}

void WorldSystem::updateTimer(float elapsed_ms_since_last_update)
{
	for (auto& stopwatch : registry.stopwatches.components) {
		stopwatch.counter += elapsed_ms_since_last_update; //update stopwatches
	}
}

// Reset the world state to its initial state
void WorldSystem::restart_game()
{
	// Debugging for memory/component leaks
	registry.list_all_components();
	printf("Restarting\n");

	// Reset the game speed
	current_speed = 1.f;

	
	// Reset the timer *the timer will reset on every level we have*
	timer = 0;
	points = 0;

	// Remove all entities that we created
	// All that have a motion, we could also iterate over all fish, turtles, ... but that would be more cumbersome
	while (registry.motions.entities.size() > 0)
		registry.remove_all_components_of(registry.motions.entities.back());

	while (registry.uiPositions.entities.size() > 0)
		registry.remove_all_components_of(registry.uiPositions.entities.back());

	// Debugging for memory/component leaks
	registry.list_all_components();


    if(save_reload.reload){
        load.load_save(levels[level_index], &camera, renderer);
    }else {
        LevelBuilder levelBuilder(renderer);
        if (!levelBuilder.loadLevel(levels[level_index], &camera)) {
            std::cout << "Level Failed to load\n";
		}

    }

	// Adding the health bar here
	health_container = createHealthContainer(renderer);
	health_fill = createHealthFill(renderer);

	// Reset enemyspawn
	has_boss = false;
	bee_spawn = true;
	snail_spawn = true;

	// non-tutorial elements:
	if (level_index != 0) {
		help = createHelp(renderer, { registry.motions.get(camera).position.x + 1100 ,screen_height - 50 });
		
		Entity levelTimer = createTimer({ 1050,50 }, { 175, 50 });
		Entity bestText = createTextUIElem({ 1007.5,97.5 }, { 88,30 }, "BEST:");
		Entity level_best = createTextUIElem({ 1088,102.5 }, {70,20}, bestTimes[level_index]);

		Entity score_text = createTextUIElem({ 600,25 }, { 82, 20 }, "-SCORE-");
		scoreCounter = createTextUIElem({ 600,50 }, { 140, 30 }, "000000");
		Entity bestText2 = createTextUIElem({ 600,77.5 }, { 70,20 }, "-BEST-");	
		Entity bestScore = createTextUIElem({ 600, 97.5 }, { 63,15 }, formatScore(bestScores[level_index]));
	}

	

	// Create a new spider
	player_spider = createSpider(renderer);

    if(!save_reload.reload) {
		if (level_index == 0) {
			tutorial.in_tutorial_mode = true;
			tutorial_instructionsDisplay = createGameIntro(renderer);
			createSkip(renderer);
			tutorial.textDisplay = 0;
			tutorial.ants, tutorial.bees, tutorial.climb, tutorial.lastTextBeforeNextLevel, tutorial.shoot_trees, tutorial.swing, tutorial.wasd = false;
			registry.renderRequests.get(player_spider).used_effect = EFFECT_ASSET_ID::UI;
		}
		else if (level_index == 1) {
			tutorial.in_tutorial_mode = false;
			nextLevel = false;
			levelDisplay = createLevel1(renderer);
			registry.renderRequests.get(player_spider).used_effect = EFFECT_ASSET_ID::SPIDER;
	
		}
		else if (level_index == 2) {
			tutorial.in_tutorial_mode = false;
			gameFinished = false;
			levelDisplay = createLevel2(renderer);
			registry.renderRequests.get(player_spider).used_effect = EFFECT_ASSET_ID::SPIDER;
			if (displayOnce) {
				level2Instructions = createLevel2SnailDetail(renderer, { registry.motions.get(levelDisplay).position.x, registry.motions.get(levelDisplay).position.y + 300 });
				displayingLevel2Instructions = true;
				snailDetails = true;
				displayOnce = false;
			}
		}
    }


	//spider_mesh = createSpiderMesh(renderer, player_spider);
	health = HealthSystem();
	registry.currentHealth.emplace(player_spider);
	registry.collisionTimer.emplace(player_spider);
    if(save_reload.reload){
        registry.motions.get(player_spider).position = load.get_position_saved();
        registry.damageable.get(player_spider).health = load.get_health_saved();
        int health_deduct = 100 - load.get_health_saved();
        health.fullHealth(player_spider);
        HealthSystem::takeDamage(player_spider, health_deduct);
    }
	cursor = createCursor(renderer);
}

// Compute collisions between entities
void WorldSystem::handle_collisions()
{
	// Loop over all collisions detected by the physics system
	auto &collisionsRegistry = registry.collisions;
	for (uint i = 0; i < collisionsRegistry.components.size(); i++)
	{
		// The entity and its collider
		Entity entity = collisionsRegistry.entities[i];
		Entity entity_other = collisionsRegistry.components[i].other;
		if (registry.particles.has(entity) || registry.particles.has(entity_other))//particle collisions probably shouldn't generate events.
			continue;
		if (!registry.motions.has(entity) || !registry.motions.has(entity_other))//make sure that these entities haven't been deleted somewhere since the creation of this event.
			continue;

		PlayerStatus& ps = registry.playerStatus.get(player_spider);

		// For now, we are only interested in collisions that involve the spider
		if (registry.players.has(entity) && !registry.playerMesh.has(entity))
		{
			Player &player = registry.players.get(entity);

			// Checking Player - Enemy collisions
			if (registry.enemy.has(entity_other) )
			{
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity))
				{
					auto &colTimer = registry.collisionTimer.get(entity);
					float min_counter_ms = 2000.f;
					colTimer.counter_ms -= Elapsed_collisionTimer;
					if (colTimer.counter_ms < min_counter_ms)
					{
						min_counter_ms = colTimer.counter_ms;
					}
					if (colTimer.counter_ms < 0)
					{
						colTimer.counter_ms = 2000.f;
						//void (*fp)(Entity){health.instaKill};
						//health.takeDamage(entity, fp);
						// if spider has immune effect, do not remove any points (will keep noise to signal hit has been made though)
						if (!registry.immuneSpider.has(entity) && registry.beeSwarm.has(entity_other)) {
							registry.damageable.get(entity).takeDamage(entity, 5);
						//	registry.remove_all_components_of(entity_other);
						}
						else if (!registry.immuneSpider.has(entity) && registry.bossStatus.has(entity_other)) {
							registry.damageable.get(entity).takeDamage(entity, 5);
						}
						else if (!registry.immuneSpider.has(entity) && !registry.bossStatus.has(entity_other)) {
							registry.damageable.get(entity).takeDamage(entity, 33);
						}

						// want to remove bees from scene even if spider is immune 
						if (registry.beeSwarm.has(entity_other) || !registry.bossStatus.has(entity_other)) {
							registry.remove_all_components_of(entity_other);
						}
						Mix_PlayChannel(-1, bee_hit, 0);
					}
				}
			}
			else if (registry.indestructibleEnemy.has(entity_other)) {
				// initiate death unless already dying
				if (!registry.deathTimers.has(entity))
				{
					auto& colTimer = registry.collisionTimer.get(entity);
					float min_counter_ms = 100.f;
					colTimer.counter_ms -= Elapsed_collisionTimer;
					if (colTimer.counter_ms < min_counter_ms)
					{
						min_counter_ms = colTimer.counter_ms;
					}
					if (colTimer.counter_ms < 0)
					{
						colTimer.counter_ms = 100.f;
						if (!registry.immuneSpider.has(entity)) {
							std::cout << "hit snail here" << std::endl;
							registry.damageable.get(entity).takeDamage(entity, 20);
						}
						registry.remove_all_components_of(entity_other);
						Mix_PlayChannel(-1, bee_hit, 0);
					}

				}
			}
			// Checking Player - Friendly collisions
			else if (registry.friendly.has(entity_other))
			{
				if (!registry.deathTimers.has(entity))
				{
					// auto &healthStatus = registry.currentHealth.get(entity);

					// if (healthStatus.health < 100)
					//{
					///*	registry.remove_all_components_of(entity_other);
					//	Mix_PlayChannel(-1, spider_eat_sound, 0);*/
					//	++points;
					//	health.addHealth(entity, healthStatus);
					//	std::cout << std::to_string(healthStatus.health) << std::endl;
					//}

					health.addHealth(entity, 15);

					if (registry.xlAnt.has(entity_other)) {
						// emplace effect
						if (!registry.spiderEffect.has(entity)) {
							registry.spiderEffect.emplace(entity);
							registry.xlSpider.emplace(entity);
							Motion &m = registry.motions.get(entity);
							m.scale = {2 * 150, 100 * 2};
						}
					}
					else if (registry.xsAnt.has(entity_other))
					{
						if (!registry.spiderEffect.has(entity))
						{
							registry.spiderEffect.emplace(entity);
							Motion &m = registry.motions.get(entity);
							m.scale = {0.5 * 150, 100 * 0.5};
						}
					}
					else if (registry.immuneAnt.has(entity_other))
					{
						if (!registry.spiderEffect.has(entity))
						{
							registry.spiderEffect.emplace(entity);

							if (!registry.immuneSpider.has(entity))
							{
								registry.immuneSpider.emplace(entity);
								SpiderEffectTimer &sp = registry.spiderEffect.get(entity);
								sp.glow = 1;
							}
						}
					}
					else if (registry.healthAnt.has(entity_other))
					{
						health.fullHealth(entity);
					}
					else if (registry.fastAnt.has(entity_other))
					{
						if (!registry.spiderEffect.has(entity))
						{
							registry.spiderEffect.emplace(entity);

							if (!registry.fastSpider.has(entity))
							{
								registry.fastSpider.emplace(entity);
								PlayerStatus &player_status = registry.playerStatus.get(entity);
								player_status.movespeed = player_status.movespeed * 2;
							}
						}

					}
					registry.remove_all_components_of(entity_other);
					Mix_PlayChannel(-1, spider_eat_sound, 0);
				}
			}
			else if (registry.climbable.has(entity_other))
			{
				if (!registry.deathTimers.has(entity))
				{

					if (!registry.CurrentlyBeingClimbed.has(entity_other))
					{
						registry.CurrentlyBeingClimbed.emplace(entity_other);
					}

					// enabling spider ability to use W key to climb tree
					PlayerStatus &playerS = registry.playerStatus.get(entity);
					playerS.canClimb = true;
					

					if (level_index == 0) {
						Motion& m = registry.motions.get(player_spider);
						if (tutorial.climb && tutorial.textDisplay == 3 && m.position.y < SPIDER_DEFAULT_Y) {
							registry.remove_all_components_of(tutorial_instructionsDisplay);
							tutorial_instructionsDisplay = createGreatInstructions(renderer);
							tutorial.textDisplay++;
							tutorial.climb = false;
						}
					}
				}
			}
			else if (registry.trackingProjectile.has(entity_other))
			{
				if (!registry.deathTimers.has(entity))
				{
					auto &colTimer = registry.collisionTimer.get(entity);
					float min_counter_ms = 100.f;
					colTimer.counter_ms -= Elapsed_collisionTimer;
					if (colTimer.counter_ms < min_counter_ms)
					{
						min_counter_ms = colTimer.counter_ms;
					}
					if (colTimer.counter_ms < 0)
					{
						colTimer.counter_ms = 100.f;

						if (!registry.immuneSpider.has(entity))
						{
							registry.damageable.get(entity).takeDamage(entity, 15);
						}

						registry.remove_all_components_of(entity_other);
						Mix_PlayChannel(-1, stinger_impact, 0);
						// std::cout << "im shot" << std::endl;
						// health.takeDamageStinger(entity, fp);
					}
				}
			}
		}

		if (registry.webshoots.has(entity) && registry.motions.has(entity_other))
		{
			WebShoot webshoot = registry.webshoots.get(entity);
			Motion& motion_other = registry.motions.get(entity_other);
			Motion &motion = registry.motions.get(entity);
			vec2 diff = motion.position - motion_other.position;
			// Trigger on hit
			if (registry.enemy.has(entity_other))
			{
				addScore(HIT_SCORE);
				if (registry.beeHive.has(entity_other)) {
					createAreaEmitter(motion_other.position, motion_other.scale, ParticleType::BEE);
					createPointEmitter(motion_other.position, diff, ParticleType::BEE);
					destroyHive(entity_other);
					delay.beeDodge = false;
					addScore(BEE_SCORE);
				}
				else if (registry.beeSwarm.has(entity_other)) {
					createAreaEmitter(motion_other.position, motion_other.scale, ParticleType::BEE);
					createPointEmitter(motion_other.position, diff, ParticleType::BEE);
					registry.remove_all_components_of(entity_other);
					delay.beeDodge = false;
					addScore(BEE_SCORE / 10);
				}
				else if (!registry.bossStatus.has(entity_other))
				{
					createAreaEmitter(motion_other.position, motion_other.scale, ParticleType::BEE);
					createPointEmitter(motion_other.position, diff, ParticleType::BEE);
					//registry.remove_all_components_of(entity_other);
					Damageable& damageable = registry.damageable.get(entity_other);
					if(damageable.takeDamage(entity_other, webshoot.damage))
						addScore(BEE_SCORE);

					// registry.remove_all_components_of(entity_other);
					delay.beeDodge = false;

					if (level_index == 0) {
						Motion& m = registry.motions.get(player_spider);
						if (tutorial.bees && tutorial.textDisplay == 10) {
							registry.remove_all_components_of(tutorial_instructionsDisplay);
							tutorial_instructionsDisplay = createGreatInstructions(renderer);
							tutorial.bees = false;
						}
					}
				}
				else if (registry.bossStatus.has(entity_other))
				{
					BossStatus &boss = registry.bossStatus.get(entity_other);
					boss.health -= webshoot.damage;
					createAreaEmitter(motion_other.position, motion_other.scale, ParticleType::BEE);
					createPointEmitter(motion_other.position, diff, ParticleType::BEE);

					delay.beeDodge = false;

					if (boss.health <= 0) {
						addScore(BOSS_SCORE);
						if (level_index == 1) {
							prompts = createLevel2Prompt(renderer, { registry.motions.get(camera).position.x + 625, registry.motions.get(camera).position.y + 400 });
							nextLevel = true;
						}
						else if (level_index == 2) {
							prompts = createEndOfGamePrompt(renderer, { registry.motions.get(camera).position.x + 625, registry.motions.get(camera).position.y + 400 });
							gameFinished = true;
						}
					}
				}
				registry.remove_all_components_of(entity);
			}
			else if (registry.trackingProjectile.has(entity_other))
			{			
				registry.remove_all_components_of(entity);
				delay.beeDodge = false;
				registry.remove_all_components_of(entity_other);
				addScore(STINGER_SCORE);
			}
			// Precise targeting

			if (pos_distance(webshoot.target_pos, motion.position) < webshoot.detect_dist)
			{
				if (registry.climbable.has(entity_other))
				{
					Climbable &climbable = registry.climbable.get(entity_other);
					// climbable.health -= webshoot.damage;

					// registry.spriteSheets.get(entity_other).index++;
					Entity residual = createWebShootResidual(renderer, motion.position);
					climbable.attachments.push_back(residual);
					 
					if (registry.xlSpider.has(player_spider)) {
						if(registry.damageable.get(entity_other).takeDamage(entity_other, webshoot.damage * 2.0))
							addScore(TREE_SCORE);
					}
					else {
						if(registry.damageable.get(entity_other).takeDamage(entity_other, webshoot.damage))
							addScore(TREE_SCORE);
					}
					delay.beeDodge = false;

					if (level_index == 0) {
						if (tutorial.shoot_trees && tutorial.textDisplay == 5) {
							registry.remove_all_components_of(tutorial_instructionsDisplay);
							tutorial_instructionsDisplay = createKeepDestroyingInstructions(renderer);
							tutorial.textDisplay++;
						}
						if (tutorial.shoot_trees && tutorial.textDisplay == 6 && !registry.climbable.has(entity_other)) {
							registry.remove_all_components_of(tutorial_instructionsDisplay);
							tutorial_instructionsDisplay = createGreatInstructions(renderer);
							tutorial.textDisplay++;
							tutorial.shoot_trees = false;
						}
		
					}


					registry.remove_all_components_of(entity);
					createPointEmitter(motion.position, diff, ParticleType::TREE);
				}
				else if(registry.unclimbable.has(entity_other)){
					// didn't hit a tree, that means we've hit a bush!
					Unclimbable& unclimbable = registry.unclimbable.get(entity_other);
					Damageable& damageable = registry.damageable.get(entity_other);
					Entity residual = createWebShootResidual(renderer, motion.position);
					unclimbable.attachments.push_back(residual);

					if (registry.damageable.get(entity_other).takeDamage(entity_other, webshoot.damage))
						addScore(TREE_SCORE);

					registry.remove_all_components_of(entity);
					createPointEmitter(motion.position, diff, ParticleType::TREE);
					
				}
			}
		}
	}

	// Remove all collisions from this simulation step
	registry.collisions.clear();
}


void WorldSystem::handle_creation()
{
	// Sting creation
	if (registry.projectileCreationList.entities.size() > 0)
	{
		for (int i = 0; i<registry.projectileCreationList.entities.size(); i++)
		{
			Entity entity = registry.projectileCreationList.entities[i];
			for (ProjectileCreation creation : registry.projectileCreationList.get(entity).creation_list)
			{
				if (creation.wait_creation)
				{
					Entity projectile_entity = createSting(renderer, creation.spawn_pos, creation.target);
				}
			}

			registry.projectileCreationList.remove(entity);
		}
	}

	for (Entity e : registry.beeHive.entities) {
		auto& hive = registry.beeHive.get(e);
		auto& hive_motion = registry.motions.get(e);
		if (hive.spawn_timer <= 0 && hive.swarm_count < hive.max_swarm_count) {
			Entity bee = createSwarmBee(renderer, hive_motion.position);
			hive.swarm.push_back(bee);
			hive.spawn_timer = hive.spawn_time;
			hive.swarm_count += 1;
		}
	}
}

void WorldSystem::handle_texts(float elapsed_ms)
{
	for (Entity e : registry.texts.entities)
	{
		auto &text_property = registry.texts.get(e);
		text_property.remove_timer -= elapsed_ms;
		if (text_property.remove_timer <= 0)
		{
			registry.remove_all_components_of(e);
		}
	}
}

void WorldSystem::handle_animation(float elapsed_ms_since_last_update)
{
	float min_counter_ms = 22000.f;
	for (Entity entity : registry.animation.entities)
	{
		Animation &counter = registry.animation.get(entity);
		counter.displaying_asset_time_counter += elapsed_ms_since_last_update;
			// IDLE SPIDER
			if (counter.SPIDER_IDLE)
			{
				if (counter.idle_state)
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_idle)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER_IDLE_EARS;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.idle_state = 0;
					}
				}
				else
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_idle)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.idle_state = 1;
					}
				}
			}

			// SPIDER MOVING LEFT/DOWN
			if (counter.SPIDER_LEFT || counter.SPIDER_DOWN)
			{
				if (counter.left_state == 0)
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_moving)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER_LEFT1;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.left_state = 1;
					}
				}
				if (counter.left_state == 1)
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_moving)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER_LEFT2;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.left_state = 2;
					}
				}
				if (counter.left_state == 2)
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_moving)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER_LEFT3;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.left_state = 0;
					}
				}
			}

			// SPIDER MOVING RIGHT/UP
			if (counter.SPIDER_RIGHT || counter.SPIDER_UP)
			{
				if (counter.right_state == 0)
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_moving)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER_RIGHT1;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.right_state = 1;
					}
				}
				if (counter.right_state == 1)
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_moving)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER_RIGHT2;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.right_state = 2;
					}
				}
				if (counter.right_state == 2)
				{
					if (counter.displaying_asset_time_counter <= counter.display_time_moving)
					{
						registry.renderRequests.get(entity).used_texture = TEXTURE_ASSET_ID::SPIDER_RIGHT3;
					}
					// once sec has passed, reset t and change the state so next asset is displayed
					else
					{
						counter.displaying_asset_time_counter = 0;
						counter.right_state = 0;
					}
				}
			}
	}
}

// Should the game be over ?
bool WorldSystem::is_over() const
{
	return bool(glfwWindowShouldClose(window));
}

void WorldSystem::on_mouse_button(int key, int action, int mod)
{
	
	if (key == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (action == GLFW_PRESS)
		{
			if (level_index >= 2) {
				
				if (!mode_big_acid) {
					spider_shooting(renderer, cursor_position);
				}
				if (mode_big_acid) {
					acid_buildup_factor = 0.f;
					mouse_hold = true;
					mouse_was_held = true;
				}
				delay.beeDodge = true;
			}
			else {
				spider_shooting(renderer, cursor_position);
				delay.beeDodge = true;
			}
			
		}
		if (action == GLFW_RELEASE) {
			if (level_index >= 2) {
				if (mode_big_acid) {
					mouse_hold = false;

				}
			}
		}
		
	}

	if (key == GLFW_MOUSE_BUTTON_RIGHT)
	{
		CursorGesture &gest = registry.cursorGesture.get(cursor);
		if (action == GLFW_PRESS)
		{
			gest.cursor_gesture_pos1 = cursor_position;
		}
		if (action == GLFW_RELEASE)
		{
			PlayerStatus& playerStatus = registry.playerStatus.get(player_spider);
			if (playerStatus.swing) {
				return;
			}
			gest.cursor_gesture_pos2 = cursor_position;
			auto& player_motion = registry.motions.get(player_spider);
			if (pos_distance(gest.cursor_gesture_pos1, gest.cursor_gesture_pos2) > 100 && gest.cursor_gesture_pos2.y < player_motion.position.y)
			{
				gest.gesture = 1;
				gest.disappear_timer = 5000;
				auto &motion = registry.motions.get(player_spider);
				vec2 direction = move_toward(gest.cursor_gesture_pos2, gest.cursor_gesture_pos1);
				motion.velocity = vec2(0, 0);
				// motion.position = motion.position + vec2(direction.x * 200.f, direction.y * 200.f);
				
				playerStatus.shootingWeb = true;
				playerStatus.swing = false;
				if (motion.angle != 0) {
					motion.angle = 0;
					playerStatus.falling = false;
				}


				if (level_index == 0) {
					if (tutorial.textDisplay == 8 && tutorial.swing) {
						registry.remove_all_components_of(tutorial_instructionsDisplay);
						tutorial_instructionsDisplay = createGreatInstructions(renderer);
						tutorial.textDisplay++;
						tutorial.swing = false;
					}
				}
				
				playerStatus.webStringTargetDirection = move_toward(gest.cursor_gesture_pos2, motion.position);
				playerStatus.webStringTargetPos = gest.cursor_gesture_pos2;
				playerStatus.webStringEndPointPos = motion.position;
				delay.beeDodge = true;

			}
		}
	}
}

// On key callback
void WorldSystem::on_key(int key, int, int action, int mod)
{

	player_movement(key, action, mod);

	// Resetting game
	if (action == GLFW_RELEASE && key == GLFW_KEY_R && !helpOn)
	{
		int w, h;
		glfwGetWindowSize(window, &w, &h);
        save_reload.reload = false;
        restart_game();
	}

	// Debugging
	if (key == GLFW_KEY_O)
	{
		if (action == GLFW_RELEASE)
			debugging.in_debug_mode = false;
		else
			debugging.in_debug_mode = true;
	}

	// for now, and for debugging purposes, "<" and ">" keys will change levels.
	if (action == GLFW_RELEASE && key == GLFW_KEY_COMMA)
	{
		level_index--;
		if (level_index < 0)
		{
			level_index = levels.size() - 1;
		}
		restart_game();
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_PERIOD)
	{
		level_index++;
		if (level_index >= levels.size())
		{
			level_index = 0;
		}
		restart_game();
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_ESCAPE)
	{
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}

    // load save
    if(action == GLFW_RELEASE && key == GLFW_KEY_I){
        if(!save_reload.reload) {
            save_reload.reload = true;
        }
        restart_game();
    }

    // save
    if (action == GLFW_RELEASE && key == GLFW_KEY_U)
    {
        if(save_reload.reload) {
            save_reload.reload = false;
        }
        save();
    }

	// ZOOM MODE
	if (action == GLFW_RELEASE && key == GLFW_KEY_Z && level_index != 0) {
		delay.zoomMode = true;
		delay.zoomPause = true;
	}

	// REGULAR MODE
	if (action == GLFW_RELEASE && key == GLFW_KEY_X && level_index != 0) {
		delay.zoomMode = false;
		delay.zoomPause = false;
	}

	// HELP
	if (action == GLFW_RELEASE && key == GLFW_KEY_H && level_index !=0) {
		// if help doc isn't displayed, we want to display it
		if (!helpOn) {
			// create helpDoc
			helpDoc = createHelpDoc(renderer, { registry.motions.get(camera).position.x + 625, registry.motions.get(camera).position.y + 400 });

			// pause the game 
			PlayerStatus& status = registry.playerStatus.get(player_spider);
			Motion& spiderMotion = registry.motions.get(player_spider);

			// pause spider and disable WASD
			status.enableKeyboardInput = false;
			spiderMotion.velocity = { 0,0 };

			// pause all spawning of new bees/ants
			pause = true;
			
			// toggle bool 
			helpOn = true;
		}
		else {
			registry.remove_all_components_of(helpDoc);
			PlayerStatus& status = registry.playerStatus.get(player_spider);
			status.enableKeyboardInput = true;

			// resume spawning
			pause = false;
			helpOn = false;

		}

	}

	// TUTORIAL
	if (action == GLFW_RELEASE && key == GLFW_KEY_N && level_index == 0) {
		level_index++;
		if (level_index >= levels.size())
		{
			level_index = 0;
		}
		restart_game();
	}
	// NEXT LEVEL DEBUG
	if (action == GLFW_RELEASE && key == GLFW_KEY_0) {
		level_index++;
		if (level_index >= levels.size())
		{
			level_index = 0;
		}
		restart_game();
	}
	if (action == GLFW_RELEASE && key == GLFW_KEY_SPACE) {
		if (level_index == 0) {
			// if in intro text, move display WASD text
			if (tutorial.textDisplay == 0) {
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createWASDInstructions(renderer);
				tutorial.textDisplay++;
				tutorial.wasd = true;
			}
			// [WASD]
			// move from GREAT to climb instructions
			if (tutorial.textDisplay == 2) {
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createClimbInstructions(renderer);
				tutorial.textDisplay++;
				tutorial.climb = true;
			}
			// [climbed tree]
			// move from  GREAT to shoot instructions  
			if (tutorial.textDisplay == 4) {
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createShootInstructions(renderer);
				tutorial.textDisplay++;
				tutorial.shoot_trees = true;
			}
			// [destroyed tree]
			// swing instructions
			else if (tutorial.textDisplay == 7) {
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createSwingInstructions(renderer);
				tutorial.textDisplay++;
				tutorial.swing = true;
			}
			// [swing]
			// bee insturctions
			if (tutorial.textDisplay == 9) {
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createBeeInstructions(renderer);
				tutorial.textDisplay++;
				tutorial.bees = true;
			}
			// [hit bee]
			// ant info
			if (tutorial.textDisplay == 10 && !tutorial.bees) {
				tutorial.bees = false;
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createAntInstructions(renderer);
				tutorial.textDisplay++;
				tutorial.ants = true;
			}
			// [added ants to scene]
			// ant details
			else if (tutorial.textDisplay == 11 && tutorial.ants) {
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createAntDetails(renderer);
				tutorial.textDisplay++;
			}
			else if (tutorial.textDisplay == 12) {
				registry.remove_all_components_of(tutorial_instructionsDisplay);
				tutorial_instructionsDisplay = createReady(renderer);
				tutorial.ants = false;
				tutorial.textDisplay++;
				tutorial.lastTextBeforeNextLevel = true;
			}
			else if (tutorial.lastTextBeforeNextLevel && tutorial.textDisplay == 13) {
				tutorial.ants = false;
				// load level 1
				level_index++;
				restart_game();
			}
		}
		else if (level_index == 1 && nextLevel) {
			//load level 2
			saveBestScores();
			level_index++;
			restart_game();
		}
		else if (level_index == 2 && gameFinished) {
			//restart at level 1
			saveBestScores();
			level_index = 1;
			restart_game();
		}
		else if (level_index == 2 && snailDetails && displayingLevel2Instructions) {
			vec2 pos = { registry.motions.get(level2Instructions).position.x, registry.motions.get(level2Instructions).position.y };
			registry.remove_all_components_of(level2Instructions);
			level2Instructions = createLevel2Acid2Detail(renderer, pos);
			acidDetails = true;
			snailDetails = false;
		} 
		else if (level_index == 2 && acidDetails && displayingLevel2Instructions) {
			registry.remove_all_components_of(level2Instructions);
			displayingLevel2Instructions = false;
			acidDetails = false;
		}
		
	}

	if (action == GLFW_RELEASE && key == GLFW_KEY_TAB) {
		if (level_index >= 2) {
			mode_big_acid = !mode_big_acid;
			std::cout << "mode_big_acid: " << mode_big_acid << std::endl;
		}
	}
}

void WorldSystem::on_mouse_move(vec2 mouse_position)
{
	(vec2) mouse_position; // dummy to avoid compiler warning
	Motion &cursor_motion = registry.motions.get(cursor);
	Motion &cam_motion = registry.motions.get(camera);
	mat3 screen_to_world = renderer->getScreenToWorldMatrix();
	vec3 worldCoord = screen_to_world * vec3(mouse_position, 1.0);// *1200.f / 1500.f - vec2(485, 0);
	mouse_window_position = vec2(clamp(worldCoord.x, 0.f,1200.f), clamp(worldCoord.y, 0.f, 800.f));
	//std::cout << "MousePosition: " << mouse_window_position.x << ", " << mouse_window_position.y << "\n";
}

void bringBackDebuggingToNormal()
{
	debugging.acceleration = 0;
	debugging.velocity = 0;
	debugging.angle = M_PI / 4;
	debugging.gravity = 0.5;
	debugging.damping = 0.995;
}
void WorldSystem::player_movement(int key, int action, int mod)
{
	//Motion& mesh_motion = registry.motions.get(spider_mesh);
	Motion &motion = registry.motions.get(player_spider);
	PlayerStatus &player_status = registry.playerStatus.get(player_spider);
	Animation &animate = registry.animation.get(player_spider);
	if (player_status.shootingWeb || player_status.forcefall)
	{
		return;
	}
	if ((key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D) && player_status.swing) {
		CursorGesture& gest = registry.cursorGesture.get(cursor);
		if (gest.disappear_timer > 3000) {
			return;
		}
		motion.velocity = player_status.flyoff_dir * player_status.movespeed * 1.5f;
		if (key == GLFW_KEY_A) {
			if (motion.velocity.x > 0)
				motion.velocity.x *= -1;

			motion.velocity *= 0.7;
		}
		if (key == GLFW_KEY_D) {
			if (motion.velocity.x < 0)
				motion.velocity.x *= -1;

			motion.velocity *= 0.7;
		}
		player_status.forcefall = true;
		
		gest.disappear_timer = 0;
		return;
	}
	if (level_index == 0) {
		if ((key == GLFW_KEY_W || key == GLFW_KEY_A || key == GLFW_KEY_S || key == GLFW_KEY_D) && tutorial.wasd && tutorial.textDisplay==1) {
			registry.remove_all_components_of(tutorial_instructionsDisplay);
			tutorial_instructionsDisplay = createGreatInstructions(renderer);
			tutorial.textDisplay++;
			tutorial.wasd = false;
		}
	}
	
	// WASD key functionaliy
	if (key == GLFW_KEY_D && !registry.deathTimers.has(player_spider) && player_status.enableKeyboardInput)
	{

		if (action == GLFW_RELEASE)
		{
			motion.velocity.x = 0;
			//mesh_motion.velocity.x = 0;
			animate.SPIDER_RIGHT = 0;
			animate.SPIDER_IDLE = 1;
			if (delay.zoomMode) {
				delay.zoomPause = true;
			}
		}
		if (action == GLFW_PRESS)
		{
			animate.SPIDER_RIGHT = 1;
			animate.SPIDER_IDLE = 0;
			motion.velocity.x = player_status.movespeed;
			if (delay.zoomMode) {
				delay.zoomPause = false;
			}
			if (registry.webString.size() > 0)
			{
				player_status.swing = false;
				bringBackDebuggingToNormal();
			}
		}
	}

	if (key == GLFW_KEY_A && !registry.deathTimers.has(player_spider) && player_status.enableKeyboardInput)
	{



		if (action == GLFW_RELEASE)
		{
			motion.velocity.x = 0;
			//mesh_motion.velocity.x = 0;
			animate.SPIDER_LEFT = 0;
			animate.SPIDER_IDLE = 1;
			if (delay.zoomMode) {
				delay.zoomPause = true;
			}
		}
		if (action == GLFW_PRESS)
		{
			animate.SPIDER_LEFT = 1;
			animate.SPIDER_IDLE = 0;
			motion.velocity.x = -player_status.movespeed;
			if (delay.zoomMode) {
				delay.zoomPause = false;
			}
			if (registry.webString.size() > 0)
			{
				player_status.swing = false;
				bringBackDebuggingToNormal();
			}
		}
	}

	if (key == GLFW_KEY_W && !registry.deathTimers.has(player_spider) && player_status.canClimb && !player_status.falling && player_status.enableKeyboardInput)
	{

		if (action == GLFW_RELEASE)
		{
			motion.velocity.y = 0;
			//mesh_motion.velocity.y = 0;
			animate.SPIDER_UP = 0;
			animate.SPIDER_IDLE = 1;
			if (delay.zoomMode) {
				delay.zoomPause = true;
			}
		}
		if (action == GLFW_PRESS)
		{
			animate.SPIDER_UP = 1;
			animate.SPIDER_IDLE = 0;
			motion.angle = -1.57;
			motion.velocity.y = -player_status.movespeed;
			if (delay.zoomMode) {
				delay.zoomPause = false;
			}
			if (registry.webString.size() > 0)
			{
				player_status.swing = false;
				bringBackDebuggingToNormal();
			}
		}
	}

	if (key == GLFW_KEY_S && !registry.deathTimers.has(player_spider) && !player_status.falling && player_status.enableKeyboardInput)
	{

		if (action == GLFW_RELEASE)
		{
			motion.velocity.y = 0;
			//mesh_motion.velocity.y = 0;
			animate.SPIDER_DOWN = 0;
			animate.SPIDER_IDLE = 1;
			if (delay.zoomMode) {
				delay.zoomPause = true;
			}
	
		}
		if (action == GLFW_PRESS)
		{
			animate.SPIDER_DOWN = 1;
			animate.SPIDER_IDLE = 0;
			motion.velocity.y = player_status.movespeed;
			if (delay.zoomMode) {
				delay.zoomPause = false;
			}
			if (registry.webString.size() > 0)
			{
				player_status.swing = false;
				bringBackDebuggingToNormal();
			}
		}
	}
}

void WorldSystem::boss_spawn()
{

	if (registry.climbable.size() <= 0)
	{
		snail_spawn = false;
		bee_spawn = false;
		if (!has_boss)
		{
		/*	createBossBee(renderer, vec2(screen_width, screen_height));*/
			Motion& spider = registry.motions.get(player_spider);
			createBossBee(renderer, vec2(screen_width, screen_height), spider.position);
		}
		has_boss = true;
	}
}

// This checks if the player is near the bounds of the screen,
//  if yes then the camera moves
//
void WorldSystem::checkPlayerOnEdge(int screen_width, float elapsed_ms)
{
	// Check if player hits the edge of the screen
	Motion &motion = registry.motions.get(player_spider);
	PlayerStatus &player_status = registry.playerStatus.get(player_spider);
	Motion &camMotion = registry.motions.get(camera);
	Camera &cam = registry.cameras.get(camera);
	float posSpiderRelativeToCam = motion.position.x - camMotion.position.x;
	float step_seconds = 1.0f * (elapsed_ms / 1000.f);

	if (level_index != 0) {
		Motion& helpMotion = registry.motions.get(help);
		if (posSpiderRelativeToCam >= screen_width - cam.camera_movement_detection_dist && camMotion.position.x < cam.xBound[1])
		{
			cam.expect_pos.x += player_status.movespeed * step_seconds;
		}
		else if (posSpiderRelativeToCam <= cam.camera_movement_detection_dist && camMotion.position.x > cam.xBound[0])
		{
			cam.expect_pos.x -= player_status.movespeed * step_seconds;
		}
		helpMotion.position = { camMotion.position.x + 1150, screen_height - 50 };
		camera_movement();
		player_healthbar_motion_update();
	}
}

void WorldSystem::player_healthbar_motion_update() {
	auto& damageable = registry.damageable.get(player_spider);

	float remaining = max(0.f, (float)damageable.health / damageable.maxHealth);
	auto& HealthBar = registry.healthBar.entities[0];
	auto& health_bar_prop = registry.healthBar.get(HealthBar);
	auto& healthBarMotion = registry.motions.get(HealthBar);

	healthBarMotion.scale.x = remaining * 150;
	health_bar_prop.pos.x = 100 - (150 - healthBarMotion.scale.x)/2;
	Motion& cam_motion = registry.motions.get(camera);
	auto& healthcontainer = registry.healthContainer.entities[0];
	auto& healthContainerMotion = registry.motions.get(healthcontainer);

	healthContainerMotion.position = vec2(100.f, 40.f) + cam_motion.position;
	healthBarMotion.position = health_bar_prop.pos + cam_motion.position;

}

void WorldSystem::camera_movement()
{
	Motion &cam_motion = registry.motions.get(camera);
	Camera &cam = registry.cameras.get(camera);
	
	Motion& spider = registry.motions.get(player_spider);
	if (level_index != 0) {
		cam_motion.position.x = cam_motion.position.x + ((cam.expect_pos.x) - cam_motion.position.x) * cam.camera_smoothing_factor;
	}
}

void WorldSystem::cursor_movement()
{
	Motion &cam_motion = registry.motions.get(camera);
	Motion &cursor_motion = registry.motions.get(cursor);
	cursor_position = mouse_window_position + cam_motion.position;
	cursor_motion.position = cursor_position;
	cursor_render();
}

void WorldSystem::cursor_render()
{
	Curssor &cursor_type = registry.cursor.get(cursor);
	registry.renderRequests.get(cursor).used_texture = cursor_type.cursor_sprite;
}

void WorldSystem::save() {
    float treePosX [20];
    float treePosY [20];
    float treeHealth [20];
    auto &climbable = registry.climbable;
    for (int i = 0; i < climbable.components.size(); ++i) {
        Entity entity = climbable.entities[i];
        Motion& motion = registry.motions.get(entity);
        if (registry.climbable.has(entity)) {
            treePosX [i] = motion.position.x;
            treePosY [i] = motion.position.y;
            treeHealth [i] = registry.damageable.get(entity).health;
        }
    }
    auto &healthFillerMotion = registry.motions.get(health_fill);

    float player_health = registry.damageable.get(player_spider).health;
    vec2 player_pos = registry.motions.get(player_spider).position;
    load.save_load(player_pos, player_health, treeHealth, treePosX, treePosY, levels[level_index]);
}
