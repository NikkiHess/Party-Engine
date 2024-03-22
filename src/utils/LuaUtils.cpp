// std library
#include <string>
#include <thread>

// my code
#include "LuaUtils.h"
#include "../world/Actor.h"
#include "../world/Scene.h"
#include "../input/Input.h"

// dependencies
#include "Helper.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

Scene LuaUtils::currentScene;
lua_State* LuaUtils::luaState;

void LuaUtils::quit() {
    exit(0);
}

void LuaUtils::sleep(const int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int LuaUtils::getFrame() {
    return Helper::GetFrameNumber();
}

void LuaUtils::openURL(const std::string& url) {
    const std::string windows = "start " + url;
    const std::string osx = "open " + url;
    const std::string linux = "xdg-open " + url;

#if defined(_WIN32)
    std::system(windows.c_str());
#elif defined(_OSX)
    std::system(osx.c_str());
#else
    std::system(linux.c_str());
#endif
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

    // establish lua Application namespace (Quit, Sleep, GetFrame)
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Application")
        .addFunction("Quit", &LuaUtils::quit)
        .addFunction("Sleep", &LuaUtils::sleep)
        .addFunction("GetFrame", &LuaUtils::getFrame)
        .endNamespace();

    // establish lua Input namespace (GetKey, GetKeyDown, GetKeyUp)
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Input")
            .addFunction("GetKey", &Input::getKey)
            .addFunction("GetKeyDown", &Input::getKeyDown)
            .addFunction("GetKeyUp", &Input::getKeyUp)
            .addFunction("GetMousePosition", &Input::getMousePosition)
            .addFunction("GetMouseButton", &Input::getMouseButton)
            .addFunction("GetMouseButtonDown", &Input::getMouseButtonDown)
            .addFunction("GetMouseButtonUp", &Input::getMouseButtonUp)
            .addFunction("GetMouseScrollDelta", &Input::getMouseScrollDelta)
        .endNamespace();

    // expose vec2 to Lua
    luabridge::getGlobalNamespace(luaState)
        .beginClass<glm::vec2>("vec2")
            .addProperty("x", &glm::vec2::x)
            .addProperty("y", &glm::vec2::y)
        .endClass();

    return luaState;
}