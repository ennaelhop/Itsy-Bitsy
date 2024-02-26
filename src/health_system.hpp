#pragma once
#include "tiny_ecs_registry.hpp"

class HealthSystem {
    public:
    static bool takeDamage(Entity entity, int damage);
    static bool takeDamageObstacle(Entity entity, int damage);
    static bool takeDamageNonClimbableObstacle(Entity entity, int damage);

    void takeDamageStinger(Entity entity, void(*fp)(Entity));
    static void instaKill(Entity entity);
    void addHealth(Entity entity, int health);


    void fullHealth(Entity entity);

    HealthSystem() {
        //printf("health system is created");
    }
     ~HealthSystem(){
	};
};



