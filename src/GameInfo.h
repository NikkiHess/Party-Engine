#pragma once

// std library
#include <string>

// my code
#include "world/Actor.h"
#include "world/Scene.h"
#include "GameState.h"
#include "utils/config/ConfigManager.h"

// lua
#include "lua/lua.h"
#include "LuaBridge/LuaBridge.h"

// information about the game, which can be shared between classes
class GameInfo {
public:
	static inline ConfigManager* configManager;
	static inline ResourceManager* resourceManager;

	static inline GameState state;
	static inline Scene scene;
	// the loaded scene
	static inline Scene newScene;

	// loads a new scene upon the next frame
	static void loadScene(const std::string& sceneName);

	// gets the current scene
	static const std::string& getCurrentScene();

	// set an actor to not be destroyed at the end of a scene
	static void dontDestroy(luabridge::LuaRef actorRef);
};