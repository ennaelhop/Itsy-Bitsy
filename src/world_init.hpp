#pragma once

#include "common.hpp"
#include <string>
#include "tiny_ecs.hpp"
#include "render_system.hpp"

// These are hard coded to the dimensions of the entity texture
const float ANT_WIDTH = 38.f;
const float ANT_HEIGHT = 31.f;
// health_fill has the same dimensions as health_container
const float HEALTH_CONTAINER_WIDTH = 100.f;
const float HEALTH_CONTAINER_HEIGHT = 52.f;
const float BEE_BB_WIDTH = 0.4f * 300.f;
const float BEE_BB_HEIGHT = 0.4f * 202.f;
const float GROUND_WIDTH = 1200.f;
const float GROUND_HEIGHT =  94.f;
const float SPIDER_DEFAULT_Y = 650.f;
const float SPIDER_DEFAULT_X = 300.f;
enum class ParticleType {
	TREE = 1,
	GROUND = 2,
	BEE = 3
};


// the player
Entity createSpider(RenderSystem* renderer);
//Entity createSpiderMesh(RenderSystem* renderer, Entity spider);

// the prey
Entity createRegularAnt(RenderSystem* renderer, vec2 position);
Entity createXLAnt(RenderSystem* renderer, vec2 position);
Entity createXSAnt(RenderSystem* renderer, vec2 position);
Entity createImmuneAnt(RenderSystem* renderer, vec2 position);
Entity createRestoreHealthAnt(RenderSystem* renderer, vec2 position);
Entity createFastAnt(RenderSystem* renderer, vec2 position);
// the enemy
Entity createBee(RenderSystem* renderer, vec2 position);
//Entity createBeeMesh(RenderSystem* renderer, vec2 position, Entity bee);

Entity createChargerBee(RenderSystem* renderer, vec2 position);
Entity createRangerBee(RenderSystem* renderer, vec2 position);
Entity createBossBee(RenderSystem* renderer, vec2 screen_size, vec2 position);
Entity createSwarmBee(RenderSystem* renderer, vec2 position);
Entity createBeeHive(RenderSystem* renderer, vec2 position, Entity tree_ref);
Entity createBossHealth(RenderSystem* renderer, vec2 screen_size, float boss_max_health);

// health bar
Entity createHealthContainer(RenderSystem* renderer);
Entity createHealthFill(RenderSystem* renderer);

Entity createSting(RenderSystem* renderer, vec2 spawn_pos, Entity target);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
Entity createLineToTarget(vec2 target_pos, vec2 origin_pos, float width);
// Background additions 
Entity createBush(RenderSystem* renderer, vec2 pos);
Entity createGrass(RenderSystem* renderer, vec2 pos);
Entity createGround(RenderSystem* renderer, vec2 pos);
Entity createLights1(RenderSystem* renderer, vec2 pos);
Entity createLights2(RenderSystem* renderer, vec2 pos);

// Help 
Entity createHelp(RenderSystem* renderer, vec2 pos);
Entity createHelpDoc(RenderSystem* renderer, vec2 pos);

// Tutorial 
Entity createGameIntro(RenderSystem* renderer);
Entity createWASDInstructions(RenderSystem* renderer);
Entity createGreatInstructions(RenderSystem* renderer);
Entity createShootInstructions(RenderSystem* renderer);
Entity createBeeInstructions(RenderSystem* renderer);
Entity createClimbInstructions(RenderSystem* renderer);
Entity createKeepDestroyingInstructions(RenderSystem* renderer);
Entity createSwingInstructions(RenderSystem* renderer);
Entity createAntInstructions(RenderSystem* renderer);
Entity createAntDetails(RenderSystem* renderer);
Entity createReady(RenderSystem* renderer);
Entity createSkip(RenderSystem* renderer);

// prompts
Entity createLevel2Prompt(RenderSystem* renderer, vec2 pos);
Entity createEndOfGamePrompt(RenderSystem* renderer, vec2 pos);

// level banners
Entity createLevel1(RenderSystem* renderer);
Entity createLevel2(RenderSystem* renderer);

// tree with bark
Entity createTreeBark(RenderSystem* renderer, float x);
// tree with leaves
Entity createTreeLeaves(RenderSystem* renderer, float x);
// background elems
Entity createBackground(RenderSystem* renderer, float depth);
Entity createBackgroundLevel2(RenderSystem* renderer, float depth);
Entity createBushLevel2(RenderSystem* renderer, float x);
Entity createTallTreeLevel2(RenderSystem* renderer, float x);
Entity createSnail(RenderSystem* renderer, vec2 position);
Entity createLevel2SnailDetail(RenderSystem* renderer, vec2 pos);
Entity createLevel2Acid2Detail(RenderSystem* renderer, vec2 pos);
// Camera entity
Entity createCamera(int xMin, int xMax);

Entity createCursor(RenderSystem* renderer);
Entity createWebShoot(RenderSystem* renderer, vec2 spider_pos, vec2 target_pos, float speed);
Entity createBigWebShoot(RenderSystem* renderer, vec2 spider_pos, vec2 target_pos, float speed, float buildUpFactor);
Entity createWebShootResidual(RenderSystem* renderer, vec2 pos);

enum class TEXT_ASSETS {
	BEE_INTRO,
	LORD_BEE,
	STOP,
	NO,
	SAVED, 
	ACID_DEFAULT, 
	ACID_CHARGED
};

Entity createTEXT(TEXT_ASSETS content, vec2 pos, float size,  float duration);
Entity createPointEmitter(vec2 pos, vec2 direction, ParticleType type);
Entity createAreaEmitter(vec2 pos, vec2 scale, ParticleType type);
Entity createHitParticle(vec2 pos, vec2 direction);
Entity createTimer(vec2 pos, vec2 scale);
Entity createTextUIElem(vec2 pos, vec2 scale, std::string text);
