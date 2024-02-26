#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "components.hpp"
#include "tiny_ecs_registry.hpp"

// A simple physics system that moves rigid bodies and checks for collision
class PhysicsSystem
{
public:
	void step(float elapsed_ms, float window_width_px, float window_height_px);
	void drawBoundingBox(Motion motion);
	vec3 convertToWorld(Motion motion, vec3 vertexPos);
	//vec2 getNormalizedAxis(vec2 current, vec2 next);
	//bool checkCollisionSAT(Entity& player, Entity& enemy);
	bool IsPointInPolygon(vec2 p, Entity entity);
	void spider_falling(Motion& spider, PlayerStatus& spider_status, const Entity& entity_j);

	void zoomModePositions(float elapsed_ms);

	PhysicsSystem()
	{
	};
private:
	void player_bound();
	void player_ground(float window_height_px);
	void groundCollide(Entity& e, Motion& motion);
	void spider_trees_climb(const Entity& entity_j, const Entity& entity_i, Motion& motion_i, Motion& motion_j, float window_height_px);

	float GRAVITY = 150;
};