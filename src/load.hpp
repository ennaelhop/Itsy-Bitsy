#pragma once

#include "common.hpp"
#include "world_init.hpp"
#include <iostream>
#include "render_system.hpp"

class Load
{
public:
//    Load(RenderSystem* renderer) { this->renderer = renderer; };

    void save_load(vec2 player_position, float player_health, float treeHealth[], float treePosX[], float treePosY[], std::string level);
    bool load_save(std::string name, Entity* camera, RenderSystem* renderer);
    int get_health_saved();
    vec2 get_position_saved();
private:
    const std::map<std::string, int> palettes = {
            {"Forest", 0},
    };
    const void createForestBackgrounds(std::ifstream& file);

    vec2 spider_pos;
    int spider_health;
    float health_bar_pos;
    RenderSystem* renderer;
    int size = 0;
};