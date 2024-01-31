#pragma once

// std stuff
#include <string>
#include <map>

// my code
#include "GameState.h"
#include "Renderer.h"

// dependencies
#include "glm/glm.hpp"
#include "../dependencies/MapHelper.h"

class Engine {
public:
	Renderer renderer;
	bool game_running = false; // is the game running? drives the start loop

	// player stuff
	Actor& player = hardcoded_actors.back(); // the player
	int player_health = 3; // the player's current health
	int player_score = 0; // the player's current score

	// actor stuff
	std::map<Actor*, bool> triggered_score_up; // keep track of which actors triggered a player score up

	Engine(Renderer& renderer) : renderer(renderer) {

	}

	// execute the main game loop
	void start();

	// shut down the main game loop
	void stop();

	// update all Actor positions according to their velocity
	void update_positions();

	// returns whether an actor would collide given its velocity
	bool would_collide(Actor& actor);
};
