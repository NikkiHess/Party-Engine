// std library
#include <string>

// my code
#include "../world/Actor.h"
#include "../world/Scene.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#pragma once
class LuaUtils {
public:
	// singleton for findActor and findAllActors
	static Scene currentScene;
	static lua_State* luaState;

	// shut down the whole application right away
	static void immediatelyStop();

	// sets up lua to begin with
	static lua_State* setupLua(lua_State* luaState);

	// for lua, finds an actor by name
	static luabridge::LuaRef findActor(const std::string& name);

	// for lua, finds all actors by name (TABLE)
	static luabridge::LuaRef findAllActors(const std::string& name);

	// Debug.Log(message), prints to cout
	static void log(const std::string& message);

	// Debug.LogError(message), prints to cerr
	static void logError(const std::string& message);

};

