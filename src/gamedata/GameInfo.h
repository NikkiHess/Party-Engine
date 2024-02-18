#pragma once

// std library
#include <string>

// my code
#include "Actor.h"
#include "Scene.h"

class Camera;

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
	Actor* player;
	GameState& state;

	Scene& scene;
	Camera& camera;
};