#pragma once
#include "common.hpp"

#include <vector>
#include <unordered_map>
#include "../ext/stb_image/stb_image.h"
#include <map>
#include <string>

// Player component
struct Player
{
};

struct PlayerMesh {
	Entity player;
};

struct BeeMesh {

};

// Bees are enemies
struct Enemy
{
	BeeMesh enemyMesh;
};

struct IndestructibleEnemy
{
	
};
struct BeeHive {
	float spawn_timer;
	float spawn_time;
	int swarm_count;
	int max_swarm_count;
	std::vector<Entity> swarm;
	Entity tree_ref;
};

struct BeeSwarm {
	vec2 scale = { 50,50 };
	bool hive_alive = true;
	vec2 spawn_point;
	float swarm_speed;
	float visualRange;
	float centeringFactor;
	float avoidFactor;
	float matchingFactor;
	float minDistance;
};
// Ants are friendlies
struct Friendly
{

};

struct WebString
{
	float acceleration;
	float velocity;
	float angle = M_PI / 1000;
	float gravity = 30;
};

struct Damageable {
	int health = 100;
	int maxHealth = 100;
	bool(*takeDamage)(Entity, int); //these functions must should true if the entity is killed by this damage, false otherwise.
	std::vector<Entity> attachments;
};

// for trees/entities that spider can climb
struct Climbable {
	std::vector<Entity> attachments;
	float health = 100;
};

struct Unclimbable {
	std::vector<Entity> attachments;
	float health = 100;
};

struct CurrentHealth {
	int health = 100;
};

// for trees/entities that the spider is currentlyClimbing
struct currentlyBeingClimbed {

};

struct AntXL {

};

struct AntImmune {

};
struct SpiderEffectTimer {
	float timer = 8000;
	int glow = 0;
};

struct AntXS {
};
struct AntFast {

};

struct AntRestoreHealth {

};

struct SpiderRestore {

};

struct SpiderImmune {

};

struct SpiderFast {

};
struct SpiderXL {

};

struct Animation {
	float displaying_asset_time_counter = 0;
	float display_time_idle = 100;
	float display_time_moving = 75;
	int SPIDER_IDLE = 1;
	int SPIDER_LEFT = 0;
	int SPIDER_RIGHT = 0;
	int SPIDER_UP = 0;
	int SPIDER_DOWN = 0;
	int idle_state = 0;
	int left_state = 0;
	int right_state = 0;
};

struct Camera {
	vec2 expect_pos = { 0,0 };
	vec2 xBound{ 0,0 };
	float camera_smoothing_factor = 0.1;
	float camera_movement_detection_dist = 300;
};

struct BackgroundDepth {
	float depth = 0;
};

// All data relevant to the shape and motion of entities
struct Motion {
	vec2 position = { 0, 0 };
	float angle = 0;
	vec2 velocity = { 0, 0 };
	vec2 scale = { 10, 10 };
};

struct Physics {
	float mass = 0;
	float terminal_speed = 0;
	bool gravity_enabled = true;
	bool collisions_enabled = true;
};

struct Levels {
	float timer = 3000;
};

// Stucture to store collision information
struct Collision
{
	// Note, the first object is stored in the ECS container.entities
	Entity other; // the second object involved in the collision
	Collision(Entity& other) { this->other = other; };
};

struct Tutorial {
	bool wasd = false;
	bool bees = false;
	bool ants = false;
	bool shoot_trees = false;
	bool swing = false;
	bool climb = false;
	bool lastTextBeforeNextLevel = false;
	int textDisplay = 0;
	bool in_tutorial_mode = false;
};
extern Tutorial tutorial;

// Data structure for toggling debug mode
struct Debug
{
	bool in_debug_mode = 0;
	bool in_freeze_mode = 0;
	float acceleration = 0;
	float velocity = 0;
	float angle = M_PI / 4;
	float gravity = 0.5;
	float damping = 0.995;
	bool firstSwing = true;
};
extern Debug debugging;

// Help structure for toggling help mode
struct Help {
	bool in_help_mode = 0;
};
extern Help helper;

// delay
struct Delay {
	bool zoomPause = false;
	bool zoomMode = false;
	bool beeDodge = false;
};
extern Delay delay;

// Sets the brightness of the screen
struct ScreenState
{
	float darken_screen_factor = -1;
};

//
struct Save {
    bool reload = 0;
};
extern Save save_reload;

// A struct to refer to debugging graphics in the ECS
struct DebugComponent
{
	// Note, an empty struct has size 1
};

// A timer that will be associated to dying salmon
struct DeathTimer
{
	float counter_ms = 3000;
};


struct CollisionTimer {
	float counter_ms = 0;
};

// Single Vertex Buffer element for non-textured meshes (coloured.vs.glsl & salmon.vs.glsl)
struct ColoredVertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

struct HealthBar {
	vec2 pos;

};

struct HealthContainer {

};


// Mesh datastructure for storing vertex and index buffers
struct Mesh
{
	static bool loadFromOBJFile(std::string obj_path, std::vector<ColoredVertex>& out_vertices, std::vector<uint16_t>& out_vertex_indices, vec2& out_size);
	vec2 original_size = { 1,1 };
	std::vector<ColoredVertex> vertices;
	std::vector<uint16_t> vertex_indices;
};

struct PlayerStatus
{
	float movespeed;
	// reload countdown
	float reload_timer;
	// timer preset
	float reload_time_preset;

	float webshoot_speed;
	// more modifier here
	// float webshoot_size;
	bool swing = false;
	bool canClimb = false;
	bool falling = false;
	
	bool shootingWeb = false;

	bool forcefall = false;
	float webStringSpeed = 3000;
	vec2 webStringTargetDirection;
	vec2 webStringTargetPos;
	vec2 webStringEndPointPos;

	vec2 flyoff_dir;
	bool enableKeyboardInput = true;
};
enum class ENEMY_STATE {
	IDLE,
	ALERT,
	ATTACK,
	POST_ATTACK
};

enum class ENEMY_TYPE {
	BEE_CHARGER,
	BEE_RANGER,
	BEE_SWARM,
	LORD_BEE,
};

struct EnemyDecision {

	ENEMY_STATE ai_state;
	ENEMY_TYPE ai_type;
	float state_switch_timer;

	float idle_reset_time;
	float alert_reset_time;
	float post_attack_reset_time;

	float alert_duration;
	// Distance for enemy to launch an attack
	float attack_range;
	// Distance for enemy to alert
	float alert_range;
	vec2 attack_target_pos;
	vec2 pre_attack_pos;
	float speed;
	float alert_speed_factor;
	float attack_speed_factor;
};

enum class BOSS_STATE {
	IDLE_WANDER,
	IDLE_TRACK,
	EVADE,
	CHARGE_ATTACK,
	RANGE_ATTACK,
	FREEZE,
	RETURN,
};

enum class BOSS_MODE {
	CHARGE_MODE,
	RANGE_MODE
};

struct BossStatus {
	BOSS_STATE boss_state;
	BOSS_MODE boss_mode;
	
	float health;
	Entity healthbar;

	vec2 default_pos;

	float charge_range;
	float range_range;
	float evasion_range;
	float evasion_cd_timer;
	float evasion_cd_time;
	float charge_attack_cd_timer;
	float charge_attack_cd_time;

	float state_switch_timer;
	
	float talk_timer;
	float talk_interval;
	float talk_duration;
	float intro_talk;
	bool random_talk;
	bool talk;

	float evasion_time;
	float idle_track_time;
	float idle_wander_time;
	float charge_attack_time;
	
	float range_attack_time;
	int range_attack_counter;
	float range_attack_cd_timer;
	float range_attack_cd_time;

	float mode_change_timer;
	float mode_change_time;

	float post_attack_wait_time;
	float post_evasion_wait_time;

	vec2 attack_target_pos;
	Entity attack_target_entity;

	float charge_mode_speed;
	float charge_attack_speed;
	float range_mode_speed;
	float evasion_speed;
	vec2 movement_direction;

	float death_timer;

};

struct BossHealthBar {
	float boss_max_health;
	float healthbar_default_scale;
	vec2 position;
};


struct TrackingProjectile {
	float disappear_timer;
	float projectile_speed;
	Entity target;
};

struct ProjectileCreation {
	bool wait_creation;
	vec2 spawn_pos;
	Entity target;

};

struct ProjectileCreationList {
	std::vector<ProjectileCreation> creation_list;
};

struct HelpDoc
{
};

struct CursorGesture {
	bool gesture = 0;
	// for temp debug
	float disappear_timer;
	vec2 cursor_gesture_pos1 = { 0,0 };
	vec2 cursor_gesture_pos2 = { 0,0 };
};

struct WebShoot {
	float disappear_timer;
	vec2 target_pos;
	float detect_dist = 50;
	float damage = 20;
};

struct TextAssets {
	float remove_timer;
};

struct Emitter {
	std::vector<Entity> particles = {};
	float lifetime = 1500;
	vec3 colour = { 0,0,0 };
};

struct Particle {
	bool enabled = false;
	float lifetime = 1500; // the particles can have separate lifetimes than the emitters
};

struct Stopwatch { //different from a timer because it counts up instead of down.
	float counter = 0;
};

struct UIPosition {
	vec2 position = { 0,0 };
	vec2 scale = {10,10};
};

struct TextElem {
	std::string content = "";
	bool visible = true;
};


/**
 * The following enumerators represent global identifiers refering to graphic
 * assets. For example TEXTURE_ASSET_ID are the identifiers of each texture
 * currently supported by the system.
 *
 * So, instead of referring to a game asset directly, the game logic just
 * uses these enumerators and the RenderRequest struct to inform the renderer
 * how to structure the next draw command.
 *
 * There are 2 reasons for this:
 *
 * First, game assets such as textures and meshes are large and should not be
 * copied around as this wastes memory and runtime. Thus separating the data
 * from its representation makes the system faster.
 *
 * Second, it is good practice to decouple the game logic from the render logic.
 * Imagine, for example, changing from OpenGL to Vulkan, if the game logic
 * depends on OpenGL semantics it will be much harder to do the switch than if
 * the renderer encapsulates all asset data and the game logic is agnostic to it.
 *
 * The final value in each enumeration is both a way to keep track of how many
 * enums there are, and as a default value to represent uninitialized fields.
 */

struct SpriteSheet {
	vec2 textureRes;
	vec2 spriteRes;
	int index = 0;
};

enum class TEXTURE_ASSET_ID {
	HEALTH_CONTAINER = 0,
	HEALTH_FILL = HEALTH_CONTAINER + 1,
	ANT = HEALTH_FILL + 1,
	SPIDER = ANT + 1,
	BEE_CHARGER = SPIDER + 1,
	BEE_RANGER = BEE_CHARGER + 1,
	BEE_SWARM = BEE_RANGER + 1,
	BEE_HIVE = BEE_SWARM + 1,
	BEE_LORD_CHARGE = BEE_HIVE +1,
	BEE_LORD_RANGE = BEE_LORD_CHARGE +1,
	BEE_STING = BEE_LORD_RANGE + 1,
	LEVEL_2_BUSH = BEE_STING + 1,
	LEVEL_2_TALL_TREE = LEVEL_2_BUSH + 1,
	LEVEL_2_SNAIL = LEVEL_2_TALL_TREE + 1,
	LEVEL_2_SNAIL_DET = LEVEL_2_SNAIL + 1,
	LEVEL_2_ACID2_DET = LEVEL_2_SNAIL_DET + 1,
	ACID_DEFAULT_TEXT = LEVEL_2_ACID2_DET + 1, 
	ACID_CHARGED_TEXT = ACID_DEFAULT_TEXT + 1, 
	TREE_BARK = ACID_CHARGED_TEXT + 1,
	TREE_LEAVES = TREE_BARK + 1,
	BUSH = TREE_LEAVES + 1,
	GRASS = BUSH + 1,
	GROUND = GRASS + 1,
	BACKGROUND_LIGHTS_1 = GROUND + 1,
	BACKGROUND_LIGHTS_2 = BACKGROUND_LIGHTS_1 + 1,
	BACKGROUND_TREE_NEAR = BACKGROUND_LIGHTS_2 + 1,
	BACKGROUND_TREE_MID = BACKGROUND_TREE_NEAR + 1,
	BACKGROUND_TREE_FAR = BACKGROUND_TREE_MID + 1,
	LEVEL_2_BG_3 = BACKGROUND_TREE_FAR + 1,
	LEVEL_2_BG_4 = LEVEL_2_BG_3 + 1, 
	LEVEL_2_BG_5 = LEVEL_2_BG_4 + 1, 
	LEVEL_2_BG_6 = LEVEL_2_BG_5 + 1,
	HELP_ICON = LEVEL_2_BG_6 + 1,
	HELP_DOC = HELP_ICON + 1,
	SPIDER_IDLE_EARS = HELP_DOC + 1,
	SPIDER_LEFT1 = SPIDER_IDLE_EARS + 1,
	SPIDER_LEFT2 = SPIDER_LEFT1 + 1,
	SPIDER_LEFT3 = SPIDER_LEFT2 + 1,
	SPIDER_RIGHT1 = SPIDER_LEFT3 + 1,
	SPIDER_RIGHT2 = SPIDER_RIGHT1 + 1,
	SPIDER_RIGHT3 = SPIDER_RIGHT2 + 1,
	CROSSHAIR = SPIDER_RIGHT3 + 1,
	ACID_WEB_SHOOT = CROSSHAIR + 1,
	GREEN_WEB = ACID_WEB_SHOOT + 1,
	ANT_XL = GREEN_WEB + 1,
	ANT_XS = ANT_XL + 1,
	ANT_IMMUNE = ANT_XS + 1,
	ANT_RESTORE = ANT_IMMUNE + 1,
	ANT_FAST = ANT_RESTORE+1,
    GAME_DROP_NAME = ANT_FAST + 1,
	TEXT_BEE_INTRO = GAME_DROP_NAME + 1,
	TEXT_LORD_BEE = TEXT_BEE_INTRO + 1,
	TEXT_STOP = TEXT_LORD_BEE + 1,
	TEXT_NO = TEXT_STOP + 1,
    GAME_INTRO_NAME = TEXT_NO + 1,
    TEXT_SAVED = GAME_INTRO_NAME + 1,
	TUTORIAL_WASD = TEXT_SAVED + 1,
	TUTORIAL_GREAT = TUTORIAL_WASD + 1,
	TUTORIAL_SHOOT = TUTORIAL_GREAT+1,
	TUTORIAL_BEES = TUTORIAL_SHOOT+1,
	TUTORIAL_CLIMB = TUTORIAL_BEES+1,
	TUTORIAL_DESTROY = TUTORIAL_CLIMB+1,
	TUTORIAL_SWING = TUTORIAL_DESTROY+1,
	TUTORIAL_ANTS = TUTORIAL_SWING+1,
	TUTORIAL_ANTINFO = TUTORIAL_ANTS+1,
	TUTORIAL_READY = TUTORIAL_ANTINFO+1,
	TUTORIAL_SKIP = TUTORIAL_READY+1,
	LEVEL1 = TUTORIAL_SKIP+1,
	LEVEL2 = LEVEL1+1,
	PROMPT_LEVEL2 = LEVEL2+1,
	PROMPT_ENDGAME = PROMPT_LEVEL2+1,
	TEXT = PROMPT_ENDGAME + 1,
	TEXTURE_COUNT = TEXT + 1
    };
const int texture_count = (int)TEXTURE_ASSET_ID::TEXTURE_COUNT;

enum class EFFECT_ASSET_ID {
	BEE_MESH = 0,
	SPIDER_MESH = BEE_MESH + 1,
	COLOURED = SPIDER_MESH + 1,
	DEBUG = COLOURED + 1,
	TEXTURED = DEBUG + 1,
	BACKGROUND = TEXTURED + 1,
	UI = BACKGROUND + 1,
	WATER = UI + 1,
	SPIDER = WATER + 1,
	ANIM_SPRITE = SPIDER +1,
	PARTICLE = ANIM_SPRITE + 1,
	ANIM_ELEMENT = PARTICLE + 1,
	EFFECT_COUNT = ANIM_ELEMENT + 1
};
const int effect_count = (int)EFFECT_ASSET_ID::EFFECT_COUNT;

enum class GEOMETRY_BUFFER_ID {
	BEE_MESH = 0, 
	SPIDER_MESH = BEE_MESH + 1,
	SALMON = SPIDER_MESH + 1,
	SPRITE = SPIDER_MESH + 1,
	PEBBLE = SPRITE + 1,
	DEBUG_LINE = SPRITE + 1,
	SCREEN_TRIANGLE = DEBUG_LINE + 1,
	POINT_PARTICLE = SCREEN_TRIANGLE + 1,
	GEOMETRY_COUNT = POINT_PARTICLE + 1
};
const int geometry_count = (int)GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;

struct Curssor {
	TEXTURE_ASSET_ID cursor_sprite = TEXTURE_ASSET_ID::CROSSHAIR;
};

struct RenderRequest {
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};

struct UIRenderRequest { //this holds the same kind of data as a normal RR, but by separating them we stop us from having to sort our RRs at every draw call.
	TEXTURE_ASSET_ID used_texture = TEXTURE_ASSET_ID::TEXTURE_COUNT;
	EFFECT_ASSET_ID used_effect = EFFECT_ASSET_ID::EFFECT_COUNT;
	GEOMETRY_BUFFER_ID used_geometry = GEOMETRY_BUFFER_ID::GEOMETRY_COUNT;
};