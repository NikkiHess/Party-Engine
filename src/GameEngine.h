#pragma once

// std stuff
#include <string>
#include <map>

// my code
#include "Renderer.h"
#include "ConfigHelper.h"
#include "Actor.h"

// dependencies
#include "glm/glm.hpp"

class Engine {
public:
	Renderer& renderer;
	ConfigHelper& config_helper;
	bool game_running = false; // is the game running? drives the start loop
	GameState state = NORMAL;

	// player stuff
	Actor* player = nullptr; // the player

	int player_health = 3; // the player's current health
	int player_score = 0; // the player's current score

	// actor stuff
	std::unordered_map<Actor*, bool> triggered_score_up; // keep track of which actors triggered a player score up

	// load the game info after everything else has been loaded
	GameInfo game_info{ player, player_health, player_score,
						state, triggered_score_up, config_helper.game_start_message,
						config_helper.game_over_bad_message, config_helper.game_over_good_message, config_helper.initial_scene };

	Engine(Renderer& renderer, ConfigHelper& config_helper) : renderer(renderer), config_helper(config_helper) {
		std::vector<Actor>& actors = config_helper.initial_scene.actors;
		// this finds the player in the actors map
		auto player_it = std::find_if(actors.begin(), actors.end(), [](Actor actor) { return actor.actor_name == "player"; });

		if (player_it == actors.end()) {
			std::cout << "error: player not defined";
			exit(0);
		}
		// sets the player from the actors vector
		player = &*player_it;
		game_info.player = player;
	}

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
private:
	// update the position of a specific actor (used in update_positions)
	void update_actor_position(Actor& actor);
};
