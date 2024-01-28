#pragma once
#include "glm/glm.hpp"

class Engine {
public:
	bool game_running = true;

	void start(); // starts the engine
	void stop(); // stops the engine
	void render(); // render the current view
	void show_stats(); // show the player's statistics
	void prompt_player(); // prompt the player for input
private:
	glm::ivec2 renderSize;

	void create_render_map();
};
