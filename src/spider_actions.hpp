#include <vector>

#include "common.hpp"


void spider_shooting(RenderSystem* renderer, vec2 cursor_position);
void spider_shooting_big_acid(RenderSystem* renderer, vec2 cursor_position, float factor);
void spider_web_shooting_update(float elapsed_time);
void spider_web_swing(float elapsed_time);
void CalculatingTheRequiredVelocityVector(Motion& motion,vec2 TargetPos);


