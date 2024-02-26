// internal
#include "render_system.hpp"
#include <world_system.hpp>
#include <SDL.h>

#include "tiny_ecs_registry.hpp"

void RenderSystem::drawTexturedMesh(Entity entity,
									const mat3 &projection,
									const mat3 &cameraMat)
{
	
	assert(registry.renderRequests.has(entity));
	RenderRequest &render_request = registry.renderRequests.get(entity);

	Motion &motion = registry.motions.get(entity);
	Transform transform;
	
	//if (render_request.used_effect == EFFECT_ASSET_ID::BACKGROUND) {//FOR BACKGROUNDS, Y is actually being used as depth, not pos
	//	transform.translate(vec2(motion.position.x, 0));
	//}
	//else {
	transform.translate(motion.position);
	transform.rotate(motion.angle);
	//}
	
	transform.scale(motion.scale);

	const GLuint used_effect_enum = (GLuint)render_request.used_effect;
	assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
	const GLuint program = (GLuint)effects[used_effect_enum];

	// Setting shaders
	glUseProgram(program);
	gl_has_errors();

	assert(render_request.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
	const GLuint vbo = vertex_buffers[(GLuint)render_request.used_geometry];
	const GLuint ibo = index_buffers[(GLuint)render_request.used_geometry];

	// Setting vertex and index buffers
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	gl_has_errors();

	// Input data location as in the vertex buffer
	if (render_request.used_effect == EFFECT_ASSET_ID::TEXTURED
		|| render_request.used_effect == EFFECT_ASSET_ID::BACKGROUND
		|| render_request.used_effect == EFFECT_ASSET_ID::UI
		|| render_request.used_effect == EFFECT_ASSET_ID::SPIDER
		|| render_request.used_effect == EFFECT_ASSET_ID::ANIM_SPRITE)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_texcoord_loc = glGetAttribLocation(program, "in_texcoord");
		gl_has_errors();
		assert(in_texcoord_loc >= 0);

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		assert(registry.renderRequests.has(entity));
		GLuint texture_id =
			texture_gl_handles[(GLuint)registry.renderRequests.get(entity).used_texture];

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		if (render_request.used_effect == EFFECT_ASSET_ID::BACKGROUND) {
			GLuint depth_loc = glGetUniformLocation(program, "depth");
			GLuint view_loc = glGetUniformLocation(program, "viewPos");
			GLuint light_pos_loc = glGetUniformLocation(program, "lightPos");
			GLuint level_loc = glGetUniformLocation(program, "level");
			assert(depth_loc >= 0);
			float depth = 0;
			if (registry.backgroundDepths.has(entity)) {
				depth = registry.backgroundDepths.get(entity).depth;
			}
			glUniform1f(depth_loc, depth);

			Entity cam = registry.cameras.entities.front();
			vec2 campos = registry.motions.get(cam).position;
			if (campos.x < 0.0f)
			{
				vec2 tempPos = {0.0f, campos.y};
				glUniform2fv(view_loc, 1, (float *)&tempPos);
			} else {
				glUniform2fv(view_loc, 1, (float *)&campos);
			}
			
			Entity player = registry.players.entities.front();
			vec2 pos = registry.motions.get(player).position;
			glUniform2fv(light_pos_loc,1,(float *)&pos);
			glUniform1i(level_loc, tutorial.in_tutorial_mode);

			gl_has_errors();
		}

		if (render_request.used_effect == EFFECT_ASSET_ID::ANIM_SPRITE) {
			auto& spriteSheet = registry.spriteSheets.get(entity);

			GLuint textureRes_loc = glGetUniformLocation(program, "textureRes");
			GLuint spriteRes_loc = glGetUniformLocation(program, "spriteRes");
			GLuint index_loc = glGetUniformLocation(program, "frameIndex");
			assert(textureRes_loc >= 0 && spriteRes_loc >= 0);
			glUniform2fv(textureRes_loc, 1, (float*)&spriteSheet.textureRes);
			glUniform2fv(spriteRes_loc, 1, (float*)&spriteSheet.spriteRes);
			glUniform1iv(index_loc, 1, &spriteSheet.index);

			gl_has_errors();
		}
		if (render_request.used_effect == EFFECT_ASSET_ID::SPIDER) {
			const GLuint spider_program = effects[(GLuint)EFFECT_ASSET_ID::SPIDER];
			GLuint time_uloc = glGetUniformLocation(program, "time");

			// adding effect
			GLint glow_uloc = glGetUniformLocation(program, "glow");
			assert(glow_uloc >= 0);

			// Set clock
			glUniform1f(time_uloc, (float)(glfwGetTime()));
			if (registry.spiderEffect.has(entity)) {
				SpiderEffectTimer& sp = registry.spiderEffect.get(entity);
				if (sp.glow) {
					glUniform1i(glow_uloc, 1);
				}
			}
			else {
				glUniform1i(glow_uloc, 0);
			}

			gl_has_errors();
		}
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::SPIDER_MESH ||
			render_request.used_effect == EFFECT_ASSET_ID::BEE_MESH) {
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();
	}
	else if (render_request.used_effect == EFFECT_ASSET_ID::DEBUG)
	{
		GLint in_position_loc = glGetAttribLocation(program, "in_position");
		GLint in_color_loc = glGetAttribLocation(program, "in_color");
		gl_has_errors();

		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_color_loc);
		glVertexAttribPointer(in_color_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(ColoredVertex), (void*)sizeof(vec3));
		gl_has_errors();

	}
	else
	{
		assert(false && "Type of render request not supported");
	}

	// Getting uniform locations for glUniform* calls
	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = registry.colors.has(entity) ? registry.colors.get(entity) : vec3(1);
	glUniform3fv(color_uloc, 1, (float *)&color);
	gl_has_errors();

	// Get number of indices from index buffer, which has elements uint16_t
	GLint size = 0;
	glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	gl_has_errors();

	GLsizei num_indices = size / sizeof(uint16_t);
	// GLsizei num_triangles = num_indices / 3;

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	// Setting uniform values to the currently bound program
	GLuint transform_loc = glGetUniformLocation(currProgram, "transform");
	glUniformMatrix3fv(transform_loc, 1, GL_FALSE, (float *)&transform.mat);
	GLuint camera_loc = glGetUniformLocation(currProgram, "camera");
	glUniformMatrix3fv(camera_loc, 1, GL_FALSE, (float*)&cameraMat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float *)&projection);
	gl_has_errors();
	// Drawing of num_indices/3 triangles specified in the index buffer
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr);
	gl_has_errors();
}

void RenderSystem::drawParticle(Entity entity, const mat3& projection, const mat3& cameraMat) {
	auto& emitter = registry.emitters.get(entity);
	std::vector<Transform> transforms;
	int instances = 0;
	for (Entity e : emitter.particles) {
		auto& motion = registry.motions.get(e);
		Transform t;
		t.translate(motion.position);
		transforms.push_back(t);
		instances++;
	}

	//glPointSize(15);

	// Setting shaders
	const GLuint program = effects[(GLuint)EFFECT_ASSET_ID::PARTICLE];
	glUseProgram(program);

	const GLuint vbo = vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::POINT_PARTICLE];
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	gl_has_errors();

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
		sizeof(ColoredVertex), (void*)0);
	gl_has_errors();

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,
		sizeof(ColoredVertex), (void*)sizeof(vec3));
	gl_has_errors();

	GLint color_uloc = glGetUniformLocation(program, "fcolor");
	const vec3 color = emitter.colour;
	glUniform3fv(color_uloc, 1, (float*)&color);
	gl_has_errors();

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, instances * sizeof(glm::mat3), transforms.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0);
	glEnableVertexAttribArray(2 + 1);
	glVertexAttribPointer(2 + 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(1 * sizeof(glm::vec3)));
	glEnableVertexAttribArray(2 + 2);
	glVertexAttribPointer(2 + 2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec3)));

	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(2 + 1, 1);
	glVertexAttribDivisor(2 + 2, 1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLint currProgram;
	glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
	GLuint camera_loc = glGetUniformLocation(currProgram, "camera");
	glUniformMatrix3fv(camera_loc, 1, GL_FALSE, (float*)&cameraMat);
	GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
	glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
	GLuint time_loc = glGetUniformLocation(currProgram, "time");
	glUniform1fv(time_loc, 1, &emitter.lifetime);
	gl_has_errors();

	glDrawArraysInstanced(GL_POINTS, 0, 1, instances);
}

void RenderSystem::drawUIElement(Entity entity, const mat3& projection) 
{
	UIRenderRequest rr = registry.uiRenderRequests.get(entity);
	if (rr.used_effect == EFFECT_ASSET_ID::ANIM_ELEMENT && rr.used_texture == TEXTURE_ASSET_ID::TEXT) {
		int instances;
		char* str;
		std::vector<int> frameIndices;
		std::vector<Transform> transforms;
		auto& pos = registry.uiPositions.get(entity);

		if (registry.stopwatches.has(entity))
		{
			instances = 5;
			float timer = registry.stopwatches.get(entity).counter;
			str = (char*)malloc(6); //size 6 because null terminator
			assert(str != 0); //make sure this has been properly allocated to suppress warnings
			sprintf(str, "%02d:%02d", (int)(timer / 60000.f), ((int)(timer / 1000.f)) % 60); //ugly as sin but it works lol
			//std::cout << "\rTime: " << str;

			
		}
		else
		{
			auto& textElem = registry.textElements.get(entity);
			if (textElem.visible) {
				instances = textElem.content.length();
				str = (char*)malloc((size_t) instances + 1);
				assert(str != 0); //make sure this has been properly allocated to suppress warnings
				strcpy(str, textElem.content.c_str());
			}
			else {
				return;
			}
		}
		vec2 char_scale = vec2(pos.scale.x / (float)instances, pos.scale.y);
		for (int i = 0; i < instances; i++) {
			frameIndices.push_back((int)str[i]);

			float x = pos.position.x + (pos.scale.x / 2.f) * (-1.f + ((float)(2 * i + 1) / (float)instances));
			vec2 char_pos = vec2(x, pos.position.y);
			Transform t;
			t.translate(char_pos);
			t.scale(char_scale);
			transforms.push_back(t);
		}
		free(str); // we have the indices, so we don't need the string anymore.
		const GLuint used_effect_enum = (GLuint)rr.used_effect;
		assert(used_effect_enum != (GLuint)EFFECT_ASSET_ID::EFFECT_COUNT);
		const GLuint program = (GLuint)effects[used_effect_enum];

		// Setting shaders
		glUseProgram(program);
		gl_has_errors();

		assert(rr.used_geometry != GEOMETRY_BUFFER_ID::GEOMETRY_COUNT);
		const GLuint vbo = vertex_buffers[(GLuint)rr.used_geometry];
		const GLuint ibo = index_buffers[(GLuint)rr.used_geometry];

		// Setting vertex and index buffers
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		gl_has_errors();

		GLint in_position_loc = 0;
		GLint in_texcoord_loc = 1;
		GLint in_index_loc = 2;
		GLint in_transform_loc = 3;
		gl_has_errors();
		glEnableVertexAttribArray(in_position_loc);
		glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE,
			sizeof(TexturedVertex), (void*)0);
		gl_has_errors();

		glEnableVertexAttribArray(in_texcoord_loc);
		glVertexAttribPointer(
			in_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(TexturedVertex),
			(void*)sizeof(
				vec3)); // note the stride to skip the preceeding vertex position
		// Enabling and binding texture to slot 0
		glActiveTexture(GL_TEXTURE0);
		gl_has_errors();

		GLuint texture_id =
			texture_gl_handles[(GLuint)rr.used_texture];

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

		glBindTexture(GL_TEXTURE_2D, texture_id);
		gl_has_errors();

		unsigned int indexBuffer;
		glGenBuffers(1, &indexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, indexBuffer);
		glBufferData(GL_ARRAY_BUFFER, instances * sizeof(int), frameIndices.data(), GL_STATIC_DRAW);
		gl_has_errors();
		glEnableVertexAttribArray(in_index_loc);
		gl_has_errors();
		glVertexAttribIPointer(in_index_loc, 1, GL_INT, 1 * sizeof(GL_INT), (void*)0);
			

		unsigned int transformBuffer;
		glGenBuffers(1, &transformBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
		glBufferData(GL_ARRAY_BUFFER, instances * sizeof(glm::mat3), transforms.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(in_transform_loc);
		glVertexAttribPointer(in_transform_loc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)0);
		glEnableVertexAttribArray(in_transform_loc + 1);
		glVertexAttribPointer(in_transform_loc + 1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(1 * sizeof(glm::vec3)));
		glEnableVertexAttribArray(in_transform_loc + 2);
		glVertexAttribPointer(in_transform_loc + 2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(glm::vec3), (void*)(2 * sizeof(glm::vec3)));
		gl_has_errors();

		glVertexAttribDivisor(in_index_loc, 1);
		glVertexAttribDivisor(in_transform_loc, 1);
		glVertexAttribDivisor(in_transform_loc + 1, 1);
		glVertexAttribDivisor(in_transform_loc + 2, 1);
		gl_has_errors();

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		gl_has_errors();

		// Get number of indices from index buffer, which has elements uint16_t
		GLint size = 0;
		glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
		gl_has_errors();

		GLsizei num_indices = size / sizeof(uint16_t);
		// GLsizei num_triangles = num_indices / 3;

		GLint currProgram;
		glGetIntegerv(GL_CURRENT_PROGRAM, &currProgram);
		// Setting uniform values to the currently bound program
		GLuint projection_loc = glGetUniformLocation(currProgram, "projection");
		glUniformMatrix3fv(projection_loc, 1, GL_FALSE, (float*)&projection);
		SpriteSheet& spriteSheet = registry.spriteSheets.get(entity);
		vec2 textureRes = spriteSheet.textureRes;
		vec2 spriteRes = spriteSheet.spriteRes;
		GLuint textureRes_loc = glGetUniformLocation(program, "textureRes");
		GLuint spriteRes_loc = glGetUniformLocation(program, "spriteRes");
		glUniform2fv(textureRes_loc, 1, (float*)&textureRes);
		glUniform2fv(spriteRes_loc, 1, (float*)&spriteRes);
		gl_has_errors();

		// Drawing of num_indices/3 triangles specified in the index buffer
		glDrawElementsInstanced(GL_TRIANGLES, num_indices, GL_UNSIGNED_SHORT, nullptr, instances);
		gl_has_errors();
	}
	else
	{
		assert(false && "Type of render request not supported");
	}
}

// draw the intermediate texture to the screen
void RenderSystem::drawToScreen()
{
	// Setting shaders
	// get the water texture, sprite mesh, and program
	glUseProgram(effects[(GLuint)EFFECT_ASSET_ID::WATER]);
	gl_has_errors();
	// Clearing backbuffer
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, w, h);
	glDepthRange(0, 10);
	glClearColor(1.f, 1.f, 1.f, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	gl_has_errors();
	// Enabling alpha channel for textures
	glDisable(GL_BLEND);
	// glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// Draw the screen texture on the quad geometry
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]);
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER,
		index_buffers[(GLuint)GEOMETRY_BUFFER_ID::SCREEN_TRIANGLE]); // Note, GL_ELEMENT_ARRAY_BUFFER associates
																	 // indices to the bound GL_ARRAY_BUFFER
	gl_has_errors();
	const GLuint water_program = effects[(GLuint)EFFECT_ASSET_ID::WATER];
	// Set clock
	GLuint time_uloc = glGetUniformLocation(water_program, "time");
	GLuint dead_timer_uloc = glGetUniformLocation(water_program, "darken_screen_factor");
	glUniform1f(time_uloc, (float)(glfwGetTime() * 10.0f));
	ScreenState &screen = registry.screenStates.get(screen_state_entity);
	glUniform1f(dead_timer_uloc, screen.darken_screen_factor);
	gl_has_errors();
	// Set the vertex position and vertex texture coordinates (both stored in the
	// same VBO)
	GLint in_position_loc = glGetAttribLocation(water_program, "in_position");
	glEnableVertexAttribArray(in_position_loc);
	glVertexAttribPointer(in_position_loc, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
	gl_has_errors();

	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, off_screen_render_buffer_color);
	gl_has_errors();
	// Draw
	glDrawElements(
		GL_TRIANGLES, 3, GL_UNSIGNED_SHORT,
		nullptr); // one triangle = 3 vertices; nullptr indicates that there is
				  // no offset from the bound index buffer
	gl_has_errors();
}

// Render our game world
// http://www.opengl-tutorial.org/intermediate-tutorials/tutorial-14-render-to-texture/
void RenderSystem::draw()
{
	// Getting size of window
	int w, h;
	glfwGetFramebufferSize(window, &w, &h);

	//The following viewport functionality was based off David Amador's work,
	// https://dzone.com/articles/opengl-2d-independent

	int virtual_W, virtual_H;

	if ((float)w / (float)h < 1) {
		virtual_W = w;
		virtual_H = (2.f / 3.f) * w;
	}
	else {
		virtual_H = h;
		virtual_W = (3.f / 2.f) * h;
	}

	int vp_x = (w / 2) - (virtual_W / 2);
	int vp_y = (h / 2) - (virtual_H / 2);

	glViewport(vp_x, h - (vp_y + virtual_H), virtual_W, virtual_H);

	// First render to the custom framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer);
	gl_has_errors();
	// Clearing backbuffer
	//glViewport(0, 0, w, h);
	glDepthRange(0.00001, 10);
	glClearColor(0, 0, 0, 1.0);
	glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST); // native OpenGL does not work with a depth buffer
							  // and alpha blending, one would have to sort
							  // sprites back to front
	gl_has_errors();
	mat3 projection_2D = createProjectionMatrix();
	mat3 camera_mat = createCameraMatrix();

	std::vector<Entity> ui_render_requests;
	std::vector<Entity> entity_render_requests;
	std::vector<Entity> particle_render_requests;
	for (Entity entity : registry.renderRequests.entities) {
		const RenderRequest& render_request = registry.renderRequests.get(entity);
		if (render_request.used_effect == EFFECT_ASSET_ID::UI) {
			ui_render_requests.push_back(entity);
		}
		else if (render_request.used_effect == EFFECT_ASSET_ID::PARTICLE) {
			particle_render_requests.push_back(entity);
		}
		else {
			entity_render_requests.push_back(entity);
		}

	}

	// Draw all textured meshes that have a position and size component
	for (Entity entity : entity_render_requests)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D, camera_mat);
	}
	for (Entity entity : particle_render_requests)
	{
		drawParticle(entity, projection_2D, camera_mat);
	}

	auto& uiRequestRegistry = registry.uiRenderRequests;
	for (Entity entity : uiRequestRegistry.entities) {
		drawUIElement(entity, projection_2D);
	}

	for (Entity entity : ui_render_requests)
	{
		if (!registry.motions.has(entity))
			continue;
		// Note, its not very efficient to access elements indirectly via the entity
		// albeit iterating through all Sprites in sequence. A good point to optimize
		drawTexturedMesh(entity, projection_2D, camera_mat);
	}

	// Truely render to the screen
	drawToScreen();

	// flicker-free display with a double buffer
	glfwSwapBuffers(window);
	gl_has_errors();
}

mat3 RenderSystem::createProjectionMatrix()
{
	// Fake projection matrix, scales with respect to window coordinates
	float left = 0.f;
	float top = 0.f;

	float defaultW = 1200; //This is the default view size, consider pulliing it out to common.hpp
	float defaultH = 800;

	int w, h;
	glfwGetFramebufferSize(window, &w, &h);
	//glfwGetWindowSize(window, &w, &h);
	gl_has_errors();
	float right = (float)w / screen_scale;
	float bottom = (float)h / screen_scale;
	
	float dynamicScaleX = w / defaultW; //this scales our matrix properly for different resolutions.
	float dynamicScaleY = h / defaultH;

	float sx = dynamicScaleX * (2.f / (right - left));
	float sy = dynamicScaleY * (2.f / (top - bottom));
	float tx = -(right + left) / (right - left);
	float ty = -(top + bottom) / (top - bottom);
	return {{sx, 0.f, 0.f}, {0.f, sy, 0.f}, {tx, ty, 1.f}};
}

mat3 RenderSystem::createCameraMatrix()
{
	Entity cam = registry.cameras.entities.front();
	vec2 pos = registry.motions.get(cam).position;
	//float xpos = -0; 
	//float ypos = 0;
	return { {1.f,0.f,0.f}, {0.f,1.f,0.f}, {-pos.x,pos.y,1.f} };//x is negative because of our coordinate systems
}

mat3 RenderSystem::getScreenToWorldMatrix() {
	return screen_to_world_matrix;
}