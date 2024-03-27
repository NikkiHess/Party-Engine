#pragma once

// std library
#include <string>

// my code
#include "world/Actor.h"
#include "world/Scene.h"
#include "GameState.h"

// lua
#include "lua/lua.h"
#include "LuaBridge/LuaBridge.h"

// information about the game, which can be shared between classes
class GameInfo {
public:
	GameState& state;

	Scene& scene;
};