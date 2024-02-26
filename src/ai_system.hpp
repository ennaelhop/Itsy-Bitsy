#pragma once

#include <vector>

#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include <random>

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class AISystem
{
public:
	void step(float elapsed_ms);

	// Parameters

	std::default_random_engine rng;
	std::uniform_real_distribution<float> uniform_dist;

private:
	float elapsed_ms_since_last_update;
	float boss_update_interval = 1000;
	float temp_boss_switcher = 0;
	
	
	void enemy_decision(Entity enemy, Motion player_motion, Entity player_entity);
	void idle(Entity enemy, Motion player_motion);
	void alerted(Entity enemy, Motion player_motion);
	void attack(Entity enemy);
	void projectile_attack(Entity enemy, Entity player);
	void post_attack(Entity enemy);
	void projectile_update(float elapsed_ms);

	//Hive & Swarm
	void hive_timer_update(Entity entity, float elapsed_ms);
	void swarm_behavior_update(Entity enemy);

	// BOSS AI
	void boss_update(Entity enemy, Entity player_entity, float elapsed_ms);
	void boss_bee_idle_track(Entity enemy, Entity player);
	void boss_bee_idle_wander(Entity enemy, Entity player);
	void boss_bee_evasion(Entity enemy);
	void boss_bee_charge_attack(Entity enemy);
	void boss_bee_range_attack(Entity enemy);
	void boss_bee_move_to_pos(Entity enemy);
	void boss_bee_transform(Entity enemy);
	void boss_freeze(Entity enemy, float freeze_time, bool restore);
	void boss_health_render(Entity enemy);
	void boss_death(Entity enemy, float elapsed_ms);
	void boss_text(Entity enemy, float elapsed_ms);
	

	void create_enemy_tracking_projectile(vec2 position, Entity target, Entity enemy);
};