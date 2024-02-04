#pragma once

// std stuff
#include <vector>

// my code
#include "gamedata/GameInfo.h"
#include "utils/ConfigUtils.h"

// dependencies
#include "glm/glm.hpp"

class Renderer {
public:
	ConfigUtils& configUtils; // the ConfigUtils the game uses
	glm::ivec2 renderSize; // the size of the rendered view

	Renderer(ConfigUtils& configUtils) : configUtils(configUtils), renderSize(configUtils.renderSize) {}

	// render the current view, returns the current GameState
	void render(GameInfo& gameInfo);

	// print any dialogue from nearby Actors, returns the current GameState
	// as a result of any dialogue commands executed via executeCommands
	void printDialogue(GameInfo& gameInfo);

	// print the player's current statistics
	void printStats(GameInfo& gameInfo);

	// prompt the player for an input command (n, e, s, w, quit)
	// does nothing and proceeds upon invalid input
	// returns NORMAL if player doesn't quit
	// returns LOSE if player quits
	void promptPlayer(GameInfo& gameInfo);
private:

	// execute the commands contained in the dialogue string with the given
	// Actor as a trigger
	GameState executeCommands(Actor& trigger, const std::string& dialogue, GameInfo& gameInfo);
};