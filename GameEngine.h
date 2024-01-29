#pragma once

#include "glm/glm.hpp"
#include "dependencies/MapHelper.h"

#include <string>
#include <map>

const int NUM_HARDCODED_ACTORS = 14;

class Engine {
public:
	bool game_running = false; // is the game running? drives the start loop
	char render_map[HARDCODED_MAP_HEIGHT][HARDCODED_MAP_WIDTH + 1]; // the map to be rendered - updated each frame

	// player stuff
	glm::ivec2 updated_player_pos; // the "future" player position
	unsigned int player_health = 3; // the player's current health
	unsigned int player_score = 0; // the player's current score

	// actor stuff
	std::map<Actor*, bool> triggered_score_up; // keep track of which actors triggered a player score up

	void start(); // starts the engine
	void stop(); // stops the engine
	void render(); // render the current view
	void show_stats(); // show the player's statistics
	void prompt_player(); // prompt the player for input
	void update_positions(); // update all actor positions

	bool would_collide(Actor& actor, glm::ivec2& position); // returns whether an actor would collide at an updated position
private:
	glm::ivec2 renderSize;

	void execute_commands(Actor& trigger, std::string& dialogue);
};
