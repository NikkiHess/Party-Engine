#pragma once

// my code
#include "GameInfo.h"

// dependencies
#include "../dependencies/MapHelper.h"
#include "glm/glm.hpp"

class Renderer {
public:
	char render_map[HARDCODED_MAP_HEIGHT][HARDCODED_MAP_WIDTH + 1]; // the map to be rendered - updated each frame

	Renderer(glm::ivec2& renderSize) : renderSize(renderSize) {
		// copy hardcoded map into render map
		for (int y = 0; y < HARDCODED_MAP_HEIGHT; ++y) {
			for (int x = 0; x < HARDCODED_MAP_WIDTH + 1; ++x) {
				render_map[y][x] = hardcoded_map[y][x];
			}
		}
	}

	// render the current view, returns the current GameState
	void render(GameInfo& game_info);

	// print any dialogue from nearby Actors, returns the current GameState
	// as a result of any dialogue commands executed via execute_commands
	void print_dialogue(GameInfo& game_info);

	// print the player's current statistics
	void print_stats(GameInfo& game_info);

	// prompt the player for an input command (n, e, s, w, quit)
	// does nothing and proceeds upon invalid input
	// returns NORMAL if player doesn't quit
	// returns LOSE if player quits
	void prompt_player(GameInfo& game_info);
private:
	glm::ivec2 renderSize; // the size of the rendered view

	// execute the commands contained in the dialogue string with the given
	// Actor as a trigger
	GameState execute_commands(Actor& trigger, const std::string& dialogue, GameInfo& game_info);
};