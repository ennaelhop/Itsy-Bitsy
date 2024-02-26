#pragma once

#include <vector>

#include "common.hpp"
#include <random>

float pos_distance(vec2 pos1, vec2 pos2);
vec2 move_toward(vec2 target_pos, vec2 self_pos);
float look_toward_angle(vec2 target_pos, vec2 self_pos);