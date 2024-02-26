#include "math_helpers.hpp"

float pos_distance(vec2 pos1, vec2 pos2) {
	return sqrt(pow((pos1.x - pos2.x), 2) + pow((pos1.y - pos2.y), 2));
}

vec2 move_toward(vec2 target_pos, vec2 self_pos) {
	vec2 movement = vec2(target_pos.x - self_pos.x, target_pos.y - self_pos.y);
	float pos_dist = pos_distance(target_pos, self_pos);
	movement = vec2(movement.x / pos_dist, movement.y / pos_dist);
	return  movement;
}

float look_toward_angle(vec2 target_pos, vec2 self_pos) {
	float angle = atan2(target_pos.y - self_pos.y, target_pos.x - self_pos.x);
	return angle;
}