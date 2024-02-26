#include "world_init.hpp"
#include "tiny_ecs_registry.hpp"
#include "health_system.hpp"
#include <math_helpers.hpp>
#include <iostream>
Entity createCamera(int xMin, int xMax)
{
	auto entity = Entity();
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 0,0 };

	Camera& camera = registry.cameras.emplace(entity);//make this a camera so it can be referenced directly
	camera.xBound = { xMin, xMax };

	return entity;
}

Entity createBackground(RenderSystem* renderer, float depth)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	vec2 worldSize = renderer->getWindowSizeWorld();

	Motion& motion = registry.motions.emplace(entity);//give the background a position, will be useful for parallax...potentially
	motion.position.y = 0;// for backgrounds, we'll let y represent depth, unitl this causes an issue there's no reason to not, since backgrounds won't scroll up, only left/right
	motion.position.x = worldSize.x / 2;
	motion.scale = vec2(worldSize.x, 2 * worldSize.y);
	
	BackgroundDepth& bg_depth = registry.backgroundDepths.emplace(entity);
	bg_depth.depth = depth;

	if (depth >= 0) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND_TREE_NEAR,
				EFFECT_ASSET_ID::BACKGROUND,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (depth == -1) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND_TREE_MID,
				EFFECT_ASSET_ID::BACKGROUND,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::BACKGROUND_TREE_FAR,
				EFFECT_ASSET_ID::BACKGROUND,
				GEOMETRY_BUFFER_ID::SPRITE });
	}

	return entity;
}

Entity createBackgroundLevel2(RenderSystem* renderer, float depth)
{
	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	vec2 worldSize = renderer->getWindowSizeWorld();

	Motion& motion = registry.motions.emplace(entity);

	// the 400 here is because the original background isn't big enough and i don't want to enlarge it too much, so just 
	// offsetting it downwards 
	motion.position.y = 0 + 400;
	motion.position.x = worldSize.x / 2;
	//motion.scale = vec2(worldSize.x, 2 * worldSize.y);
	motion.scale = vec2(3.2* 512, 3.2*256);
	BackgroundDepth& bg_depth = registry.backgroundDepths.emplace(entity);
	bg_depth.depth = depth;

	if (depth == -3) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_2_BG_6,
				EFFECT_ASSET_ID::BACKGROUND,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (depth == -2) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_2_BG_5,
				EFFECT_ASSET_ID::BACKGROUND,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (depth == -1) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_2_BG_4,
				EFFECT_ASSET_ID::BACKGROUND,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (depth == 0) {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::LEVEL_2_BG_3,
				EFFECT_ASSET_ID::BACKGROUND,
				GEOMETRY_BUFFER_ID::SPRITE });
	}
	return entity;
} 


Entity createBush(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	vec2 worldSize = renderer->getWindowSizeWorld();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 1200, 1025 };


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BUSH,
			EFFECT_ASSET_ID::BACKGROUND,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createGrass(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 1200, 100 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GRASS,
			EFFECT_ASSET_ID::BACKGROUND,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createGround(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 2.0 * 1200, 2.0 * 95 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GROUND,
			EFFECT_ASSET_ID::BACKGROUND,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLights1(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 1200, 1025 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BACKGROUND_LIGHTS_1,
			EFFECT_ASSET_ID::BACKGROUND,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLights2(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 1200, 1025 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BACKGROUND_LIGHTS_2,
			EFFECT_ASSET_ID::BACKGROUND,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHealthContainer(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);

	motion.position = { 100.f, 40.f };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 1.5 * HEALTH_CONTAINER_WIDTH, 1.5 * HEALTH_CONTAINER_HEIGHT });
	registry.healthContainer.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEALTH_CONTAINER,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHealthFill(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);

	motion.position = { 100.f, 40.f };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 1.5 * HEALTH_CONTAINER_WIDTH, 1.5 * HEALTH_CONTAINER_HEIGHT });
	auto& health_bar_prop = registry.healthBar.emplace(entity);
	health_bar_prop.pos = { 100.f, 40.f };
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEALTH_FILL,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createSting(RenderSystem* renderer, vec2 spawn_pos, Entity target)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	TrackingProjectile& tracking = registry.trackingProjectile.emplace(entity);
	tracking.disappear_timer = 6000;
	tracking.projectile_speed = 275;
	tracking.target = target;

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);

	motion.position = spawn_pos;
	motion.angle = 0.f;
	motion.velocity = { 106, 106 };
	motion.scale = { 100, 100 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BEE_STING,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}



Entity createSpider(RenderSystem* renderer)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPIDER_MESH);
	registry.meshPtrs.emplace(entity, &mesh);

	PlayerStatus& player_status = registry.playerStatus.emplace(entity);
	player_status.movespeed = 200;
	player_status.reload_time_preset = 500;
	player_status.reload_timer = 0;
	player_status.webshoot_speed = 400;

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { SPIDER_DEFAULT_X, SPIDER_DEFAULT_Y };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	//motion.scale = mesh.original_size * 175.f;
	motion.scale = { 150, 100 };
	motion.scale.x *= -1; // point front to the right

	auto& damageable = registry.damageable.emplace(entity);
	damageable.takeDamage = &(HealthSystem::takeDamage);

	// Create and (empty) Spider component to be able to refer to all turtles
	registry.players.emplace(entity);
	
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::SPIDER,
			EFFECT_ASSET_ID::SPIDER,
			GEOMETRY_BUFFER_ID::SPRITE });
	registry.animation.emplace(entity);
	return entity;

}

Entity createSnail(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -100, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 0.3*462, 0.3*407 });

	//registry.enemy.emplace(entity);
	registry.indestructibleEnemy.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_2_SNAIL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createRegularAnt(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 2.5f * ANT_WIDTH, 2.5f * ANT_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	registry.friendly.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ANT,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createXLAnt(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 2.5f * ANT_WIDTH, 2.5f * ANT_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	registry.friendly.emplace(entity);
	registry.xlAnt.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ANT_XL,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createFastAnt(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 2.5f * ANT_WIDTH, 2.5f * ANT_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	registry.friendly.emplace(entity);
	registry.fastAnt.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ANT_FAST,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createXSAnt(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 2.5f * ANT_WIDTH, 2.5f * ANT_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	registry.friendly.emplace(entity);
	registry.xsAnt.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ANT_XS,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createRestoreHealthAnt(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 2.5f * ANT_WIDTH, 2.5f * ANT_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	registry.friendly.emplace(entity);
	registry.healthAnt.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ANT_RESTORE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createImmuneAnt(RenderSystem* renderer, vec2 position)
{
	// Reserve en entity
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the position, scale, and physics components
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -50, 0 };
	motion.position = position;

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 2.5f * ANT_WIDTH, 2.5f * ANT_HEIGHT });

	// Create an (empty) Fish component to be able to refer to all fish
	registry.friendly.emplace(entity);
	registry.immuneAnt.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ANT_IMMUNE,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBee(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	if (position.y > 350.f) 
	{
		entity = createChargerBee(renderer, position);
	}
	else if (position.y < 275.f)
	{
		entity = createRangerBee(renderer, position);
	}
	else
	{
		if (rand()%2 > 1)
			entity = createChargerBee(renderer, position);
		else 
			entity = createRangerBee(renderer, position);
	}

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { -100.f, 0.f };
	motion.position = position;


	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ BEE_BB_WIDTH, BEE_BB_HEIGHT });

	registry.enemy.emplace(entity);

	return entity;
}
/*
// doing it for normal bee only 
Entity createBeeMesh(RenderSystem* renderer, vec2 position, Entity bee)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BEE_MESH);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = position;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = mesh.original_size * 175.f;
	motion.scale.y *= -1; // point front to the right

	// Initializing the AI stuff now 
	auto& enemy_ai = registry.aidecisions.get(bee);

	auto& enemy_mesh_ai = registry.aiMeshDecisions.emplace(entity);
	enemy_mesh_ai.bee = bee;

	enemy_mesh_ai.ai_type = enemy_ai.ai_type;
	enemy_mesh_ai.ai_state = enemy_ai.ai_state;
	enemy_mesh_ai.alert_range = enemy_ai.alert_range;
	enemy_mesh_ai.attack_range = enemy_ai.attack_range;
	enemy_mesh_ai.speed = enemy_ai.speed;
	enemy_mesh_ai.alert_speed_factor = enemy_ai.alert_speed_factor;
	enemy_mesh_ai.attack_speed_factor = enemy_ai.attack_speed_factor;
	enemy_mesh_ai.idle_reset_time = enemy_ai.idle_reset_time;
	enemy_mesh_ai.alert_reset_time = enemy_ai.alert_reset_time;
	enemy_mesh_ai.post_attack_reset_time = enemy_ai.post_attack_reset_time;

	registry.beeMesh.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::BEE_MESH,
		 GEOMETRY_BUFFER_ID::BEE_MESH });

	return entity;
}
*/


Entity createBossBee(RenderSystem* renderer, vec2 screen_size, vec2 position)
{

	auto entity = Entity();
	bool randombool = 0;
	randombool = rand() > (RAND_MAX / 2);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	// TODO
	//motion.position = vec2(screen_size.x / 2, 400);
	motion.position = vec2(position.x, position.y - 400);

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BEE_MESH);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& boss = registry.bossStatus.emplace(entity);
	boss.health = 600;
	boss.default_pos = vec2(position.x, position.y - 400);

	boss.boss_mode = BOSS_MODE::CHARGE_MODE;

	boss.talk_interval = 6000;
	boss.talk_timer = 0;
	boss.talk_duration = 3000;
	boss.intro_talk = 5000;
	boss.talk = false;

	boss.charge_range = 300;
	boss.range_range = 400;
	boss.evasion_range = 100;
	boss.evasion_cd_timer = 2000;
	boss.evasion_cd_time = 3000;

	boss.charge_attack_cd_timer = 0;
	boss.charge_attack_cd_time = 5000;

	boss.state_switch_timer = 1000;
	boss.evasion_time = 1000;
	boss.idle_track_time = 1000;
	boss.idle_wander_time = 2000;
	boss.charge_attack_time = 1200;
	boss.range_attack_time = 1000;
	boss.post_attack_wait_time = 1000;
	boss.post_evasion_wait_time = 500;

	boss.charge_mode_speed = 60;
	boss.charge_attack_speed = 300;
	boss.range_mode_speed = 30;
	boss.evasion_speed = 300;

	boss.range_attack_counter = 0;
	boss.range_attack_cd_timer = 0;
	boss.range_attack_cd_time = 1500;

	boss.mode_change_timer = 50000;
	boss.mode_change_time = 50000;

	boss.death_timer = 5000;

	Entity healthbar = createBossHealth(renderer, screen_size, boss.health);

	boss.healthbar = healthbar;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BEE_LORD_CHARGE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 150, 150 });

	registry.enemy.emplace(entity);

	return entity;
}

Entity createBossHealth(RenderSystem* renderer, vec2 screen_size, float boss_max_health) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);

	motion.position = { screen_size.x / 2, screen_size.y - 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = vec2({ 500, 100 });
	auto& healthbar = registry.bossHealthBar.emplace(entity);
	healthbar.boss_max_health = boss_max_health;
	healthbar.healthbar_default_scale = 500;
	healthbar.position = { screen_size.x / 2, screen_size.y - 100 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HEALTH_FILL,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createChargerBee(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BEE_MESH);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& enemy_ai = registry.aidecisions.emplace(entity);
	enemy_ai.ai_type = ENEMY_TYPE::BEE_CHARGER;
	enemy_ai.ai_state = ENEMY_STATE::IDLE;
	enemy_ai.alert_range = 800.f;
	enemy_ai.attack_range = 600.0f;
	enemy_ai.speed = 100;
	enemy_ai.alert_speed_factor = 0.7;
	enemy_ai.attack_speed_factor = 3.0;
	enemy_ai.idle_reset_time = 2000;
	enemy_ai.alert_reset_time = 1000;
	enemy_ai.post_attack_reset_time = 500;

	Damageable& damageable = registry.damageable.emplace(entity);
	damageable.maxHealth = 30;
	damageable.health = 30;
	damageable.takeDamage = HealthSystem::takeDamage;


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BEE_CHARGER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createRangerBee(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BEE_MESH);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& enemy_ai = registry.aidecisions.emplace(entity);
	enemy_ai.ai_type = ENEMY_TYPE::BEE_RANGER;
	enemy_ai.ai_state = ENEMY_STATE::IDLE;
	enemy_ai.alert_range = 800.f;
	enemy_ai.attack_range = 800.0f;
	enemy_ai.speed = 150;
	enemy_ai.alert_speed_factor = 0.8;
	enemy_ai.attack_speed_factor = 1.0;
	enemy_ai.idle_reset_time = 4000;
	enemy_ai.alert_reset_time = 500;
	enemy_ai.post_attack_reset_time = 4000;

	Damageable& damageable = registry.damageable.emplace(entity);
	damageable.maxHealth = 20;
	damageable.health = 20;
	damageable.takeDamage = HealthSystem::takeDamage;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BEE_RANGER,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createSwarmBee(RenderSystem* renderer, vec2 position)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BEE_MESH);
	registry.meshPtrs.emplace(entity, &mesh);
	auto& swarm = registry.beeSwarm.emplace(entity);
	swarm.swarm_speed = 100;
	swarm.visualRange = 50;
	swarm.centeringFactor = 0.01;
	swarm.avoidFactor = 0.05;
	swarm.matchingFactor = 0.005;
	swarm.minDistance = 40;
	swarm.spawn_point = position;


	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = normalize(vec2(rand()%11 - 5,rand()%11 - 5)) * swarm.swarm_speed;
	motion.position = position;
	motion.scale = swarm.scale;


	registry.enemy.emplace(entity);

	

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BEE_SWARM,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBeeHive(RenderSystem* renderer, vec2 position, Entity tree_ref)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::BEE_MESH);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = position;
	motion.scale = vec2({ 150, 150 });

	registry.enemy.emplace(entity);

	auto& hive = registry.beeHive.emplace(entity);
	hive.spawn_time = 1000;
	hive.spawn_timer = 500;
	hive.swarm_count = 0;
	hive.max_swarm_count = 10;
	hive.tree_ref = tree_ref;
	
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::BEE_HIVE,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTreeBark(RenderSystem* renderer, float x)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = { x, 550 };

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -225, 300 });
	registry.climbable.emplace(entity);

	auto& damageable = registry.damageable.emplace(entity);
	damageable.health = 80;
	damageable.maxHealth = 80;
	damageable.takeDamage = HealthSystem::takeDamageObstacle;

	auto& spriteSheet = registry.spriteSheets.emplace(entity);
	spriteSheet.textureRes = { 800, 255 }; //can these be not hard coded? I dont think so, unless we can somehow embed them in the texture itself.
	spriteSheet.spriteRes = { 200, 255 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TREE_BARK,
		 EFFECT_ASSET_ID::ANIM_SPRITE,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTreeLeaves(RenderSystem* renderer, float x)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = { x, 500 };

	auto& damageable = registry.damageable.emplace(entity);
	damageable.health = 80;
	damageable.maxHealth = 80;
	damageable.takeDamage = HealthSystem::takeDamageObstacle;

	auto& spriteSheet = registry.spriteSheets.emplace(entity);
	spriteSheet.textureRes = { 1000, 330 }; //can these be not hard coded? I dont think so, unless we can somehow embed them in the texture itself.
	spriteSheet.spriteRes = { 250, 330 };

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ -250, 400 });
	registry.climbable.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TREE_LEAVES,
		 EFFECT_ASSET_ID::ANIM_SPRITE,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}


Entity createBushLevel2(RenderSystem* renderer, float x)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = { x, 630 };

	auto& damageable = registry.damageable.emplace(entity);
	damageable.health = 50;
	damageable.maxHealth = 50;
	damageable.takeDamage = HealthSystem::takeDamageNonClimbableObstacle;

	auto& unclimbable = registry.unclimbable.emplace(entity);

	motion.scale = vec2({ 2.0*82, 2.0*53 });
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_2_BUSH,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTallTreeLevel2(RenderSystem* renderer, float x)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = { x, 400 };

	auto& damageable = registry.damageable.emplace(entity);
	damageable.health = 180;
	damageable.maxHealth = 180;
	damageable.takeDamage = HealthSystem::takeDamageObstacle;

	auto& spriteSheet = registry.spriteSheets.emplace(entity);
	spriteSheet.textureRes = { 1261, 579 }; //can these be not hard coded? I dont think so, unless we can somehow embed them in the texture itself.
	spriteSheet.spriteRes = { 315, 579 };

	// Setting initial values, scale is negative to make it face the opposite way
	motion.scale = vec2({ 315, 579 });
	registry.climbable.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_2_TALL_TREE,
		 EFFECT_ASSET_ID::ANIM_SPRITE,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHelp(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 100, 50 };


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_ICON,
			EFFECT_ASSET_ID::TEXTURED,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createHelpDoc(RenderSystem* renderer, vec2 pos)
{
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 550, 600 };
	registry.helpDoc.emplace(entity);


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::HELP_DOC,
			EFFECT_ASSET_ID::UI,
			GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createGameIntro(RenderSystem* renderer)
{
    auto entity = Entity();

    // Store a reference to the potentially re-used mesh object
    Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
    registry.meshPtrs.emplace(entity, &mesh);

    // Setting initial motion values
    Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 150 };
    motion.angle = 0.f;
    motion.velocity = { 0.f, 0.f };
    motion.scale = { 600, 300 };

    registry.renderRequests.insert(
            entity,
            { TEXTURE_ASSET_ID::GAME_INTRO_NAME,
              EFFECT_ASSET_ID::UI,
              GEOMETRY_BUFFER_ID::SPRITE });

    return entity;
}

Entity createWASDInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_WASD,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createShootInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_SHOOT,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createBeeInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_BEES,
		  EFFECT_ASSET_ID::UI,  
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createClimbInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_CLIMB,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createAntInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_ANTS,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createAntDetails(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 150 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 300 };


	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_ANTINFO,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createSwingInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_SWING,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createGreatInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_GREAT,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createReady(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_READY,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createSkip(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 1125, 50 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 100, 50 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_SKIP,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createKeepDestroyingInstructions(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TUTORIAL_DESTROY,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLevel2Prompt(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PROMPT_LEVEL2,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}


Entity createLevel2SnailDetail(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };
	//registry.levels.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_2_SNAIL_DET,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createLevel2Acid2Detail(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };
//	registry.levels.emplace(entity);
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL_2_ACID2_DET,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createEndOfGamePrompt(RenderSystem* renderer, vec2 pos) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::PROMPT_ENDGAME,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}
Entity createLevel1(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };
	registry.levels.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL1,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createLevel2(RenderSystem* renderer) {
	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = { 625, 100 };
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 600, 200 };
	registry.levels.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::LEVEL2,
		  EFFECT_ASSET_ID::UI,
		  GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createCursor(RenderSystem* renderer) {

	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.position = { 0,0 };

	motion.scale = vec2(100, 100);

	Curssor& cursor = registry.cursor.emplace(entity);
	cursor.cursor_sprite = TEXTURE_ASSET_ID::CROSSHAIR;
	CursorGesture& cursorGesture = registry.cursorGesture.emplace(entity);
	cursorGesture.disappear_timer = 500;

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::CROSSHAIR,
		 EFFECT_ASSET_ID::UI,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createWebShoot(RenderSystem* renderer, vec2 spider_pos, vec2 target_pos, float speed) {

	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);

	float angle = look_toward_angle(target_pos, spider_pos);
	vec2 velocity = move_toward(target_pos, spider_pos);
	motion.angle = angle;
	motion.velocity = velocity * speed;
	motion.position = spider_pos;

	motion.scale = vec2(40, 30);

	Curssor& cursor = registry.cursor.emplace(entity);

	WebShoot& webshoot = registry.webshoots.emplace(entity);
	float time = pos_distance(spider_pos, target_pos) / (speed / 1000) + 100;
	webshoot.disappear_timer = time;
	webshoot.target_pos = target_pos;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACID_WEB_SHOOT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createBigWebShoot(RenderSystem* renderer, vec2 spider_pos, vec2 target_pos, float speed,
						float buildUpFactor) {

	auto entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	// Initialize the motion
	auto& motion = registry.motions.emplace(entity);

	float angle = look_toward_angle(target_pos, spider_pos);
	vec2 velocity = move_toward(target_pos, spider_pos);
	motion.angle = angle;
	motion.velocity = velocity * speed;
	motion.position = spider_pos;

	float maxFactor = 7.0;
	if (buildUpFactor <= maxFactor && buildUpFactor >= 1.0) {
		vec2 scaleFactor = vec2(0.5 * buildUpFactor, 0.5 * buildUpFactor);
		motion.scale = scaleFactor * vec2(40, 30);
	} else if (buildUpFactor > maxFactor){
		vec2 scaleFactor = vec2(0.5 * maxFactor, 0.5 * maxFactor);
		motion.scale = scaleFactor * vec2(40, 30);
	}
	else if (buildUpFactor < 1.0) {
		motion.scale = vec2(40, 30);
	}
	

	Curssor& cursor = registry.cursor.emplace(entity);

	WebShoot& webshoot = registry.webshoots.emplace(entity);
	float time = pos_distance(spider_pos, target_pos) / (speed / 1000) + 100;
	webshoot.disappear_timer = time * 50.f;
	webshoot.target_pos = target_pos;

	if (buildUpFactor <= maxFactor) {
		webshoot.damage += 5* buildUpFactor;
	}
	else {
		webshoot.damage += 5* maxFactor;
	}

	std::cout << "webshoot dmage" << webshoot.damage << std::endl;
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::ACID_WEB_SHOOT,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;

}

Entity createWebShootResidual(RenderSystem* renderer, vec2 pos) {

	auto entity = Entity();

	Mesh& mesh = renderer->getMesh(GEOMETRY_BUFFER_ID::SPRITE);
	registry.meshPtrs.emplace(entity, &mesh);

	auto& motion = registry.motions.emplace(entity);

	motion.angle = 0;
	motion.velocity = { 0,0 };
	motion.position = pos;

	motion.scale = vec2(100, 120);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::GREEN_WEB,
		 EFFECT_ASSET_ID::TEXTURED,
		 GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createLine(vec2 position, vec2 scale)
{
	Entity entity = Entity();

	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::DEBUG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE });

	// Create motion
	Motion& motion = registry.motions.emplace(entity);
	motion.angle = 0.f;
	motion.velocity = { 0, 0 };
	motion.position = position;
	motion.scale = scale;

	registry.debugComponents.emplace(entity);
	return entity;
}

Entity createLineToTarget(vec2 target_pos, vec2 origin_pos, float width)
{
	Entity entity = Entity();
	// Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
	registry.renderRequests.insert(
		entity,
		{TEXTURE_ASSET_ID::TEXTURE_COUNT,
		 EFFECT_ASSET_ID::DEBUG,
		 GEOMETRY_BUFFER_ID::DEBUG_LINE});

	float angle = look_toward_angle(target_pos, origin_pos);
	auto& webstring = registry.webString.emplace(entity);
	float length = pos_distance(target_pos, origin_pos);
	vec2 scale = vec2(length, 5);
	auto &motion = registry.motions.emplace(entity);
	Entity cam = registry.cameras.entities[0];
	Motion &cam_motion = registry.motions.get(cam);
	motion.angle = angle;
	vec2 offset = vec2(cos(angle) * length / 2, sin(angle) * length / 2);
	motion.position += target_pos - offset -cam_motion.position;
	motion.scale = scale;
	webstring.angle = angle;
	registry.debugComponents.emplace(entity);

	return entity;
}



Entity createTEXT(TEXT_ASSETS content, vec2 pos, float size, float duration)
{
	auto entity = Entity();

	// Setting initial motion values
	Motion& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.angle = 0.f;
	motion.velocity = { 0.f, 0.f };
	motion.scale = { 5* size, size };
	if (duration > 0) {
		auto& text = registry.texts.emplace(entity);
		text.remove_timer = duration;
	}
	// if size passed in is 0, i'm going to manually set the text size 
	// in this case, only ACID_CHARGED and ACID_DEFAULT are using 0 
	if (size == 0) {
		motion.scale = {0.8* 236, 0.8 * 19};
	}
	switch (content) {
	case TEXT_ASSETS::BEE_INTRO: {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TEXT_BEE_INTRO,
				EFFECT_ASSET_ID::UI,
				GEOMETRY_BUFFER_ID::SPRITE });

	}break;
	case TEXT_ASSETS::STOP: {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TEXT_STOP,
				EFFECT_ASSET_ID::UI,
				GEOMETRY_BUFFER_ID::SPRITE });

	}break;
	case TEXT_ASSETS::NO: {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TEXT_NO,
				EFFECT_ASSET_ID::UI,
				GEOMETRY_BUFFER_ID::SPRITE });

	}break;
    case TEXT_ASSETS::SAVED: {
        registry.renderRequests.insert(
                entity,
                { TEXTURE_ASSET_ID::TEXT_SAVED,
                  EFFECT_ASSET_ID::UI,
                  GEOMETRY_BUFFER_ID::SPRITE });
    }break;
	case TEXT_ASSETS::LORD_BEE: {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::TEXT_LORD_BEE,
			  EFFECT_ASSET_ID::UI,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}break;
	case TEXT_ASSETS::ACID_DEFAULT: {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ACID_DEFAULT_TEXT,
			  EFFECT_ASSET_ID::UI,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}break;
	case TEXT_ASSETS::ACID_CHARGED: {
		registry.renderRequests.insert(
			entity,
			{ TEXTURE_ASSET_ID::ACID_CHARGED_TEXT,
			  EFFECT_ASSET_ID::UI,
			  GEOMETRY_BUFFER_ID::SPRITE });
	}break;
	default: break;
	}
	return entity;
}


Entity createPointEmitter(vec2 pos, vec2 direction, ParticleType type) {
	auto entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;

	auto& emitter = registry.emitters.emplace(entity);
	
	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		EFFECT_ASSET_ID::PARTICLE,
		GEOMETRY_BUFFER_ID::POINT_PARTICLE});

	switch (type) {
	case ParticleType::TREE:
		emitter.colour = { 105 / 255.f, 89 / 255.f, 71 / 255.f };
		for (int i = 0; i < 10; i++) {
			emitter.particles.push_back(createHitParticle(pos, direction));
		}
		break;
	case ParticleType::GROUND:
		emitter.colour = { 105, 89, 71 };
		break;
	case ParticleType::BEE:
		emitter.colour = { 0.91, 0.88, 0.51 };
		for (int i = 0; i < 5; i++) {
			emitter.particles.push_back(createHitParticle(pos, direction));
		}
		break;
	default:
		assert(false);
	}
	return entity;
}

Entity createAreaEmitter(vec2 pos, vec2 scale, ParticleType type) {
	auto entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;
	motion.scale = scale;

	auto& emitter = registry.emitters.emplace(entity);

	registry.renderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXTURE_COUNT,
		EFFECT_ASSET_ID::PARTICLE,
		GEOMETRY_BUFFER_ID::POINT_PARTICLE });

	switch (type) {
	case ParticleType::TREE:
		emitter.colour = { 105 / 255.f, 89 / 255.f, 71 / 255.f };
		for (int i = 0; i < 20; i++) {
			float rand1 = ((float)rand() / RAND_MAX)*2 - 1 ;
			float rand2 = ((float)rand() / RAND_MAX)*2 - 1;
			vec2 particle_pos = {pos.x + (rand1 * scale.x / 2), pos.y + (rand2 * scale.y / 2)}; // random position inside area bounded
			emitter.particles.push_back(createHitParticle(particle_pos, vec2(0,-1)));
		}
		break;
	case ParticleType::GROUND:
		emitter.colour = { 105, 89, 71 };
		break;
	case ParticleType::BEE:
		emitter.colour = { 0,0,0 };
		for (int i = 0; i < 5; i++) {
			float rand1 = ((float)rand() / RAND_MAX);
			float rand2 = ((float)rand() / RAND_MAX);
			vec2 particle_pos = { pos.x + rand1, pos.y + rand2 };
			emitter.particles.push_back(createHitParticle(particle_pos, vec2(0, -1)));
		}
		break;
	default:
		assert(false);
	}
	return entity;
}

Entity createHitParticle(vec2 pos, vec2 direction) {
	auto entity = Entity();

	auto& motion = registry.motions.emplace(entity);
	motion.position = pos;
	float angle = atan2f(direction.y, direction.x);
	float random = ((float) rand() / RAND_MAX) * 2* M_PI;
	angle = (angle / 2.f) + (random * 2.f/ 4.f); //slightly random angle in direction 2 halves random, 1 quarter up, and 1 quarter in the direction
	motion.velocity = mat2({ cos(angle), sin(angle) }, { -sin(angle), cos(angle) }) * vec2(100, 0);

	auto& particle = registry.particles.emplace(entity);
	particle.enabled = true;

	auto& physics = registry.physics.emplace(entity);
	physics.gravity_enabled = true;
	physics.collisions_enabled = false;
	physics.terminal_speed = 300;
	physics.mass = 10;

	return entity;
}

Entity createTimer(vec2 pos, vec2 scale) {
	auto entity = Entity();

	auto& uiPos = registry.uiPositions.emplace(entity);
	uiPos.position = pos;
	uiPos.scale = scale;

	auto& stopwatch = registry.stopwatches.emplace(entity);

	SpriteSheet& spriteSheet = registry.spriteSheets.emplace(entity);
	spriteSheet.spriteRes = { 45,65 };
	spriteSheet.textureRes = {720, 520};

	registry.uiRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXT,
		EFFECT_ASSET_ID::ANIM_ELEMENT,
		GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}

Entity createTextUIElem(vec2 pos, vec2 scale, std::string text) {
	auto entity = Entity();

	auto& uiPos = registry.uiPositions.emplace(entity);
	uiPos.position = pos;
	uiPos.scale = scale;

	SpriteSheet& spriteSheet = registry.spriteSheets.emplace(entity);
	spriteSheet.spriteRes = { 45,65 };
	spriteSheet.textureRes = { 720, 520 };

	TextElem& textElem = registry.textElements.emplace(entity);
	textElem.content = text;
	textElem.visible = true;

	registry.uiRenderRequests.insert(
		entity,
		{ TEXTURE_ASSET_ID::TEXT,
		EFFECT_ASSET_ID::ANIM_ELEMENT,
		GEOMETRY_BUFFER_ID::SPRITE });

	return entity;
}