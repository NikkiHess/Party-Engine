// std library
#include <string>
#include <unordered_set>

// my code
#include "../world/Actor.h"
#include "../world/Scene.h"
#include "../utils/config/SceneConfig.h"
#include "../components/Component.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

#pragma once
class LuaUtils {
public:
	// singleton for findActor and findAllActors
	static inline Scene* currentScene;
	static inline SceneConfig* sceneConfig;
	static inline ResourceManager* resourceManager;

	static inline int componentsAdded = 0;

	// sets up lua to begin with
	static void setupLua();

	// prints a formatted lua error message
    static void printLuaException(const luabridge::LuaException& e, const std::string& actorName, const std::string& componentName);

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

	// Actor.Instantiate(templateName), prepare to instantiate an Actor
	static luabridge::LuaRef requestInstantiateActor(const std::string& templateName);

	// actually instantiate the actor (helper)
	static void instantiateActor(std::shared_ptr<Actor> actorPtr);

	// Actor.Destroy(actor), prepare to destroy an actor
	// by disabling its components and requesting its destruction
	static void requestDestroyActor(const luabridge::LuaRef& actorRef);

	// actually complete the destruction
	static void destroyActor(std::shared_ptr<Actor> actorPtr);

	/* DEBUG NAMESPACE */

	// Debug.Log(message), prints to cout
	static void log(const std::string& message);

	// Debug.LogError(message), prints to cerr
	static void logError(const std::string& message);
};

