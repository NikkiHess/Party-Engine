#pragma once

#include "glm/glm.hpp"
#include "dependencies/MapHelper.h"

class Engine {
public:
	bool game_running = true; // is the game running? drives the start loop
	char render_map[HARDCODED_MAP_HEIGHT][HARDCODED_MAP_WIDTH + 1]; // the map to be rendered - updated each frame

	void start(); // starts the engine
	void stop(); // stops the engine
	void render(); // render the current view
	void show_stats(); // show the player's statistics
	void prompt_player(); // prompt the player for input
	void print_dialogue(); // show npc dialogue when player is nearby
private:
	glm::ivec2 renderSize;
};
