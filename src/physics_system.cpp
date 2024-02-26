// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include <world_system.hpp>
#include <iostream>

WorldSystem world;

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	  return { abs(motion.scale.x), abs(motion.scale.y) };
}


bool collides(const Motion& motion1, const Motion& motion2)
{

	// getting bounding boxes for entities 
	vec2 motion1box = get_bounding_box(motion1);
	vec2 motion2box = get_bounding_box(motion2);

	// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
	// note: dividing bounding box by 2 such that collision will register when entities are overlapping/collision detection is not too sensitive
	if (motion1.position.x < motion2.position.x + motion2box.x/2 &&
		motion1.position.x + motion1box.x/2 > motion2.position.x &&
		motion1.position.y < motion2.position.y + motion2box.y/2 &&
		motion1.position.y + motion1box.y/2 > motion2.position.y) {
			return true;
	} 
	
	return false;

}

bool collidesTree(const Motion& motion1, const Motion& motion2)
{

	// getting bounding boxes for entities 
	vec2 motion1box = get_bounding_box(motion1);
	vec2 motion2box = get_bounding_box(motion2);
	// https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
	// note: dividing bounding box by 2 such that collision will register when entities are overlapping/collision detection is not too sensitive
	if (motion1.position.x < motion2.position.x + motion2box.x / 3 &&
		motion1.position.x + motion1box.x / 3 > motion2.position.x &&
		motion1.position.y < motion2.position.y + motion1box.y/2 &&
		motion1.position.y > motion2.position.y - motion1box.y/2) {
		return true;
	}

	return false;

}

// TODO: bind it with actual level width
void PhysicsSystem::player_bound() {
	for (int i = 0; i < registry.players.entities.size(); i++) {
		Entity camera = registry.cameras.entities[0];
		Camera& cam = registry.cameras.get(camera);

		Entity player = registry.players.entities[i];

		Motion& player_motion = registry.motions.get(player);
		if (player_motion.position.x <= 0 && player_motion.velocity.x < 0) {
			player_motion.velocity.x = 0;
		}
		if (player_motion.position.x >= cam.xBound[1] + 1200 && player_motion.velocity.x > 0) {
			player_motion.velocity.x = 0;

		}
		if (player_motion.position.y <= 0 && player_motion.velocity.y < 0) {
			player_motion.velocity.y = 0;

		}

		// in tutorial mode we will disable scrolling 
		if (player_motion.position.x >= 1163 && tutorial.in_tutorial_mode && player_motion.position.y == SPIDER_DEFAULT_Y) {
			player_motion.position --;
			player_motion.velocity.x = 0;
		}
	}
}

void PhysicsSystem::player_ground(float window_height_px) {
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{

		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		// spider cannot penetrate ground
		if (registry.players.has(entity) && !registry.deathTimers.has(entity)) {
			PlayerStatus& status = registry.playerStatus.get(entity);
			if (motion.position.y > SPIDER_DEFAULT_Y) {
				// stop spider from moving and reset its default y position
				motion.velocity.y = 0;
				motion.position.y = SPIDER_DEFAULT_Y;
				// reset angle
				motion.angle = 0;


				// reset falling status 
				status.falling = false;
				//motion.velocity.x = 0;
				// go back to idle animation
				Animation& animate = registry.animation.get(entity);
				animate.SPIDER_IDLE = 1;
				animate.SPIDER_LEFT, animate.SPIDER_RIGHT, animate.SPIDER_DOWN, animate.SPIDER_UP = 0;
		
			}
		}
	}
}

void PhysicsSystem::groundCollide(Entity& e, Motion& motion) {
	if (registry.physics.has(e)){
		if (motion.position.y >= 700.f) {
			motion.velocity = { 0,0 }; //no friction yet, so to prevent sliding
			motion.position.y = 700.f;
			registry.physics.get(e).gravity_enabled = false;
		}
	}
}

void PhysicsSystem::spider_trees_climb(const Entity& entity_j, const Entity& entity_i, Motion& motion_i, Motion& motion_j, float window_height_px)
{
	// if the spider and tree collide, and the tree is the currentclimbable tree, then
	// once they stop colliding, remove currentclimbable component and stop from moving up. 
	bool spider_tree_collide = registry.players.has(entity_j) && registry.climbable.has(entity_i);
	if (spider_tree_collide && registry.webString.size()==0) {
		// if we were colliding with tree but now aren't, add gravity to spider
		if (!collidesTree(motion_i, motion_j) && registry.CurrentlyBeingClimbed.has(entity_i)) {
			Motion& spider = motion_j;
			Motion& tree = motion_i;
			PlayerStatus& spider_status = registry.playerStatus.get(entity_j);
			spider_status.canClimb = false;
			spider.velocity.y = 0;

			spider_falling(spider, spider_status, entity_j);
			registry.CurrentlyBeingClimbed.remove(entity_i);
		}
	}
}

void PhysicsSystem::spider_falling(Motion& spider, PlayerStatus& spider_status, const Entity& entity_j)
{
	if (spider.position.y < SPIDER_DEFAULT_Y) {
		// add gravity
		spider.velocity.y += GRAVITY;
		// disable 'S' key 
		spider_status.falling = true;
		// play sound effect
		Mix_Chunk* spider_falling;
		spider_falling = Mix_LoadWAV(audio_path("spider_falling.wav").c_str());
		Mix_PlayChannel(-1, spider_falling, 0);
		// flip spider upside down
		spider.angle = 3.1415f;


		// reset to idle animations
		Animation& animate = registry.animation.get(entity_j);
		animate.SPIDER_IDLE = 1;
		animate.SPIDER_LEFT, animate.SPIDER_RIGHT, animate.SPIDER_DOWN, animate.SPIDER_UP = 0;

	}
}

void PhysicsSystem::zoomModePositions(float elapsed_ms) {
	auto& motion_registry = registry.motions;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		(void)elapsed_ms; // placeholder to silence unused warning until implemented
		player_bound();

		if (registry.webshoots.has(entity)) {
			motion.position += (step_seconds * motion.velocity);
		}
	}
}

void PhysicsSystem::step(float elapsed_ms, float window_width_px, float window_height_px)
{
	// Move spider based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	for(uint i = 0; i< motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = 1.0f * (elapsed_ms / 1000.f);
		(void)elapsed_ms; // placeholder to silence unused warning until implemented
		player_bound();

		// in zoom mode and not paused
		// not be in zoom mode 
		bool o1 = !delay.zoomPause && delay.zoomMode;
		bool o2 = delay.zoomPause;
		bool doNotUpdateTheseEntitiesWhileGamePaused = registry.players.has(entity) || registry.enemy.has(entity) || registry.friendly.has(entity) || registry.trackingProjectile.has(entity) || registry.indestructibleEnemy.has(entity);
		bool spidershouldfall = registry.players.has(entity) && registry.motions.get(entity).position.y < SPIDER_DEFAULT_Y;
		bool spiderPendulum = false;
		if (registry.players.has(entity)) {
			if (registry.playerStatus.get(entity).swing == true) {
				spiderPendulum = true;
			}
			else false;
		}
		bool move = false;
		if (registry.enemy.has(entity) || registry.friendly.has(entity) || registry.trackingProjectile.has(entity) || registry.indestructibleEnemy.has(entity)) {
			if (delay.beeDodge) {
				move = true;
			}
		}

		if ((!delay.zoomPause && delay.zoomMode) || (o2 && !doNotUpdateTheseEntitiesWhileGamePaused) || !delay.zoomMode || spiderPendulum || spidershouldfall || move) {
			motion.position += (step_seconds * motion.velocity);
		}

		if (registry.physics.has(entity)) {
			auto& physics = registry.physics.get(entity);
			if (physics.gravity_enabled) {
				motion.velocity.y += 9.81;
			}
			groundCollide(entity, motion); // I dont want to mess with the spider movement stuff so for now this will only apply to physics entities
		}
		player_ground(window_height_px);
	}

	// Check for collisions between all moving entities
    ComponentContainer<Motion> &motion_container = registry.motions;
	for(uint i = 0; i<motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];
		if (registry.physics.has(entity_i) && !registry.physics.get(entity_i).collisions_enabled)
			continue;
		for(uint j = 0; j<motion_container.components.size(); j++) // i+1
		{
			if (i == j)
				continue;

			Motion& motion_j = motion_container.components[j];
			Entity entity_j = motion_container.entities[j];

			if (registry.physics.has(entity_j) && !registry.physics.get(entity_j).collisions_enabled)
				continue;

			spider_trees_climb(entity_j, entity_i, motion_i, motion_j, window_height_px);
			if (registry.climbable.has(entity_i) || registry.climbable.has(entity_j)) {
				if (collidesTree(motion_i, motion_j)) {
					registry.collisions.emplace_with_duplicates(entity_i, entity_j);
					registry.collisions.emplace_with_duplicates(entity_j, entity_i);
				}
			}
			
			else if (collides(motion_i, motion_j)) {
				if (registry.players.has(entity_j) && registry.enemy.has(entity_i)) {
					int flag = 0;
					auto& enemyMeshPtrs = registry.meshPtrs.get(entity_i);
					std::vector<ColoredVertex>& enemyMeshVertices = enemyMeshPtrs->vertices;
					for (int i = 0; i < enemyMeshVertices.size(); i++) {
						vec3 enemy_point_world = convertToWorld(registry.motions.get(entity_i), vec3(enemyMeshVertices[i].position.x, enemyMeshVertices[i].position.y, 0));
						//std::cout << enemy_point_world.x << " enemy_point_world " << enemy_point_world.y << std::endl;
						if (IsPointInPolygon(vec2(enemy_point_world.x, enemy_point_world.y), entity_j)) {
							//std::cout << "Spider collides with the bee" << std::endl;
							
							registry.collisions.emplace_with_duplicates(entity_i, entity_j);
							registry.collisions.emplace_with_duplicates(entity_j, entity_i);
							
							
						}
					}
				}
				else {
					// Create a collisions event
					// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
					registry.collisions.emplace_with_duplicates(entity_i, entity_j);
					registry.collisions.emplace_with_duplicates(entity_j, entity_i);
				}
			}
			
		}
	}
	// debugging of bounding boxes
	if (debugging.in_debug_mode) {

		// Debugging for SPIDER mesh
		Entity player_mesh_entity = registry.players.entities[0];
		Motion& mesh_motion = registry.motions.get(player_mesh_entity);
		drawBoundingBox(mesh_motion);
		auto& meshPtrs = registry.meshPtrs.get(player_mesh_entity);
		std::vector<ColoredVertex>& meshVertices = meshPtrs->vertices;
		auto& vert_indices = meshPtrs->vertex_indices;
		//std::cout << vert_indices.size() << std::endl;

		/*for (int j = 0; j < vert_indices.size(); j++) {
			std::cout << vert_indices[j] << std::endl;
		}*/
		for (int k = 0; k < meshVertices.size(); k++) {
			Entity cam = registry.cameras.entities[0];
			Motion& cam_motion = registry.motions.get(cam);
			vec3 camPos = { cam_motion.position.x, cam_motion.position.y, 0 };


			vec3 spider_world_vert = convertToWorld(mesh_motion, meshVertices[k].position);
			spider_world_vert = spider_world_vert - camPos;

			Entity line = createLine({ spider_world_vert.x, spider_world_vert.y },
				{ mesh_motion.scale.x / 30, mesh_motion.scale.y / 30 });
		}
		
		// First we check the bounding box of the bee, if it goes beyond the left side of screen,
		// we find the associated mesh and delete it from beeMesh
		ComponentContainer<Enemy>& bee_container = registry.enemy;
		for (int i = 0; i < bee_container.size(); i++) {
			Entity bee_entity = bee_container.entities[i];
			Motion& motion = registry.motions.get(bee_entity);

			drawBoundingBox(motion);

			// This doesn't work, once the first bee goes beyond the meshes for the other bees disappear too
			// might be worth trying to get it right for efficiency 

			//// if the left side of bounding box goes beyond the left side of screen
			//if (registry.motions.get(line_left).position.x < 0) {
			//	ComponentContainer<EnemyMeshDecision>& enemy_mesh_container = registry.aiMeshDecisions;
			//	for (int i = 0; i < enemy_mesh_container.size(); i++) {
			//		Entity enemy_mesh_entity = enemy_mesh_container.entities[i];
			//		if (registry.aiMeshDecisions.get(enemy_mesh_entity).bee = bee_entity) {
			//			// removing the mesh from the screen to save memory 
			//			registry.beeMesh.remove(enemy_mesh_entity);
			//			registry.aiMeshDecisions.remove(enemy_mesh_entity);
			//			registry.motions.remove(enemy_mesh_entity);
			//		}
			//	}
			//}
		}

		// Debugging for BEE mesh
		ComponentContainer<Enemy>& bee_mesh_container = registry.enemy;
		for (uint i = 0; i < bee_mesh_container.size(); i++) {

			Entity bee_mesh_entity = bee_mesh_container.entities[i];
			Motion& bee_mesh_motion = registry.motions.get(bee_mesh_entity);

			

			auto& meshPtrs = registry.meshPtrs.get(bee_mesh_entity);
			std::vector<ColoredVertex>& meshVertices = meshPtrs->vertices;
			/*auto& vert_indices = meshPtrs->vertex_indices;
			for (int j = 0; j < vert_indices.size(); j++) {
				std::cout << vert_indices[j] << std::endl;
			}*/
			for (int j = 0; j < meshVertices.size(); j++) {
				Entity cam = registry.cameras.entities[0];
				Motion& cam_motion = registry.motions.get(cam);
				vec3 camPos = { cam_motion.position.x, cam_motion.position.y, 0 };
				vec3 bee_world_vert = convertToWorld(bee_mesh_motion, meshVertices[j].position);
				bee_world_vert = bee_world_vert - camPos;
				Entity line = createLine({ bee_world_vert.x, bee_world_vert.y },
					{ bee_mesh_motion.scale.x / 30, bee_mesh_motion.scale.y / 30 });
			}
		}
	}
}


void PhysicsSystem::drawBoundingBox(Motion motion) {
	Entity cam = registry.cameras.entities[0];
	Motion& cam_motion = registry.motions.get(cam);
	vec3 camPos = { cam_motion.position.x, cam_motion.position.y, 0 };

	const vec2 bounding_box = get_bounding_box(motion);
	float radius = sqrt(dot(bounding_box / 2.f, bounding_box / 2.f));

	vec2 line_scale1 = { motion.scale.x / 25, 2 * radius };
	Entity line_right = createLine({ motion.position.x + radius - camPos.x, motion.position.y - camPos.y }, line_scale1);
	Entity line_left = createLine({ motion.position.x - radius - camPos.x, motion.position.y - camPos.y }, line_scale1);

	vec2 line_scale2 = { 2 * radius, motion.scale.y / 25 };
	Entity line_top = createLine({ motion.position.x - camPos.x, (motion.position.y) - radius - camPos.y }, line_scale2);
	Entity line_bottom = createLine({ motion.position.x - camPos.x, (motion.position.y) + radius - camPos.y }, line_scale2);
}


vec3 PhysicsSystem::convertToWorld(Motion motion, vec3 vertexPos) {
	mat3 S = { { motion.scale.x, 0.f, 0.f },{ 0.f, motion.scale.y, 0.f },{ 0.f, 0.f, 1.f } };
	float c = cosf(motion.angle);
	float s = sinf(motion.angle);
	mat3 R = { { c, s, 0.f },{ -s, c, 0.f },{ 0.f, 0.f, 1.f } };

	vec3 pos = { motion.position.x, motion.position.y, 0 };

	Entity cam = registry.cameras.entities[0];
	Motion& cam_motion = registry.motions.get(cam);
	vec3 camPos = { cam_motion.position.x, cam_motion.position.y, 0 };

	vec3 ret_world_vertex = S * (vertexPos);
	ret_world_vertex = R * ret_world_vertex;
	ret_world_vertex = ret_world_vertex + pos;

	return ret_world_vertex;
}

bool PhysicsSystem::IsPointInPolygon(vec2 p, Entity entity)
{
	/*
	double minX = 0;
	double maxX = 0;
	double minY = 0;
	double maxY = 0;

	auto& playerMeshPtrs = registry.meshPtrs.get(entity);
	std::vector<ColoredVertex>& playerMeshVertices = playerMeshPtrs->vertices;

	for (int i = 0; i < playerMeshVertices.size(); i++)
	{
		vec2 q_local = playerMeshVertices[i].position;
		vec3 q_world = convertToWorld(registry.motions.get(entity), vec3(q_local.x, q_local.y, 0));
		vec2 q = vec2(q_world.x, q_world.y);
		//std::cout << q.x << " q " << q.y << std::endl;

		if (q.x < minX) {
			minX = q.x;
		}
		
		if (q.x > maxX) {
			maxX = q.x;
		}
		if (q.y < minY) {
			minY = q.y;
		}
		if (q.y > maxY) {
			maxY = q.y;
		}
	}

	if (p.x < minX || p.x > maxX || p.y < minY || p.y > maxY)
	{
		return false;
	}*/
	
	auto& playerMeshPtrs = registry.meshPtrs.get(entity);
	std::vector<ColoredVertex>& playerMeshVertices = playerMeshPtrs->vertices;
	// https://wrf.ecse.rpi.edu/Research/Short_Notes/pnpoly.html
	bool inside = false;
	for (int i = 0, j = playerMeshVertices.size() - 1; i < playerMeshVertices.size(); j = i++)
	{
		vec2 q_local_i = playerMeshVertices[i].position;
		vec3 q_world_i = convertToWorld(registry.motions.get(entity), vec3(q_local_i.x, q_local_i.y, 0));
		vec2 q_i = vec2(q_world_i.x, q_world_i.y);

		vec2 q_local_j = playerMeshVertices[j].position;
		vec3 q_world_j = convertToWorld(registry.motions.get(entity), vec3(q_local_j.x, q_local_j.y, 0));
		vec2 q_j= vec2(q_world_j.x, q_world_j.y);
		/*float i_x = playerMeshVertices[i].position.x;
		float i_y = playerMeshVertices[i].position.y;
		float j_x = playerMeshVertices[j].position.x;
		float j_y = playerMeshVertices[j].position.y;*/
		float i_x = q_i.x;
		float i_y = q_i.y;
		float j_x = q_j.x;
		float j_y = q_j.y;
		if ((i_y > p.y) != (j_y > p.y) && 
			p.x < ((j_x - i_x) * (p.y - i_y) / (j_y - i_y) + i_x))
		{
			inside = !inside;
		}
	}

	return inside;
}


/*
vec2 PhysicsSystem::getNormalizedAxis(vec2 current, vec2 next) {
	double xAxis = -(next.y - current.y);
	double yAxis = next.x - current.x;
	double magnitude = hypot(xAxis, yAxis);

	return vec2(xAxis / magnitude, yAxis / magnitude);
}

// Using the Separating Axis Theorem (SAT) to compute precise collision 
// REFERENCES : 
// https://github.com/winstxnhdw/2d-separating-axis-theorem/blob/master/separating_axis.hpp
// https://www.sevenson.com.au/programming/sat/
// https://gist.github.com/nyorain/dc5af42c6e83f7ac6d831a2cfd5fbece

bool PhysicsSystem::checkCollisionSAT(Entity& player, Entity& enemy) {

	auto& playerMeshPtrs = registry.meshPtrs.get(player);
	std::vector<ColoredVertex>& playerMeshVertices = playerMeshPtrs->vertices;

	auto& enemyMeshPtrs = registry.meshPtrs.get(enemy);
	std::vector<ColoredVertex>& enemyMeshVertices = enemyMeshPtrs->vertices;
	
	for (size_t i = 0; i < playerMeshVertices.size(); i++) {
		vec2 current = vec2(playerMeshVertices[i].position.x, playerMeshVertices[i].position.y);
		vec2 next = vec2(playerMeshVertices[(i + 1) % playerMeshVertices.size()].position.x,
			playerMeshVertices[(i + 1) % playerMeshVertices.size()].position.y);

		//vec3 current_world = convertToWorld(registry.motions.get(player), vec3(current.x, current.y, 0.0));
		//vec3 next_world = convertToWorld(registry.motions.get(player), vec3(next.x, next.y, 0.0));


		//vec2 axis = getNormalizedAxis(vec2(current_world.x, current_world.y), vec2(next_world.x, next_world.y));
		vec2 axis = getNormalizedAxis(current, next);
		//std::cout << "axis x " << axis.x << "axis y " << axis.y << std::endl;
		double playerMeshMaxProjection = 0;
		double playerMeshMinProjection = 0;
		double enemyMeshMaxProjection = 0;
		double enemyMeshMinProjection = 0;

		for (int i = 0; i < enemyMeshVertices.size(); i++) {
			float projection = dot(axis, vec2(enemyMeshVertices[i].position));
			if (projection < enemyMeshMinProjection) {
				enemyMeshMinProjection = projection;
			}
			if (projection > enemyMeshMaxProjection) {
				enemyMeshMaxProjection = projection;
			}
		}

		if (!(playerMeshMaxProjection < enemyMeshMinProjection || playerMeshMinProjection > enemyMeshMaxProjection)) {
			return false;
		}
		else {
			return true;
		}
	}

	for (size_t i = 0; i < enemyMeshVertices.size(); i++) {
		vec2 current = vec2(enemyMeshVertices[i].position.x, enemyMeshVertices[i].position.y);
		vec2 next = vec2(enemyMeshVertices[(i + 1) % enemyMeshVertices.size()].position.x,
			enemyMeshVertices[(i + 1) % enemyMeshVertices.size()].position.y);

		vec2 axis = getNormalizedAxis(current, next);
		double playerMeshMaxProjection = 0;
		double playerMeshMinProjection = 0;
		double enemyMeshMaxProjection = 0;
		double enemyMeshMinProjection = 0;

		for (int i = 0; i < playerMeshVertices.size(); i++) {
			float projection = dot(axis, vec2(playerMeshVertices[i].position));
			if (projection < playerMeshMinProjection) {
				playerMeshMinProjection = projection;
			}
			if (projection > playerMeshMaxProjection) {
				playerMeshMaxProjection = projection;
			}
		}

		if (!(playerMeshMaxProjection < enemyMeshMinProjection || playerMeshMinProjection > enemyMeshMaxProjection)) {
			return false;
		}
		else {
			return true;
		}
	}
	return true;
}
*/