#pragma once
#include <vector>

#include "tiny_ecs.hpp"
#include "components.hpp"

class ECSRegistry
{
	// Callbacks to remove a particular or all entities in the system
	std::vector<ContainerInterface *> registry_list;

public:
	// Manually created list of all components this game has
	ComponentContainer<IndestructibleEnemy> indestructibleEnemy;
	ComponentContainer<PlayerMesh> playerMesh;
	ComponentContainer<BeeMesh> beeMesh;
	ComponentContainer<DeathTimer> deathTimers;
	ComponentContainer<Motion> motions;
	ComponentContainer<Collision> collisions;
	ComponentContainer<Player> players;
	ComponentContainer<Mesh *> meshPtrs;
	ComponentContainer<RenderRequest> renderRequests;
	ComponentContainer <UIRenderRequest> uiRenderRequests;
	ComponentContainer<ScreenState> screenStates;
	ComponentContainer<Friendly> friendly;
	ComponentContainer<Enemy> enemy;
	ComponentContainer<DebugComponent> debugComponents;
	ComponentContainer<vec3> colors;
	ComponentContainer<EnemyDecision> aidecisions;
	ComponentContainer<BossStatus> bossStatus;
	ComponentContainer<Camera> cameras;
	ComponentContainer<BackgroundDepth> backgroundDepths;
	ComponentContainer<Damageable> damageable;
	ComponentContainer<PlayerStatus> playerStatus;
	ComponentContainer<Climbable> climbable;
	ComponentContainer<Unclimbable> unclimbable;
	ComponentContainer<currentlyBeingClimbed> CurrentlyBeingClimbed;
	ComponentContainer<TrackingProjectile> trackingProjectile;
	ComponentContainer<ProjectileCreationList> projectileCreationList;
	ComponentContainer<HelpDoc> helpDoc;
	ComponentContainer<CurrentHealth> currentHealth;
	ComponentContainer<CollisionTimer> collisionTimer;
	ComponentContainer<HealthBar> healthBar;
	ComponentContainer<Animation> animation;
	ComponentContainer<HealthContainer> healthContainer;
	ComponentContainer<Curssor> cursor;
	ComponentContainer<WebShoot> webshoots;
	ComponentContainer<CursorGesture> cursorGesture;
	ComponentContainer<AntXL> xlAnt;
	ComponentContainer<SpiderEffectTimer> spiderEffect;
	ComponentContainer<AntXS> xsAnt;
	ComponentContainer<AntImmune> immuneAnt;
	ComponentContainer<SpiderImmune> immuneSpider;
	ComponentContainer<AntRestoreHealth> healthAnt;
	ComponentContainer<SpiderRestore> restoreSpider;
	ComponentContainer<AntFast> fastAnt;
	ComponentContainer<SpiderFast> fastSpider;
	ComponentContainer<BossHealthBar> bossHealthBar;
	ComponentContainer<TextAssets> texts;
	ComponentContainer<SpriteSheet> spriteSheets;
	ComponentContainer<Emitter> emitters;
	ComponentContainer<Particle> particles;
	ComponentContainer<Physics> physics;
	ComponentContainer<WebString> webString;
	ComponentContainer<SpiderXL>xlSpider;
	ComponentContainer<BeeHive>beeHive;
	ComponentContainer<BeeSwarm>beeSwarm;
	//ComponentContainer<Tutorial>tutorial;
	ComponentContainer<Levels>levels;
	ComponentContainer<UIPosition> uiPositions;
	ComponentContainer<Stopwatch> stopwatches;
	ComponentContainer<TextElem> textElements;

	// constructor that adds all containers for looping over them
	// IMPORTANT: Don't forget to add any newly added containers!
	ECSRegistry()
	{
		registry_list.push_back(&indestructibleEnemy);
		registry_list.push_back(&playerMesh);
		registry_list.push_back(&beeMesh);
		registry_list.push_back(&deathTimers);
		registry_list.push_back(&motions);
		registry_list.push_back(&collisions);
		registry_list.push_back(&players);
		registry_list.push_back(&meshPtrs);
		registry_list.push_back(&renderRequests);
		registry_list.push_back(&uiRenderRequests);
		registry_list.push_back(&screenStates);
		registry_list.push_back(&friendly);
		registry_list.push_back(&enemy);
		registry_list.push_back(&debugComponents);
		registry_list.push_back(&colors);
		registry_list.push_back(&cameras);
		registry_list.push_back(&backgroundDepths);
		registry_list.push_back(&aidecisions);
		registry_list.push_back(&damageable);
		registry_list.push_back(&playerStatus);
		registry_list.push_back(&climbable);
		registry_list.push_back(&unclimbable);
		registry_list.push_back(&CurrentlyBeingClimbed);
		registry_list.push_back(&trackingProjectile);
		registry_list.push_back(&projectileCreationList);
		registry_list.push_back(&helpDoc);
		registry_list.push_back(&cursor);
		registry_list.push_back(&webshoots);
		registry_list.push_back(&cursorGesture);
		registry_list.push_back(&currentHealth);
		registry_list.push_back(&collisionTimer);
		registry_list.push_back(&healthBar);
		registry_list.push_back(&animation);
		registry_list.push_back(&healthContainer);
		registry_list.push_back(&xlAnt);
		registry_list.push_back(&spiderEffect);
		registry_list.push_back(&xsAnt);
		registry_list.push_back(&immuneAnt);
		registry_list.push_back(&immuneSpider);
		registry_list.push_back(&healthAnt);
		registry_list.push_back(&restoreSpider);
		registry_list.push_back(&fastAnt);
		registry_list.push_back(&fastSpider);
		registry_list.push_back(&bossStatus);
		registry_list.push_back(&bossHealthBar);
		registry_list.push_back(&texts);
		registry_list.push_back(&spriteSheets);
		registry_list.push_back(&emitters);
		registry_list.push_back(&particles);
		registry_list.push_back(&physics);
		registry_list.push_back(&webString);
		registry_list.push_back(&xlSpider);
		registry_list.push_back(&beeHive);
		registry_list.push_back(&beeSwarm);
	//	registry_list.push_back(&tutorial);
		registry_list.push_back(&levels);
		registry_list.push_back(&uiPositions);
		registry_list.push_back(&stopwatches);
		registry_list.push_back(&textElements);
    }

	void clear_all_components()
	{
		for (ContainerInterface *reg : registry_list)
			reg->clear();
	}

	void list_all_components()
	{
		printf("Debug info on all registry entries:\n");
		for (ContainerInterface *reg : registry_list)
			if (reg->size() > 0)
				printf("%4d components of type %s\n", (int)reg->size(), typeid(*reg).name());
	}

	void list_all_components_of(Entity e)
	{
		printf("Debug info on components of entity %u:\n", (unsigned int)e);
		for (ContainerInterface *reg : registry_list)
			if (reg->has(e))
				printf("type %s\n", typeid(*reg).name());
	}

	void remove_all_components_of(Entity e)
	{
		for (ContainerInterface *reg : registry_list)
			reg->remove(e);
	}
};

extern ECSRegistry registry;