#pragma once

#include "common.hpp"
#include "world_init.hpp"
#include <iostream>
#include "render_system.hpp"

class LevelBuilder
{
public:
	LevelBuilder(RenderSystem* renderer) { this->renderer = renderer; };

	bool loadLevel(std::string name, Entity* camera);

private:
	// The following is a mapping from palette to index, so we can use switch
	const std::map<std::string, int> palettes = { 
		{"Forest", 0},
		{"Forest2", 1}
	};
	const void createForestBackgrounds(std::ifstream& file);
	const void createLevel2Background(std::ifstream& file);
	const void createForestObstacles(std::ifstream& file);

	RenderSystem* renderer;
};