#include <tiny_ecs.hpp>
#include "ai_swarm.hpp"
#include "common.hpp"
#include <components.hpp>
#include "tiny_ecs_registry.hpp"
#include <math_helpers.hpp>
#include <iostream>

void flyTowardsCenter(Entity swarm) {

    float centerX = 0;
    float centerY = 0;
    int numNeighbors = 0;
    auto& swarm_behavior = registry.beeSwarm.get(swarm);
    auto& swarm_motion = registry.motions.get(swarm);
    
    for (Entity e : registry.beeSwarm.entities) {
        if (e != swarm) {
            auto& swarm_other_motion = registry.motions.get(e);
            if (pos_distance(swarm_motion.position, swarm_other_motion.position) < swarm_behavior.visualRange) {
                centerX += swarm_other_motion.position.x;
                centerY += swarm_other_motion.position.y;
                numNeighbors += 1;
            }
        }
    }

    if (numNeighbors) {
        centerX = centerX / numNeighbors;
        centerY = centerY / numNeighbors;
        float diff = (centerX - swarm_motion.position.x) * swarm_behavior.centeringFactor;
        swarm_motion.velocity.x += (centerX - swarm_motion.position.x) * swarm_behavior.centeringFactor;
        swarm_motion.velocity.y += (centerY - swarm_motion.position.y) * swarm_behavior.centeringFactor;
    }
}
void avoidOthers(Entity swarm) {

    float moveX = 0;
    float moveY = 0;
    auto& swarm_behavior = registry.beeSwarm.get(swarm);
    auto& swarm_motion = registry.motions.get(swarm);

    for (Entity e : registry.beeSwarm.entities) {
        if (e != swarm) {
            auto& swarm_other_motion = registry.motions.get(e);
            if (pos_distance(swarm_motion.position, swarm_other_motion.position) < swarm_behavior.minDistance) {
                moveX += swarm_motion.position.x - swarm_other_motion.position.x;
                moveY += swarm_motion.position.y - swarm_other_motion.position.y;
            }
        }
    }

    swarm_motion.velocity.x += moveX * swarm_behavior.avoidFactor;
    swarm_motion.velocity.y += moveY * swarm_behavior.avoidFactor;

}

void keepWithinBounds(Entity swarm) {
	auto& swarm_motion = registry.motions.get(swarm);
    auto& swarm_behavior = registry.beeSwarm.get(swarm);
    const float margin = 200;
    const int turnFactor = 100;

    if (swarm_motion.position.x < swarm_behavior.spawn_point.x - margin) {
        swarm_motion.velocity.x += turnFactor;
    }
    if (swarm_motion.position.x > swarm_behavior.spawn_point.x + margin) {
        swarm_motion.velocity.x -= turnFactor;
    }
    if (swarm_motion.position.y < swarm_behavior.spawn_point.y - margin) {
        swarm_motion.velocity.y += turnFactor;
    }
    if (swarm_motion.position.y > swarm_behavior.spawn_point.y + margin) {
        swarm_motion.velocity.y -= turnFactor;
    }
}

void matchVelocity(Entity swarm) {
    float avgDX = 0;
    float avgDY = 0;
    int numNeighbors = 0;
    auto& swarm_behavior = registry.beeSwarm.get(swarm);
    auto& swarm_motion = registry.motions.get(swarm);

    for (Entity e : registry.beeSwarm.entities) {
        if (e != swarm) {
            auto& swarm_other_motion = registry.motions.get(e);
            if (pos_distance(swarm_motion.position, swarm_other_motion.position) < swarm_behavior.visualRange) {
                avgDX += swarm_other_motion.velocity.x;
                avgDY += swarm_other_motion.velocity.y;
                numNeighbors += 1;
            }
        }
    }

    if (numNeighbors) {
        avgDX = avgDX / numNeighbors;
        avgDY = avgDY / numNeighbors;

        swarm_motion.velocity.x += (avgDX - swarm_motion.velocity.x) * swarm_behavior.matchingFactor;
        swarm_motion.velocity.y += (avgDY - swarm_motion.velocity.y) * swarm_behavior.matchingFactor;
    }
}

void limitSpeed(Entity swarm) {
    
    auto& swarm_behavior = registry.beeSwarm.get(swarm);
    auto& swarm_motion = registry.motions.get(swarm);

    float speedLimit = swarm_behavior.swarm_speed;
    float speed = sqrt(swarm_motion.velocity.x * swarm_motion.velocity.x + swarm_motion.velocity.y * swarm_motion.velocity.y);
    if (speed > speedLimit) {
        swarm_motion.velocity.x = (swarm_motion.velocity.x / speed) * speedLimit;
        swarm_motion.velocity.y = (swarm_motion.velocity.y / speed) * speedLimit;
    }
}

void destroyHive(Entity entity_other) {
    
    auto& hive = registry.beeHive.get(entity_other);
    for (Entity e : hive.swarm) {
        if (registry.beeSwarm.has(e) && registry.motions.has(e)) {
            auto& swarm_motion = registry.motions.get(e);
            auto& swarm_behavior = registry.beeSwarm.get(e);
            swarm_behavior.hive_alive = false;
            swarm_motion.velocity = normalize(vec2(rand() % 11 - 5, rand() % 11 - 5)) * swarm_behavior.swarm_speed;
        }
    }
    registry.remove_all_components_of(entity_other);
}