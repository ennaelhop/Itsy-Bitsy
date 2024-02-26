// internal
#include "ai_system.hpp"
#include <iostream>
#include <math_helpers.hpp>
#include <world_init.hpp>
#include <ai_decisionTree.hpp>
#include <ai_swarm.hpp>

void AISystem::step(float elapsed_ms)
{
	elapsed_ms_since_last_update = elapsed_ms;
	Entity player = registry.players.entities[0];
	Motion player_motion = registry.motions.get(player);
	// Enemy statemachine
	for (int i = 0; i < registry.enemy.entities.size(); i++) {
		Entity enemy = registry.enemy.entities[i];

		if (registry.aidecisions.has(enemy)) {
			Motion& enemy_motion = registry.motions.get(enemy);

			enemy_decision(enemy, player_motion, player);
		}
		if (registry.beeHive.has(enemy)) {
			hive_timer_update(enemy, elapsed_ms);
		}
		if (registry.beeSwarm.has(enemy)) {
			swarm_behavior_update(enemy);
		}


	}

	projectile_update(elapsed_ms);

	if (registry.bossStatus.entities.size() > 0) {
		Entity boss_entity = registry.bossStatus.entities[0];
		BossStatus& boss_status = registry.bossStatus.get(boss_entity);
		boss_status.state_switch_timer -= elapsed_ms;

		// Make new decision
		if (boss_status.state_switch_timer < 0) {
			boss_bee_decision(boss_entity);
		}


		boss_update(boss_entity, player, elapsed_ms);
	}



}

void AISystem::swarm_behavior_update(Entity enemy) {
	auto& swarm_motion = registry.motions.get(enemy);
	auto& swarm = registry.beeSwarm.get(enemy);
	
	// Facing direction
	if (swarm_motion.velocity.x < 0) {
		swarm_motion.scale.x = -swarm.scale.x;
	}
	else {
		swarm_motion.scale.x = swarm.scale.x;
	}

	if (!swarm.hive_alive) return;
	flyTowardsCenter(enemy);
	avoidOthers(enemy);
	matchVelocity(enemy);
	limitSpeed(enemy);
	keepWithinBounds(enemy);

}


void AISystem::hive_timer_update(Entity entity, float elapsed_ms) {
	auto& hive = registry.beeHive.get(entity);
	hive.spawn_timer -= elapsed_ms;
	if (!registry.motions.has(hive.tree_ref)) {
		destroyHive(entity);
	}
}


void AISystem::enemy_decision(Entity enemy, Motion player_motion, Entity player_entity) {

	//Regular AI
	if (registry.aidecisions.has(enemy)) {
		EnemyDecision& ai = registry.aidecisions.get(enemy);
		if (ai.ai_state == ENEMY_STATE::IDLE) {
			idle(enemy, player_motion);
		}
		else if (ai.ai_state == ENEMY_STATE::ALERT) {
			alerted(enemy, player_motion);
		}
		else if (ai.ai_state == ENEMY_STATE::ATTACK) {
			if (ai.ai_type == ENEMY_TYPE::BEE_CHARGER) {
				attack(enemy);
			}
			if (ai.ai_type == ENEMY_TYPE::BEE_RANGER) {
				projectile_attack(enemy, player_entity);
			}

		}
		else if (ai.ai_state == ENEMY_STATE::POST_ATTACK) {
			post_attack(enemy);
		}
	}

}

void AISystem::boss_update(Entity enemy, Entity player_entity, float elapsed_ms) {
	//BOSS AI
	if (registry.bossStatus.has(enemy)) {
		BossStatus& boss_status = registry.bossStatus.get(enemy);
		BOSS_STATE boss_state = boss_status.boss_state;
		boss_health_render(enemy);
		
		if (boss_status.health <= 0) {
			boss_death(enemy, elapsed_ms);
			return;
		}
		boss_text(enemy, elapsed_ms);

		boss_status.mode_change_timer -= elapsed_ms;
		if (boss_status.mode_change_timer <= 0) {
			boss_bee_transform(enemy);
			boss_status.mode_change_timer = boss_status.mode_change_time;
		}
		switch (boss_state)
		{
		case BOSS_STATE::IDLE_TRACK: {
			boss_status.charge_attack_cd_timer -= elapsed_ms;
			boss_status.evasion_cd_timer -= elapsed_ms;
			boss_bee_idle_track(enemy, player_entity);

		} break;
		case BOSS_STATE::IDLE_WANDER: {
			boss_status.range_attack_cd_timer -= elapsed_ms;
			boss_status.evasion_cd_timer -= elapsed_ms;
			boss_bee_idle_wander(enemy, player_entity);
		} break;
		case BOSS_STATE::EVADE: boss_bee_evasion(enemy); break;
		case BOSS_STATE::CHARGE_ATTACK: boss_bee_charge_attack(enemy); break;
		case BOSS_STATE::RANGE_ATTACK: {
			boss_status.range_attack_cd_timer -= elapsed_ms;
			boss_bee_range_attack(enemy);
		}break;
		case BOSS_STATE::FREEZE: break;
		case BOSS_STATE::RETURN: boss_bee_move_to_pos(enemy); break;
		default:  boss_bee_idle_track(enemy, player_entity); break;
		}
	}
}

void AISystem::boss_death(Entity enemy, float elapsed_ms) {
	auto& boss = registry.bossStatus.get(enemy);
	auto& boss_motion = registry.motions.get(enemy);
	boss.death_timer -= elapsed_ms;
	if (boss.death_timer > 0) {
		createTEXT(TEXT_ASSETS::NO, boss_motion.position + vec2{0, -100}, 20, 100);
		boss_motion.velocity = vec2(0, 10);
		boss_motion.angle = 3.14;
	}
	else {
		registry.remove_all_components_of(boss.healthbar);
		registry.remove_all_components_of(enemy);
	}
	
}

void AISystem::boss_text(Entity enemy, float elapsed_ms) {
	auto& boss = registry.bossStatus.get(enemy);
	auto& boss_motion = registry.motions.get(enemy);
	if (boss.intro_talk >= 0) {
		boss.intro_talk -= elapsed_ms;
		createTEXT(TEXT_ASSETS::BEE_INTRO, boss_motion.position + vec2{ 0, -100 }, 60, 20);
		return;
	}
	boss.talk_timer -= elapsed_ms;
	if (boss.talk_timer < 0) {
		if (boss.talk) {
			boss.talk_timer = boss.talk_interval;
		}
		else {
			boss.talk_timer = boss.talk_duration;
		}
		boss.talk = !boss.talk;
		bool randombool = 0;
		randombool = rand() > (RAND_MAX / 2);
		boss.random_talk = randombool;
	}

	if(boss.talk)
	{
		switch (boss.boss_state)
		{
		case BOSS_STATE::IDLE_TRACK: {
			if (boss.random_talk) {
				createTEXT(TEXT_ASSETS::BEE_INTRO, boss_motion.position + vec2{ 0, -100 }, 60, 20);
			}
			else {
				createTEXT(TEXT_ASSETS::STOP, boss_motion.position + vec2{ 0, -100 }, 20, 20);
			}

		} break;
		case BOSS_STATE::IDLE_WANDER: {
			if (boss.random_talk) {
				createTEXT(TEXT_ASSETS::BEE_INTRO, boss_motion.position + vec2{ 0, -100 }, 60, 20);
			}
			else {
				createTEXT(TEXT_ASSETS::STOP, boss_motion.position + vec2{ 0, -100 }, 20, 20);
			}
		} break;
		case BOSS_STATE::EVADE: {
			if (boss.talk_timer > boss.state_switch_timer) {
				createTEXT(TEXT_ASSETS::STOP, boss_motion.position + vec2{ 0, -100 }, 20, 20);
			}
		}break;
		case BOSS_STATE::CHARGE_ATTACK: break;
		case BOSS_STATE::RANGE_ATTACK: {

		}break;
		case BOSS_STATE::FREEZE: {
		} break;
		case BOSS_STATE::RETURN: break;
		default: break;
		}

	}
}

void AISystem::boss_health_render(Entity enemy) {
	auto& boss = registry.bossStatus.get(enemy);
	auto& healthbar = registry.bossHealthBar.get(boss.healthbar);
	auto& healthbar_motion = registry.motions.get(boss.healthbar);
	if (registry.cameras.entities.size() <= 0) {
		return;
	}
	Entity camera = registry.cameras.entities[0];
	Motion& cam_motion = registry.motions.get(camera);
	
	createTEXT(TEXT_ASSETS::LORD_BEE, healthbar_motion.position + vec2{ 0, -10 }, 30, 20);
	healthbar_motion.scale.x = boss.health / healthbar.boss_max_health * healthbar.healthbar_default_scale;
	if (boss.health <= 0) {
		healthbar_motion.scale.x = 0;
	}

	healthbar_motion.position = healthbar.position +  cam_motion.position;
}

void AISystem::boss_bee_transform(Entity enemy) {
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	if (boss_status.boss_mode == BOSS_MODE::CHARGE_MODE) {
		boss_status.boss_mode = BOSS_MODE::RANGE_MODE;
		registry.renderRequests.remove(enemy);
		registry.renderRequests.insert(
			enemy,
			{ TEXTURE_ASSET_ID::BEE_LORD_RANGE,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}
	else if (boss_status.boss_mode == BOSS_MODE::RANGE_MODE) {
		boss_status.boss_mode = BOSS_MODE::CHARGE_MODE;
		registry.renderRequests.remove(enemy);
		registry.renderRequests.insert(
			enemy,
			{ TEXTURE_ASSET_ID::BEE_LORD_CHARGE,
			 EFFECT_ASSET_ID::TEXTURED,
			 GEOMETRY_BUFFER_ID::SPRITE });
	}

	boss_freeze(enemy, 1000, false);
}

void AISystem::boss_freeze(Entity enemy, float freeze_time, bool restore) {
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	Motion& boss_movement = registry.motions.get(enemy);
	boss_status.boss_state = BOSS_STATE::FREEZE;
	if (restore) {
		temp_boss_switcher = boss_status.state_switch_timer;
	}
	boss_status.state_switch_timer = freeze_time;
	boss_movement.velocity = vec2(0, 0);
}

void AISystem::boss_bee_move_to_pos(Entity enemy) {
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	Motion& boss_movement = registry.motions.get(enemy);
	if (pos_distance(vec2(boss_movement.position.x, boss_status.default_pos.y), boss_movement.position) > 50) {
		boss_status.state_switch_timer = 500;
		if (boss_status.boss_mode == BOSS_MODE::CHARGE_MODE) {
			boss_movement.velocity = move_toward(vec2(boss_movement.position.x, boss_status.default_pos.y), boss_movement.position) * 2.f * boss_status.charge_mode_speed;
		}
		else if (boss_status.boss_mode == BOSS_MODE::RANGE_MODE) {
			boss_movement.velocity = move_toward(vec2(boss_movement.position.x, boss_status.default_pos.y), boss_movement.position) * boss_status.range_mode_speed;
		}
	}
	else {
		boss_bee_transform(enemy);
	}

}

void AISystem::boss_bee_idle_track(Entity enemy, Entity player) {
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	Motion& boss_movement = registry.motions.get(enemy);
	Motion& player_movement = registry.motions.get(player);

	boss_movement.velocity = move_toward((player_movement.position + vec2(0, 100)), boss_movement.position) * boss_status.charge_mode_speed;
}


void AISystem::boss_bee_idle_wander(Entity enemy, Entity player) {
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	Motion& boss_movement = registry.motions.get(enemy);
	Motion& player_movement = registry.motions.get(player);

	if ((player_movement.position.x - 50) > boss_movement.position.x) {
		boss_movement.velocity = vec2(1, 0) * boss_status.charge_mode_speed;
	}
	else if ((player_movement.position.x + 50) < boss_movement.position.x) {
		boss_movement.velocity = vec2(-1, 0) * boss_status.charge_mode_speed;
	}
	else {
		boss_movement.velocity = vec2(0, 0);
	}
}

void AISystem::boss_bee_evasion(Entity enemy) {
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	Motion& boss_movement = registry.motions.get(enemy);
	boss_movement.velocity = boss_status.movement_direction * boss_status.evasion_speed;
	if (boss_status.state_switch_timer < 200) {
		boss_freeze(enemy, boss_status.post_evasion_wait_time, false);

	}
}

void AISystem::boss_bee_charge_attack(Entity enemy) {
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	Motion& enemy_motion = registry.motions.get(enemy);
	float player_dist = pos_distance(boss_status.attack_target_pos, enemy_motion.position);
	if (boss_status.state_switch_timer > boss_status.charge_attack_time - 300) {
		enemy_motion.velocity = vec2(0, 0);
		return;
	}
	enemy_motion.velocity = boss_status.movement_direction * boss_status.charge_attack_speed;
	if (boss_status.state_switch_timer < 100) {
		enemy_motion.velocity = vec2(0, 0);
		boss_status.state_switch_timer = boss_status.post_attack_wait_time;
		boss_status.boss_state = BOSS_STATE::RETURN;

	}
}

void AISystem::boss_bee_range_attack(Entity enemy) {
	Motion& enemy_motion = registry.motions.get(enemy);
	BossStatus& boss_status = registry.bossStatus.get(enemy);
	Motion& boss_movement = registry.motions.get(enemy);

	if (boss_status.range_attack_counter < 5 && boss_status.range_attack_cd_timer < 0) {
		boss_movement.velocity = vec2(0, 0);

		create_enemy_tracking_projectile(boss_movement.position, boss_status.attack_target_entity, enemy);
		create_enemy_tracking_projectile(vec2(50, 0) + boss_movement.position, boss_status.attack_target_entity, enemy);
		create_enemy_tracking_projectile(vec2(-50, 0) + boss_movement.position, boss_status.attack_target_entity, enemy);
		boss_status.range_attack_counter++;
		boss_status.range_attack_cd_timer = boss_status.range_attack_cd_time;
	}
	else if (boss_status.range_attack_counter >= 5 && boss_status.state_switch_timer < 100) {
		boss_status.range_attack_counter = 0;
		boss_status.range_attack_cd_timer = 0;
		boss_status.state_switch_timer = boss_status.post_attack_wait_time;
		boss_bee_transform(enemy);
	}
}

void AISystem::idle(Entity enemy, Motion player_motion) {
	EnemyDecision& ai = registry.aidecisions.get(enemy);
	ai.state_switch_timer -= elapsed_ms_since_last_update;
	if (ai.state_switch_timer > 0) {
		return;
	}
	Motion& enemy_motion = registry.motions.get(enemy);
	float player_dist = pos_distance(player_motion.position, enemy_motion.position);

	if (player_dist < ai.alert_range) {
		ai.ai_state = ENEMY_STATE::ALERT;
		enemy_motion.velocity *= ai.alert_speed_factor;
		/*if (registry.motions.has(enemy_mesh)) {
			Motion& enemy_mesh_motion = registry.motions.get(enemy_mesh);
			enemy_mesh_motion.velocity *= ai.alert_speed_factor;
		}*/
		ai.state_switch_timer = ai.alert_reset_time;
	}
	else {
		ai.state_switch_timer = ai.idle_reset_time;
	}
}

void AISystem::alerted(Entity enemy, Motion player_motion) {
	EnemyDecision& ai = registry.aidecisions.get(enemy);
	Motion& enemy_motion = registry.motions.get(enemy);
	float player_dist = pos_distance(player_motion.position, enemy_motion.position);
	ai.state_switch_timer -= elapsed_ms_since_last_update;

	if (ai.state_switch_timer > 0) {
		return;
	}
	else if (player_dist < ai.attack_range) {
		ai.ai_state = ENEMY_STATE::ATTACK;
		ai.attack_target_pos = player_motion.position;
		ai.pre_attack_pos = enemy_motion.position;
		enemy_motion.velocity = move_toward(ai.attack_target_pos, enemy_motion.position) * ai.speed * ai.attack_speed_factor;
		/*if (registry.motions.has(enemy_mesh)) {
			Motion& enemy_mesh_motion = registry.motions.get(enemy_mesh);
			enemy_mesh_motion.velocity = enemy_motion.velocity;
		}*/
		ai.state_switch_timer = ai.post_attack_reset_time;
	}
	else if (player_dist < ai.alert_range) {
		ai.state_switch_timer = ai.alert_reset_time;
	}
	else {
		ai.ai_state = ENEMY_STATE::IDLE;
		ai.state_switch_timer = ai.idle_reset_time;
		enemy_motion.velocity = vec2(-1, 0) * ai.speed;
		/*if (registry.motions.has(enemy_mesh)) {
			Motion& enemy_mesh_motion = registry.motions.get(enemy_mesh);
			enemy_mesh_motion.velocity = enemy_motion.velocity;
		}*/
	}
}


void AISystem::attack(Entity enemy) {
	EnemyDecision& ai = registry.aidecisions.get(enemy);
	Motion& enemy_motion = registry.motions.get(enemy);
	float player_dist = pos_distance(ai.attack_target_pos, enemy_motion.position);

	if (player_dist < 10) {
		if (ai.state_switch_timer > 0) {
			enemy_motion.velocity = vec2(0, 0);
			/*if (registry.motions.has(enemy_mesh)) {
				Motion& enemy_mesh_motion = registry.motions.get(enemy_mesh);
				enemy_mesh_motion.velocity = enemy_motion.velocity;
			}*/
			ai.state_switch_timer -= elapsed_ms_since_last_update;

		}
		else {
			ai.ai_state = ENEMY_STATE::POST_ATTACK;
			enemy_motion.velocity = move_toward(ai.pre_attack_pos, enemy_motion.position) * ai.speed;
			/*if (registry.motions.has(enemy_mesh)) {
				Motion& enemy_mesh_motion = registry.motions.get(enemy_mesh);
				enemy_mesh_motion.velocity = enemy_motion.velocity;
			}*/
		}
	}

}

void AISystem::projectile_attack(Entity enemy, Entity player) {
	EnemyDecision& ai = registry.aidecisions.get(enemy);
	Motion& enemy_motion = registry.motions.get(enemy);
	create_enemy_tracking_projectile(enemy_motion.position, player, enemy);
	ai.ai_state = ENEMY_STATE::POST_ATTACK;
}

void AISystem::create_enemy_tracking_projectile(vec2 position, Entity target, Entity enemy) {
	if (registry.projectileCreationList.has(enemy))
	{
		ProjectileCreationList& creation_list = registry.projectileCreationList.get(enemy);
		ProjectileCreation creation = ProjectileCreation();
		creation.spawn_pos = position;
		creation.target = target;
		creation.wait_creation = true;
		creation_list.creation_list.push_back(creation);
	}
	else {
		ProjectileCreationList& creation_list = registry.projectileCreationList.emplace(enemy);
		ProjectileCreation creation = ProjectileCreation();
		creation.spawn_pos = position;
		creation.target = target;
		creation.wait_creation = true;
		creation_list.creation_list.push_back(creation);
	}

	
}

void AISystem::post_attack(Entity enemy) {
	EnemyDecision& ai = registry.aidecisions.get(enemy);
	Motion& enemy_motion = registry.motions.get(enemy);
	float og_pos_dist = pos_distance(ai.pre_attack_pos, enemy_motion.position);
	if (og_pos_dist < 10) {
		ai.ai_state = ENEMY_STATE::IDLE;
		ai.state_switch_timer = ai.idle_reset_time;
		enemy_motion.velocity = vec2(-1, 0) * ai.speed;
		/*if (registry.motions.has(enemy_mesh)) {
			Motion& enemy_mesh_motion = registry.motions.get(enemy_mesh);
			enemy_mesh_motion.velocity = enemy_motion.velocity;
		}*/
	}

}


void AISystem::projectile_update(float elapsed_ms) {
	if (registry.trackingProjectile.size() > 0) {
		for (int i = 0; i < registry.trackingProjectile.size(); i++) {
			Entity projectile_entity = registry.trackingProjectile.entities[i];
			TrackingProjectile& tracking = registry.trackingProjectile.get(projectile_entity);
			Motion& projectile_motion = registry.motions.get(projectile_entity);
			Entity target = tracking.target;
			Motion& target_motion = registry.motions.get(target);

			if (tracking.disappear_timer >= 0) {
				projectile_motion.velocity = move_toward(target_motion.position, projectile_motion.position) * tracking.projectile_speed;
				projectile_motion.angle = look_toward_angle(target_motion.position, projectile_motion.position);
				tracking.disappear_timer -= elapsed_ms;
			}
			else {
				registry.remove_all_components_of(projectile_entity);
			}

			if (debugging.in_debug_mode) {
				createLineToTarget(target_motion.position, projectile_motion.position, 5);
			}

		}
	}
}
