#include "health_system.hpp"
#include "tiny_ecs_registry.hpp"
#include "world_system.hpp"
#include "physics_system.hpp"
#include "world_init.hpp" 
#include <iostream>
#include <world_init.hpp>
using namespace std;

bool HealthSystem::takeDamage(Entity e, int damage)
{
	//auto &healthOfTheSpider = registry.currentHealth.get(e);
	//auto &HealthBar = registry.healthBar.entities[0];
	//auto &healthBarMotion = registry.motions.get(HealthBar);
	//healthOfTheSpider.health -= 33;

	//if (healthOfTheSpider.health <= 0)
	//{
	//	healthBarMotion.scale.x = 0;
	//	fp(e);
	//}
	//else
	//{
	//	cout << to_string(healthOfTheSpider.health) << endl;
	//	healthBarMotion.position.x = healthBarMotion.position.x - 23.f;
	//	healthBarMotion.scale.x = 0.66 * healthBarMotion.scale.x;
	//}

	auto& damageable = registry.damageable.get(e);
	damageable.health -= damage;
	if (damageable.health <= 0) {
		instaKill(e); // I feel like this is a little strange...
		return true;
	}
	if (registry.players.has(e)) { //update the health bar
		
		/*float remaining = std::max(0.f, (float)damageable.health / damageable.maxHealth);
		auto &HealthBar = registry.healthBar.entities[0];
		auto& health_bar_prop = registry.healthBar.get(HealthBar);
		auto &healthBarMotion = registry.motions.get(HealthBar);
		float left = healthBarMotion.position.x - (healthBarMotion.scale.x / 2.f);
		healthBarMotion.scale.x = remaining * 150;
		health_bar_prop.pos.x = left + (healthBarMotion.scale.x / 2.f);
		*/
		//std::cout << "left: "<< left << "\nscale: "<< healthBarMotion.scale.x << "\nplayer health: " << remaining << "\n";
	}
	return false;

}

bool HealthSystem::takeDamageObstacle(Entity e, int damage) {
	auto& damageable = registry.damageable.get(e);
	auto& spriteSheet = registry.spriteSheets.get(e);
	damageable.health -= damage;
	float remaining = (float)damageable.health / damageable.maxHealth;
	
	//update the sprite
	if (remaining <= 0.25)
		spriteSheet.index = 3;
	else if (remaining <= 0.50)
		spriteSheet.index = 2;
	else if (remaining <= 0.75)
		spriteSheet.index = 1;
	else
		spriteSheet.index = 0;

	if (damageable.health <= 0) {
		auto& attachments = registry.climbable.get(e).attachments;
		auto& motion = registry.motions.get(e);
		createAreaEmitter(motion.position, motion.scale, ParticleType::TREE);

		// if tree gets destroyed while spider is on it, have spider fall down from it
		PhysicsSystem physics;
		Entity player_spider = registry.players.entities[0];
		PlayerStatus& ps = registry.playerStatus.get(player_spider);
		if (registry.CurrentlyBeingClimbed.has(e)) {
			ps.canClimb = false;
			Motion& motion_spider = registry.motions.get(player_spider);
			motion_spider.velocity.y = 0;
			physics.spider_falling(motion_spider, ps, player_spider);
		}


		for (Entity e : attachments) {
			registry.remove_all_components_of(e);
		}
		registry.remove_all_components_of(e);

		return true;
	}
	return false;
}

bool HealthSystem::takeDamageNonClimbableObstacle(Entity e, int damage) {
	auto& damageable = registry.damageable.get(e);
	damageable.health -= damage;
	float remaining = (float)damageable.health / damageable.maxHealth;

	if (damageable.health <= 0) {
		auto& attachments = registry.unclimbable.get(e).attachments;
		auto& motion = registry.motions.get(e);
		createAreaEmitter(motion.position, motion.scale, ParticleType::TREE);
		for (Entity e : attachments) {
			registry.remove_all_components_of(e);
		}
		registry.remove_all_components_of(e);
		return true;
	}
	return false;
}

void HealthSystem::takeDamageStinger(Entity entity, void (*fp)(Entity))
{
	auto &healthOfTheSpider = registry.currentHealth.get(entity);
	auto &HealthBar = registry.healthBar.entities[0];
	auto &healthBarMotion = registry.motions.get(HealthBar);
	healthOfTheSpider.health -= 15;

	if (healthOfTheSpider.health <= 0)
	{
		healthBarMotion.scale.x = 0;
		fp(entity);
	}
	else
	{
		cout << to_string(healthOfTheSpider.health) << endl;
		healthBarMotion.position.x = healthBarMotion.position.x - 9.f;
		healthBarMotion.scale.x = 0.85 * healthBarMotion.scale.x;
	}
}

void HealthSystem::instaKill(Entity entity)
{
	if (registry.players.has(entity)) {
		registry.deathTimers.emplace(entity);
		// Mix_PlayChannel(-1, spider_dead_sound, 0);
		// play sound effect
		Mix_Chunk* spider_dead;
		spider_dead = Mix_LoadWAV(audio_path("spider_dead.wav").c_str());
		Mix_PlayChannel(-1, spider_dead, 0);
		registry.motions.get(entity).angle = 3.1415f;
		registry.motions.get(entity).velocity = { 0, 80 };
	}
	else {
		registry.remove_all_components_of(entity);
	}
}

void HealthSystem::addHealth(Entity entity, int health)
{
	auto &HealthBar = registry.healthBar.entities[0];
	auto &healthBarMotion = registry.motions.get(HealthBar);
	auto& damageable = registry.damageable.get(entity);

	damageable.health = std::min(damageable.maxHealth, damageable.health + health);
	
	if (registry.players.has(entity)) { //update the health bar for the player
		float remaining = std::max(0.f, (float)damageable.health / damageable.maxHealth);
		auto& HealthBar = registry.healthBar.entities[0];
		auto& healthBarMotion = registry.motions.get(HealthBar);
		float left = healthBarMotion.position.x - (healthBarMotion.scale.x / 2.f);
		healthBarMotion.scale.x = remaining * 150;
		healthBarMotion.position.x = left + (healthBarMotion.scale.x / 2.f);
		//std::cout << "left: " << left << "\nscale: " << healthBarMotion.scale.x << "\nplayer health: " << remaining << "\n";
	}

	//if (currentHealth.health < 100)
	//{
	//	if (currentHealth.health < 85)
	//	{
	//		healthBarMotion.position.x = healthBarMotion.position.x + 7;
	//		healthBarMotion.scale.x = 1.15 * healthBarMotion.scale.x;
	//		currentHealth.health += 15;
	//	}
	//	else
	//	{
	//		healthBarMotion.position.x+= 7;
	//		healthBarMotion.scale = vec2({1.5 * 100.f, 1.5 * 52.f});
	//		currentHealth.health = 100;
	//	}
	//}
}

void HealthSystem::fullHealth(Entity entity)
{
	auto& damageable = registry.damageable.get(entity);

	damageable.health = damageable.maxHealth;

	if (registry.players.has(entity)) { //update the health bar for the player
		float remaining = std::max(0.f, (float)damageable.health / damageable.maxHealth);
		auto& HealthBar = registry.healthBar.entities[0];
		auto& healthBarMotion = registry.motions.get(HealthBar);
		float left = healthBarMotion.position.x - (healthBarMotion.scale.x / 2.f);
		healthBarMotion.scale.x = remaining * 150;
		healthBarMotion.position.x = left + (healthBarMotion.scale.x / 2.f);
		//std::cout << "left: " << left << "\nscale: " << healthBarMotion.scale.x << "\nplayer health: " << remaining << "\n";
	}
}
