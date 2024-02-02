#pragma once

// std stuff
#include <string>
#include <map>

// my code
#include "Renderer.h"
#include "ConfigHelper.h"

// dependencies
#include "glm/glm.hpp"
#include "../dependencies/MapHelper.h"

class Engine {
public:
	ConfigHelper& configHelper;
	Renderer& renderer;
	bool game_running = false; // is the game running? drives the start loop
	GameState state = NORMAL;

	// player stuff
	Actor& player = hardcoded_actors.back(); // the player
	int player_health = 3; // the player's current health
	int player_score = 0; // the player's current score

	// actor stuff
	std::map<Actor*, bool> triggered_score_up; // keep track of which actors triggered a player score up

	// load the game info after everything else has been loaded
	GameInfo game_info{ player, player_health, player_score,
						state, triggered_score_up, configHelper.game_start_message,
						configHelper.game_over_bad_message, configHelper.game_over_good_message };

	Engine(Renderer& renderer, ConfigHelper& configHelper) : renderer(renderer), configHelper(configHelper) {}

	// execute the main game loop
	void start();

	// shut down the main game loop
	void stop();

	// update all Actor positions according to their velocity
	void update_positions();

	// returns whether an actor would collide given its velocity
	bool would_collide(Actor& actor);

	// handles the current state
	// NORMAL = do nothing
	// WIN or LOSE = exit 
	void handle_state();
};
