#pragma once

// std stuff
#include <vector>

// my code
#include "GameInfo.h"

// dependencies
#include "glm/glm.hpp"

class Renderer {
public:
	glm::ivec2 render_size; // the size of the rendered view

	Renderer(glm::ivec2& render_size) : render_size(render_size) {}

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

	// execute the commands contained in the dialogue string with the given
	// Actor as a trigger
	GameState execute_commands(Actor& trigger, const std::string& dialogue, GameInfo& game_info);
};