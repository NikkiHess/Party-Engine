#pragma once

// std library
#include <string>

// my code
#include "Actor.h"
#include "Scene.h"
#include "GameState.h"

class Camera;

// information about the game, which can be shared between classes
class GameInfo {
public:
	Actor* player;
	GameState& state;

	Scene& scene;
	Camera& camera;
};