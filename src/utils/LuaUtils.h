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
	static inline Scene* currentScene;
	static inline lua_State* luaState;

	static inline int componentsAdded = 0;

	// sets up lua to begin with
	static lua_State* setupLua(lua_State* luaState);


	/* APPLICATION NAMESPACE */

	// Application.Quit(), shut down the whole application right away
	static void quit();

	// Application.Sleep(ms), sleep for the given ms
	static void sleep(const int ms);

	// Application.GetFrame(), get the current frame
	static int getFrame();

	// Application.OpenURL(url), opens the URL in user's default browser
	static void openURL(const std::string& url);


	/* ACTOR NAMESPACE */

	// Actor.Find(name), finds an actor by name
	static luabridge::LuaRef findActor(const std::string& name);

	// Actor.FindAll(name), finds all actors by name (TABLE)
	static luabridge::LuaRef findAllActors(const std::string& name);

	// Debug.Log(message), prints to cout
	static void log(const std::string& message);

	// Debug.LogError(message), prints to cerr
	static void logError(const std::string& message);

};

