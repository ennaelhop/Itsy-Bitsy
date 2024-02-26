#pragma once

#include <array>
#include <utility>

#include "common.hpp"
#include "components.hpp"
#include "tiny_ecs.hpp"

// System responsible for setting up OpenGL and for rendering all the
// visual entities in the game
class RenderSystem {
	/**
	 * The following arrays store the assets the game will use. They are loaded
	 * at initialization and are assumed to not be modified by the render loop.
	 *
	 * Whenever possible, add to these lists instead of creating dynamic state
	 * it is easier to debug and faster to execute for the computer.
	 */
	std::array<GLuint, texture_count> texture_gl_handles;
	std::array<ivec2, texture_count> texture_dimensions;

	// Make sure these paths remain in sync with the associated enumerators.
	// Associated id with .obj path
	const std::vector < std::pair<GEOMETRY_BUFFER_ID, std::string>> mesh_paths =
	{
		  // std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SALMON, mesh_path("salmon.obj"))
		  // specify meshes of other assets here
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::SPIDER_MESH, mesh_path("spider4.0.obj")),
		std::pair<GEOMETRY_BUFFER_ID, std::string>(GEOMETRY_BUFFER_ID::BEE_MESH, mesh_path("bee2.0.obj"))
	};

	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, texture_count> texture_paths = {
			textures_path("health_container.png"),
			textures_path("health_fill.png"),
			textures_path("ants/ant.png"),
			textures_path("spider_idle/spider_idle.png"),
			textures_path("bee_charger.png"),
			textures_path("bee_ranger.png"),
			textures_path("bee_swarm.png"),
			textures_path("bee_hive.png"),
			textures_path("lord_of_bee_charge.png"),
			textures_path("lord_of_bee_range.png"),
			textures_path("bee_sting.png"),
			textures_path("level_2_assets/bush.png"),
			textures_path("level_2_assets/tall_tree_sprite.png"),
			textures_path("level_2_assets/snail.png"),
			textures_path("level_2_assets/snail_detail.png"),
			textures_path("level_2_assets/acid2.0_detail.png"),
			textures_path("level_2_assets/acid_default_text.png"),
			textures_path("level_2_assets/acid_charged_text.png"),
			textures_path("tree_bark.png"),
			textures_path("tree_leaves.png"),
			textures_path("level_1_bg/bush.png"),
			textures_path("level_1_bg/grass.png"),
			textures_path("level_1_bg/ground.png"),
			textures_path("level_1_bg/lights-1.png"),
			textures_path("level_1_bg/lights-2.png"),
			textures_path("level_1_bg/layer-1.png"),
			textures_path("level_1_bg/layer-2.png"),
			textures_path("level_1_bg/layer-3.png"),
			textures_path("level_2_bg/lvl2_3.png"),
			textures_path("level_2_bg/lvl2_4.png"),
			textures_path("level_2_bg/lvl2_5.png"),
			textures_path("level_2_bg/lvl2_6.png"),
			textures_path("help/help.png"),
			textures_path("help/help_info.png"),
			textures_path("spider_idle/spider_idle_ears_up.png"),
			textures_path("spider_left/spider_1_l.png"),
			textures_path("spider_left/spider_2_l_ears_up.png"),
			textures_path("spider_left/spider_3_l.png"),
			textures_path("spider_right/spider_1_r.png"),
			textures_path("spider_right/spider_2_r.png"),
			textures_path("spider_right/spider_3_r_ears_up.png"),
			textures_path("crosshair.png"),
			textures_path("acid.png"),
			textures_path("web_green.png"),
			textures_path("ants/antXL.png"),
			textures_path("ants/antXS.png"),
			textures_path("ants/antImmune.png"),
			textures_path("ants/antHealth.png"),
			textures_path("ants/antFast.png"),
            textures_path("game_drop_name.png"),
			textures_path("text_lord_bee_intro.png"),
			textures_path("text_lord_bee.png"),
			textures_path("text_stop.png"),
			textures_path("text_no.png"),
            textures_path("tutorial/intro_script.png"),
            textures_path("saved.png"),
			textures_path("tutorial/WASD.png"),
			textures_path("tutorial/Great!.png"),
			textures_path("tutorial/shoot.png"),
			textures_path("tutorial/bees.png"),
			textures_path("tutorial/climb.png"),
			textures_path("tutorial/keep_destroying.png"),
			textures_path("tutorial/swing.png"),
			textures_path("tutorial/ants.png"),
			textures_path("tutorial/ants_details.png"),
			textures_path("tutorial/ready.png"),
			textures_path("tutorial/skip.png"),
			textures_path("levels/level_1.png"),
			textures_path("levels/level_2.png"),
			textures_path("level2prompt.png"),
			textures_path("endOfGame.png"),
			textures_path("text.png")

	};

	std::array<GLuint, effect_count> effects;
	// Make sure these paths remain in sync with the associated enumerators.
	const std::array<std::string, effect_count> effect_paths = {
		shader_path("bee"),
		shader_path("spider_mesh"),
		shader_path("coloured"),
		shader_path("pebble"),
		shader_path("textured"),
		shader_path("background"),
		shader_path("UI"),
		shader_path("water"),
		shader_path("spider"),
		shader_path("animSprite"),
		shader_path("pointParticle"),
		shader_path("animElement")
	};

	std::array<GLuint, geometry_count> vertex_buffers;
	std::array<GLuint, geometry_count> index_buffers;
	std::array<Mesh, geometry_count> meshes;

	const vec2 windowSizeWorld = { 1200, 800 };

public:
	// Initialize the window
	bool init(int width, int height, GLFWwindow* window);

	template <class T>
	void bindVBOandIBO(GEOMETRY_BUFFER_ID gid, std::vector<T> vertices, std::vector<uint16_t> indices);

	void initializeGlTextures();

	void initializeGlEffects();

	void initializeGlMeshes();
	Mesh& getMesh(GEOMETRY_BUFFER_ID id) { return meshes[(int)id]; };

	void initializeGlGeometryBuffers();
	// Initialize the screen texture used as intermediate render target
	// The draw loop first renders to this texture, then it is used for the water
	// shader
	bool initScreenTexture();

	// Destroy resources associated to one or all entities created by the system
	~RenderSystem();

	// Draw all entities
	void draw();

	vec2 getWindowSizeWorld() { return windowSizeWorld; };
	void getWindowSizeWorld(int* w, int* h) { *w = windowSizeWorld.x; *h = windowSizeWorld.y; };

	mat3 createProjectionMatrix();

	mat3 createCameraMatrix();

	mat3 getScreenToWorldMatrix();

private:
	// Internal drawing functions for each entity type
	void drawTexturedMesh(Entity entity, const mat3& projection, const mat3& cameraMat);
	void drawParticle(Entity entity, const mat3& projection, const mat3& cameraMat);
	void drawUIElement(Entity, const mat3& projection);
	void drawToScreen();

	// Window handle
	GLFWwindow* window;
	float screen_scale;  // Screen to pixel coordinates scale factor (for apple
						 // retina display?)

	// Screen texture handles
	GLuint frame_buffer;
	GLuint off_screen_render_buffer_color;
	GLuint off_screen_render_buffer_depth;

	Entity screen_state_entity;
	
	mat3 screen_to_world_matrix;
};

bool loadEffectFromFile(
	const std::string& vs_path, const std::string& fs_path, const std::string& gs_path, GLuint& out_program);
