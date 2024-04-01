// std library
#include <string>
#include <thread>
#include <algorithm>
#include <unordered_map>

// my code
#include "LuaUtils.h"
#include "../world/Actor.h"
#include "../world/Scene.h"
#include "../input/Input.h"
#include "../utils/config/SceneConfig.h"
#include "../visuals/Artist.h"
#include "../utils/LuaStateSaver.h"

// dependencies
#include "Helper.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

// Box2D
#include "box2d/box2d.h"

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

luabridge::LuaRef actorToLuaRef(std::shared_ptr<Actor> actor) {
    // push the actor
    luabridge::push(LuaStateSaver::luaState, actor.get());

    // create a LuaRef to return
    luabridge::LuaRef actorRef = luabridge::LuaRef::fromStack(LuaStateSaver::luaState, -1);

    lua_pop(LuaStateSaver::luaState, 1);

    return actorRef;
}

luabridge::LuaRef LuaUtils::findActor(const std::string& name) {
    luabridge::LuaRef foundActor = luabridge::LuaRef(LuaStateSaver::luaState);

    // make sure the actor exists
    if (currentScene->actorsByName.find(name) != currentScene->actorsByName.end()) {
        // can't get begin() of an empty set
        if (currentScene->actorsByName.size() != 0) {
            std::shared_ptr<Actor> actor = *(currentScene->actorsByName[name].begin());

            // if we're gonna remove the actor, don't return it
            if (currentScene->actorsToRemove.find(actor) == currentScene->actorsToRemove.end()) {
                foundActor = actorToLuaRef(actor);
            }
        }
    }

    return foundActor;
}

luabridge::LuaRef LuaUtils::findAllActors(const std::string& name) {
    luabridge::LuaRef foundActors = luabridge::newTable(LuaStateSaver::luaState);
    int index = 1;

    if(currentScene->actorsByName.find(name) != currentScene->actorsByName.end()) {
        for (std::shared_ptr<Actor> actor : currentScene->actorsByName[name]) {
            foundActors[index] = actor.get();
            ++index;
        }
    }

    return foundActors;
}

luabridge::LuaRef LuaUtils::requestInstantiateActor(const std::string& templateName) {
    static std::unordered_map<std::string, rapidjson::Document> templateCache;

    Actor actor;
    SceneConfig* sceneConfig = LuaUtils::sceneConfig;

    std::string templatePath = "resources/actor_templates/" + templateName + ".template";

    if (!resourceManager->fileExists(templatePath)) {
        Error::error("template " + templateName + " is missing");
    }

    // search for the template in the cache
    auto it = templateCache.find(templateName);
    if (it == templateCache.end()) {
        // cache miss, read the file and add it to the cache
        rapidjson::Document document;
        JsonUtils::readJsonFile(templatePath, document);
        it = templateCache.emplace(templateName, std::move(document)).first;
    }

    sceneConfig->setActorProps(actor, it->second, *resourceManager);

    currentScene->instantiateActor(actor, false);
    currentScene->actorsToAdd.emplace(currentScene->actorsById[actor.id]);

    return actorToLuaRef(currentScene->actorsById[actor.id]);
}

void LuaUtils::instantiateActor(std::shared_ptr<Actor> actorPtr) {
    currentScene->instantiateActorLifecycle(actorPtr);
}


void LuaUtils::requestDestroyActor(const luabridge::LuaRef& actorRef) {
    std::shared_ptr actorShared = currentScene->actorsById[actorRef.cast<Actor*>()->id];

    for (auto& [key, componentPtr] : actorShared->componentPtrsByKey) {
        componentPtr->instanceTable["enabled"] = false;
    }

    currentScene->actorsToRemove.emplace(actorShared);
}

void LuaUtils::destroyActor(std::shared_ptr<Actor> actorPtr) {
    auto& actors = currentScene->actors;
    actors.erase(std::remove(actors.begin(), actors.end(), actorPtr));

    currentScene->actorsById.erase(actorPtr->id);
    currentScene->actorsByName[actorPtr->name].erase(actorPtr);
    // if, after removing the actor, no more actors exist with that name
    // erase the entry with the actor's name
    if (currentScene->actorsByName[actorPtr->name].empty()) {
        currentScene->actorsByName.erase(actorPtr->name);
    }

    currentScene->actorsByRenderOrder.erase(actorPtr);
    currentScene->actorsWithComponentsToRemove.erase(actorPtr);
    currentScene->actorsWithNewComponents.erase(actorPtr);
    currentScene->actorsWithOnLateUpdate.erase(actorPtr);
    currentScene->actorsWithOnStart.erase(actorPtr);
    currentScene->actorsWithOnUpdate.erase(actorPtr);
}

// establish our lua_State* and all namespaces
lua_State* LuaUtils::setupLua() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        // establish lua namespace: Debug
        // Debug.Log and Debug.LogError
        .beginNamespace("Debug")
            .addFunction("Log", LuaUtils::log)
            .addFunction("LogError", LuaUtils::logError)
        .endNamespace()

        // establish lua class: Actor 
        .beginClass<Actor>("Actor")
            .addFunction("GetName", &Actor::getName)
            .addFunction("GetID", &Actor::getID)
            .addFunction("GetComponentByKey", &Actor::getComponentByKey)
            .addFunction("GetComponent", &Actor::getComponent)
            .addFunction("GetComponents", &Actor::getComponents)
            .addFunction("AddComponent", &Actor::requestAddComponent)
            .addFunction("RemoveComponent", &Actor::requestRemoveComponent)
        .endClass()

        // establish lua namespace: Actor (Find and FindAll)
        .beginNamespace("Actor")
            .addFunction("Find", &LuaUtils::findActor)
            .addFunction("FindAll", &LuaUtils::findAllActors)
            .addFunction("Instantiate", &LuaUtils::requestInstantiateActor)
            .addFunction("Destroy", &LuaUtils::requestDestroyActor)
        .endNamespace()

        // establish lua namespace: Application (Quit, Sleep, GetFrame)
        .beginNamespace("Application")
            .addFunction("Quit", &LuaUtils::quit)
            .addFunction("Sleep", &LuaUtils::sleep)
            .addFunction("GetFrame", &LuaUtils::getFrame)
        .endNamespace()

        // establish lua namespace: Input (GetKey, GetKeyDown, GetKeyUp)
        .beginNamespace("Input")
            .addFunction("GetKey", &Input::getKey)
            .addFunction("GetKeyDown", &Input::getKeyDown)
            .addFunction("GetKeyUp", &Input::getKeyUp)
            .addFunction("GetMousePosition", &Input::getMousePosition)
            .addFunction("GetMouseButton", &Input::getMouseButton)
            .addFunction("GetMouseButtonDown", &Input::getMouseButtonDown)
            .addFunction("GetMouseButtonUp", &Input::getMouseButtonUp)
            .addFunction("GetMouseScrollDelta", &Input::getMouseScrollDelta)
        .endNamespace()

        // establish lua class: vec2 (glm)
        .beginClass<glm::vec2>("vec2")
            .addProperty("x", &glm::vec2::x)
            .addProperty("y", &glm::vec2::y)
        .endClass()

        // establish lua namespace: Text
        .beginNamespace("Text")
            .addFunction("Draw", &Artist::requestDrawText)
        .endNamespace()

        // establish lua namespace: Audio
        .beginNamespace("Audio")
            .addFunction("Play", &AudioPlayer::play)
            .addFunction("Halt", &AudioPlayer::halt)
            .addFunction("SetVolume", &AudioPlayer::setVolume)
        .endNamespace()

        // establish lua namespace: Image
        .beginNamespace("Image")
            .addFunction("DrawUI", &Artist::requestDrawUI)
            .addFunction("DrawUIEx", &Artist::requestDrawUIEx)
            .addFunction("Draw", &Artist::requestDrawImage)
            .addFunction("DrawEx", &Artist::requestDrawImageEx)
            .addFunction("DrawPixel", &Artist::requestDrawPixel)
        .endNamespace()

        // establish lua namespace: Camera
        .beginNamespace("Camera")
            .addFunction("SetPosition", &Camera::setPosition)
            .addFunction("GetPositionX", &Camera::getPositionX)
            .addFunction("GetPositionY", &Camera::getPositionY)
            .addFunction("SetZoom", &Camera::setZoom)
            .addFunction("GetZoom", &Camera::getZoom)
        .endNamespace()

        // establish lua namespace: Scene
        .beginNamespace("Scene")
            .addFunction("Load", &GameInfo::loadScene)
            .addFunction("GetCurrent", &GameInfo::getCurrentScene)
            .addFunction("DontDestroy", &GameInfo::dontDestroy)
        .endNamespace()

        // establish lua class: Vector2 (b2Vec2) for physics
        // allows a constructor taking two floats
        .beginClass<b2Vec2>("Vector2")
            .addConstructor<void(*) (float, float)>()
            .addProperty("x", &b2Vec2::x)
            .addProperty("y", &b2Vec2::y)
            .addFunction("Normalize", &b2Vec2::Normalize)
            .addFunction("Length", &b2Vec2::Length)
            .addFunction("__add", &b2Vec2::operatorAdd)
            .addFunction("__sub", &b2Vec2::operatorSub)
            .addFunction("__mul", &b2Vec2::operatorMul)
        .endClass()

        .beginNamespace("Vector2")
            .addFunction("Distance", &b2Distance)
            .addFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endNamespace();

    return LuaStateSaver::luaState;
}