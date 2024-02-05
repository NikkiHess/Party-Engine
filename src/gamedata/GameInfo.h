#pragma once

// std library
#include <string>

// my code
#include "Scene.h"
#include "Actor.h"

// the state the engine is in
// normal = nothing of note happened
// win = stop the engine with a win message
// lose = stop the engine with a lose message
enum GameState {
	NORMAL, WIN, LOSE, PROCEED
};

// information about the game, which can be shared between classes
class GameInfo {
public:
	Actor* player; // the player
	int& playerHealth; // the player's health
	int& playerScore; // the player's score
	GameState& state; // the current state of the game

	// the message to be shown upon game start
	const std::string& gameStartMessage;
	// the message to be shown upon game end
	const std::string& gameOverBadMessage, gameOverGoodMessage;

	Scene& currentScene;
};