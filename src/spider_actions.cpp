#include "common.hpp"
#include <world_system.hpp>
#include <world_init.hpp>
#include <math_helpers.hpp>
#include "iostream"
#include <glm/gtx/string_cast.hpp>
#include <math_helpers.hpp>
#include <cmath>
#include "components.hpp"
using namespace std;

void spider_shooting(RenderSystem *renderer, vec2 cursor_position)
{
	Entity player_spider = registry.players.entities[0];
	PlayerStatus &player_status = registry.playerStatus.get(player_spider);
	Motion &player_motion = registry.motions.get(player_spider);
	if (player_status.reload_timer <= 0)
	{
		Entity web_shoot = createWebShoot(renderer, player_motion.position, cursor_position, player_status.webshoot_speed);
		player_status.reload_timer = player_status.reload_time_preset;
		// play sound effect
		Mix_Chunk *web;
		web = Mix_LoadWAV(audio_path("web.wav").c_str());
		Mix_PlayChannel(-1, web, 0);
	}
}

void spider_shooting_big_acid(RenderSystem* renderer, vec2 cursor_position, float factor)
{
	Entity player_spider = registry.players.entities[0];
	PlayerStatus& player_status = registry.playerStatus.get(player_spider);
	Motion& player_motion = registry.motions.get(player_spider);
	if (player_status.reload_timer <= 0)
	{
		Entity web_shoot = createBigWebShoot(renderer, player_motion.position, cursor_position, player_status.webshoot_speed, factor);
		player_status.reload_timer = player_status.reload_time_preset;
		// play sound effect
		Mix_Chunk* web;
		web = Mix_LoadWAV(audio_path("web.wav").c_str());
		Mix_PlayChannel(-1, web, 0);
	}
}

void pendulumUpdate(Entity &spider, vec2 targetPos, vec2 origin_pos)
{

	Motion &motionSpider = registry.motions.get(spider);
	PlayerStatus &playerStatus = registry.playerStatus.get(spider);
	float length = pos_distance(targetPos, origin_pos);
	debugging.acceleration = (-1 * debugging.gravity / length) * sin(debugging.angle);
	debugging.velocity += debugging.acceleration;
	debugging.velocity *= debugging.damping;
	debugging.angle += debugging.velocity;
	motionSpider.position = vec2(length * sin(debugging.angle), length * cos(debugging.angle));
	motionSpider.position += targetPos;

	Entity camera = registry.cameras.entities[0];
	Camera& cam = registry.cameras.get(camera);
	if (motionSpider.position.x <= 0) {
		motionSpider.position.x = 0;
	}
	if (motionSpider.position.x >= cam.xBound[1] + 1200) {
		motionSpider.position.x = cam.xBound[1] + 1200;

	}

	playerStatus.flyoff_dir = move_toward(motionSpider.position, origin_pos);
	

}

void spider_web_swing(float elapsed_time)
{

Entity player_spider = registry.players.entities[0];
	PlayerStatus &playerStatus = registry.playerStatus.get(player_spider);
	Motion &player_motion = registry.motions.get(player_spider);
	Entity cursor = registry.cursor.entities[0];
	CursorGesture &gest = registry.cursorGesture.get(cursor);
	gest.disappear_timer -= elapsed_time;

	if (gest.gesture && gest.disappear_timer >= 0 && playerStatus.swing)
	{
		// cout << playerStatus.webStringTargetPos.y << endl;

		Entity line = createLineToTarget(playerStatus.webStringTargetPos, player_motion.position, 5);
		if (registry.webString.size() > 0 && playerStatus.swing == true)
		{
			if ((pos_distance(player_motion.position, playerStatus.webStringEndPointPos) <= 20 && pos_distance(player_motion.position, playerStatus.webStringEndPointPos) >= 0))
			{
				if (gest.cursor_gesture_pos1.x < playerStatus.webStringEndPointPos.x && debugging.firstSwing)
				{

					debugging.angle = 7 * M_PI / 4;
					debugging.firstSwing = false;
				}
				player_motion.velocity = {0, 0};
			}
			if (player_motion.velocity == vec2(0, 0))
			{
				pendulumUpdate(player_spider, playerStatus.webStringTargetPos, player_motion.position);
			}
		}
		player_motion;
		registry.colors.insert(line, {255, 255, 255});
	}
	if (gest.disappear_timer <= 0 && playerStatus.swing) {
		playerStatus.swing = false;
		playerStatus.forcefall = true;
	}
	if (playerStatus.forcefall)
	{
		debugging.acceleration = 0;
		debugging.velocity = 0;
		debugging.angle = M_PI / 4;
		debugging.gravity = 0.5;
		debugging.damping = 0.997;
		debugging.firstSwing = true;
	//	delay.beeDodge = false;
		// This might be redundant since it's already in the physics sys? it is not since phys only checks this condition if there was colision with tree
		// not all the time
		if (player_motion.position.y < SPIDER_DEFAULT_Y)
		{
			playerStatus.canClimb = false;

			// add gravity
			player_motion.velocity.y += 150 * elapsed_time/1000;
			// disable 'S' key
			playerStatus.falling = true;
			playerStatus.swing = false;
			// play sound effect
			Mix_Chunk *spider_falling;
			spider_falling = Mix_LoadWAV(audio_path("spider_falling.wav").c_str());
			Mix_PlayChannel(-1, spider_falling, 0);
			// flip spider upside down
			player_motion.angle = 3.1415f;
			// reset to idle animations
			Animation &animate = registry.animation.get(player_spider);
			animate.SPIDER_IDLE = 1;
			animate.SPIDER_LEFT, animate.SPIDER_RIGHT, animate.SPIDER_DOWN, animate.SPIDER_UP = 0;
			
			delay.beeDodge = false;
		}
		else
		{
			player_motion.velocity = vec2(0, 0);
			playerStatus.forcefall = false;
			playerStatus.falling = false;
		}
	}
}

void CalculatingTheRequiredVelocityVector(Motion &motion, vec2 TargetPos)
{

	float distance = pos_distance(motion.position, TargetPos);
	motion.velocity.x = 400 / distance * (TargetPos.x - motion.position.x);
	motion.velocity.y = 400 / distance * (TargetPos.y - motion.position.y);
}

void spider_web_string_update(Entity player, float elapsed_time)
{
	Motion &player_motion = registry.motions.get(player);
	PlayerStatus &player_status = registry.playerStatus.get(player);
	Entity &cursor = registry.cursor.entities[0];
	CursorGesture &cursorGest = registry.cursorGesture.get(cursor);
	float target_dist = pos_distance(player_status.webStringEndPointPos, player_status.webStringTargetPos);
	if (player_status.forcefall) {
		player_status.forcefall = false;
	}
	player_motion.angle = 0;
	if (target_dist < 50)
	{
		player_status.swing = true;
		player_status.shootingWeb = false;
		vec2 direction = move_toward(cursorGest.cursor_gesture_pos2, cursorGest.cursor_gesture_pos1);
		vec2 spiderMoveTarget = player_motion.position + vec2(direction.x * 200.f, direction.y * 200.f);
		player_status.webStringEndPointPos = spiderMoveTarget;
		CalculatingTheRequiredVelocityVector(player_motion, player_status.webStringEndPointPos);
		return;
	}
	player_status.webStringEndPointPos += player_status.webStringTargetDirection * player_status.webStringSpeed * (elapsed_time / 1000);

	Entity line = createLineToTarget(player_status.webStringEndPointPos, player_motion.position, 5);
	registry.colors.insert(line, {255, 255, 255});
}

void spider_web_shooting_update(float elapsed_time)
{
	Entity player_spider = registry.players.entities[0];
	PlayerStatus &player_status = registry.playerStatus.get(player_spider);

	if (player_status.reload_timer >= 0)
	{
		player_status.reload_timer -= elapsed_time;
	}
	if (registry.webshoots.size() > 0)
	{
		for (int i = 0; i < registry.webshoots.size(); i++)
		{
			Entity e = registry.webshoots.entities[i];
			WebShoot &webshoot = registry.webshoots.get(e);
			if (webshoot.disappear_timer < 0)
			{
				registry.remove_all_components_of(e);
				delay.beeDodge = false;
			}
			else
			{
				webshoot.disappear_timer -= elapsed_time;
			}
		}
	}

	spider_web_swing(elapsed_time);
	if (player_status.shootingWeb)
	{
		spider_web_string_update(player_spider, elapsed_time);
	}
}
