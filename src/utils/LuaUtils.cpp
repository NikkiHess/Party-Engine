// std library
#include <string>

// my code
#include "LuaUtils.h"
#include "../world/Actor.h"
#include "../world/Scene.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

Scene LuaUtils::currentScene;
lua_State* LuaUtils::luaState;

void LuaUtils::immediatelyStop() {
    exit(0);
}

void LuaUtils::log(const std::string& message) {
    std::cout << message << "\n";
}

void LuaUtils::logError(const std::string& message) {
    std::cerr << message << "\n";
}

luabridge::LuaRef LuaUtils::findActor(const std::string& name) {
    luabridge::LuaRef foundActor = luabridge::LuaRef(luaState);

    if (currentScene.actorsByName.find(name) != currentScene.actorsByName.end()) {
        Actor actor = **(currentScene.actorsByName[name].begin());

        // push the actor
        luabridge::push(luaState, actor);

        // create a LuaRef to return
        luabridge::LuaRef actorRef = luabridge::LuaRef::fromStack(luaState, -1);

        lua_pop(luaState, 1);

        foundActor = actorRef;
    }

    return foundActor;
}

luabridge::LuaRef LuaUtils::findAllActors(const std::string& name) {
    luabridge::LuaRef foundActors = luabridge::LuaRef(luaState);

    if (currentScene.actorsByName.find(name) != currentScene.actorsByName.end()) {
        std::set<Actor*> setOfActors = currentScene.actorsByName[name];

        int index = 1; // lua tables are 1 indexed :(
        // push the actors one by one to our foundActors table
        for (Actor* actor : setOfActors) {
            foundActors[index] = actor;
            ++index;
        }
    }

    return foundActors;
}

// establish our lua_State* and all namespaces
lua_State* LuaUtils::setupLua(lua_State* luaState) {
    // establish lua Debug namespace
    // Debug.Log and Debug.LogError
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Debug")
        .addFunction("Log", LuaUtils::log)
        .addFunction("LogError", LuaUtils::logError)
        .endNamespace();

    // establish lua Actor class
    luabridge::getGlobalNamespace(luaState)
        .beginClass<Actor>("Actor")
        .addFunction("GetName", &Actor::getName)
        .addFunction("GetID", &Actor::getID)
        .addFunction("GetComponentByKey", &Actor::getComponentByKey)
        .addFunction("GetComponent", &Actor::getComponent)
        .addFunction("GetComponents", &Actor::getComponents)
        .endClass();

    // establish lua Actor namespace (Find and FindAll)
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Actor")
        .addFunction("Find", &LuaUtils::findActor)
        .addFunction("FindAll", &LuaUtils::findAllActors)
        .endNamespace();

    // establish lua Application namespace (Find and FindAll)
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Application")
        .addFunction("Quit", &LuaUtils::immediatelyStop)
        .endNamespace();

    return luaState;
}