#pragma once

#include <string>

// my code
#include "Scene.h"
#include "Actor.h"

// the state the engine is in
// normal = nothing of note happened
// win = stop the engine with a win message
// lose = stop the engine with a lose message
enum GameState {
	NORMAL, WIN, LOSE
};

// information about the game, which can be shared between classes
// player, player_health, player_score, triggered_score_up map
class GameInfo {
public:
	Actor* player; // the player
	int& player_health; // the player's health
	int& player_score; // the player's score
	GameState& state; // the current state of the game
	std::unordered_map<Actor*, bool>& triggered_score_up; // the Actors who have triggered score up commands

	// the message to be shown upon game start
	const std::string& game_start_message;
	// the message to be shown upon game end
	const std::string& game_over_bad_message, game_over_good_message;

	Scene& current_scene;
};