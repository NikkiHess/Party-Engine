#pragma once

#include "glm/glm.hpp"
#include "../dependencies/MapHelper.h"

#include <string>
#include <map>

const int NUM_HARDCODED_ACTORS = 14;

class Engine {
public:
	// the state the engine is in
	// normal = nothing of note happened
	// win = stop the engine with a win message
	// lose = stop the engine with a lose message
	enum GameState {
		NORMAL, WIN, LOSE
	};

	bool game_running = false; // is the game running? drives the start loop
	char render_map[HARDCODED_MAP_HEIGHT][HARDCODED_MAP_WIDTH + 1]; // the map to be rendered - updated each frame
	Actor& player = hardcoded_actors.back(); // the player

	// player stuff
	int player_health = 3; // the player's current health
	int player_score = 0; // the player's current score

	// actor stuff
	std::map<Actor*, bool> triggered_score_up; // keep track of which actors triggered a player score up

	// execute the main game loop
	void start();

	// shut down the main game loop
	void stop();

	// render the current view, returns the current GameState
	void render();

	// print any dialogue from nearby Actors, returns the current GameState
	// as a result of any dialogue commands executed via execute_commands
	GameState print_dialogue();

	// print the player's current statistics
	void print_stats();

	// prompt the player for an input command (n, e, s, w, quit)
	// does nothing and proceeds upon invalid input
	void prompt_player();

	// update all Actor positions according to their velocity
	void update_positions();

	// returns whether an actor would collide given its velocity
	bool would_collide(Actor& actor);
private:
	glm::ivec2 renderSize;

	GameState execute_commands(Actor& trigger, std::string& dialogue);
};
