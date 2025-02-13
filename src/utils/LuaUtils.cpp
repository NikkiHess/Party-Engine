// std library
#include <algorithm>
#include <string>
#include <thread>
#include <unordered_map>

// my code
#include "../audio/AudioPlayer.h"
#include "../GameInfo.h"
#include "../input/Input.h"
#include "../utils/config/SceneConfig.h"
#include "../utils/LuaStateSaver.h"
#include "../utils/SaveData.h"
#include "../visuals/Artist.h"
#include "../visuals/Camera.h"
#include "../world/Actor.h"
#include "../world/physics/VectorWrapper.h"
#include "LuaUtils.h"

// dependencies
#include "Helper.h"

// lua
#include "lua/lua.hpp" 
#include "LuaBridge/LuaBridge.h" 

// Box2D
#include "box2d/box2d.h" 

void LuaUtils::printLuaException(const luabridge::LuaException& e, const std::string& actorName, const std::string& componentName) {
    std::string errorMessage = e.what();

    // normalize file paths across platforms
    std::replace(errorMessage.begin(), errorMessage.end(), '\\', '/');

    // in the case that the script was loaded as a string, fix the error message
    std::string wrongMessage = "[string \"" + componentName + " = {...\"]";
    size_t index = errorMessage.find(wrongMessage) ;
    if(index != std::string::npos) {
        errorMessage.replace(index, wrongMessage.size(), "resources/component_types/" + componentName + ".lua");
    }

    // display (with color codes)
    std::cerr << "\033[31m" << actorName << " : " << errorMessage << "\033[0m\n";
}

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

void LuaUtils::logWithTime(const std::string& message) {
    std::time_t time = std::time(0); // get current time
    std::tm local;

    // convert to local time, platform dependent
    // https://stackoverflow.com/a/38034148/4426055
#ifdef __unix__
    localtime_r(&time, &local);
#elif defined(_MSC_VER)
    localtime_s(&local, &time);
#else
    static std::mutex mtx;
    std::lock_guard<std::mutex> lock(mtx);
    local = *std::localtime(&time);
#endif

    char formattedTime[9];
    strftime(formattedTime, sizeof(formattedTime), "%T", &local);

    printf("[%s] %s\n", formattedTime, message.c_str());
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
    currentScene->instantiateActorLifecycle(actorPtr, true);
}


void LuaUtils::requestDestroyActor(const luabridge::LuaRef& actorRef) {
    std::shared_ptr actorShared = currentScene->actorsById[actorRef.cast<Actor*>()->id];

    for (auto& [key, componentPtr] : actorShared->componentsByKey) {
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

    currentScene->actorsWithComponentsToRemove.erase(actorPtr);
    currentScene->actorsWithNewComponents.erase(actorPtr);
    currentScene->actorsWithOnLateUpdate.erase(actorPtr);
    currentScene->actorsWithOnStart.erase(actorPtr);
    currentScene->actorsWithOnUpdate.erase(actorPtr);
    currentScene->actorsWithOnExit.erase(actorPtr);
    currentScene->actorsWithOnClick.erase(actorPtr);
}

// establish lua namespace: Debug
// Debug.Log and Debug.LogError
void setupDebug() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        .beginNamespace("Debug")
            .addFunction("Log", LuaUtils::log)
            .addFunction("LogTime", LuaUtils::logWithTime)
            .addFunction("LogError", LuaUtils::logError)
        .endNamespace();
}

// establish lua class: Actor (GetName, GetID, GetComponent, GetComponentByKey,
// GetComponents, AddComponent, RemoveComponent)
// establish lua namespace: Actor (Find and FindAll)
// establish lua namespace: Scene
void setupWorld() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        .beginClass<Actor>("Actor")
            .addFunction("GetName", &Actor::getName)
            .addFunction("GetID", &Actor::getID)
            .addFunction("GetComponentByKey", &Actor::getComponentByKey)
            .addFunction("GetComponent", &Actor::getComponent)
            .addFunction("GetComponents", &Actor::getComponents)
            .addFunction("AddComponent", &Actor::requestAddComponent)
            .addFunction("RemoveComponent", &Actor::requestRemoveComponent)
        .endClass()

        .beginNamespace("Actor")
            .addFunction("Find", &LuaUtils::findActor)
            .addFunction("FindAll", &LuaUtils::findAllActors)
            .addFunction("Instantiate", &LuaUtils::requestInstantiateActor)
            .addFunction("Destroy", &LuaUtils::requestDestroyActor)
        .endNamespace()

        .beginNamespace("Scene")
            .addFunction("Load", &GameInfo::loadScene)
            .addFunction("GetCurrent", &GameInfo::getCurrentScene)
            .addFunction("DontDestroy", &GameInfo::dontDestroy)
        .endNamespace();
}

// establish lua namespace: Application (Quit, Sleep, GetFrame)
void setupApplication() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        .beginNamespace("Application")
            .addFunction("SetBG", &Artist::setBG)
            .addFunction("Quit", &LuaUtils::quit)
            .addFunction("Sleep", &LuaUtils::sleep)
            .addFunction("GetFrame", &LuaUtils::getFrame)
        .endNamespace();
}

// establish lua namespace: Input (GetKey, GetKeyDown, GetKeyUp)
void setupInput() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
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
}

// establish lua class: Vector2 (glm/box2d)
// allows a constructor taking two floats
void setupPhysics() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        .beginClass<VectorWrapper>("Vector2")
            .addConstructor<void(*) (float, float)>()
            .addProperty("x", &VectorWrapper::getX, &VectorWrapper::setX)
            .addProperty("y", &VectorWrapper::getY, &VectorWrapper::setY)
            .addFunction("Normalize", &VectorWrapper::normalize)
            .addFunction("Length", &VectorWrapper::length)
            .addFunction("__add", &VectorWrapper::operator+)
            .addFunction("__sub", &VectorWrapper::operator-)
            .addFunction("__mul", &VectorWrapper::operator*)
        .endClass()

        .beginNamespace("Vector2")
            .addFunction("Distance", &b2Distance)
            .addFunction("Dot", static_cast<float (*)(const b2Vec2&, const b2Vec2&)>(&b2Dot))
        .endNamespace()

        .beginClass<Rigidbody>("Rigidbody")
            .addProperty("key", &Rigidbody::key)
            .addProperty("enabled", &Rigidbody::enabled)
            .addProperty("actor", &Rigidbody::actor)
            .addProperty("bodyType", &Rigidbody::bodyType)
            .addProperty("precise", &Rigidbody::precise)
            .addProperty("gravityScale", &Rigidbody::gravityScale)
            .addProperty("density", &Rigidbody::density)
            .addProperty("angularFriction", &Rigidbody::angularFriction)
            .addProperty("hasCollider", &Rigidbody::hasCollider)
            .addProperty("hasTrigger", &Rigidbody::hasTrigger)
            .addProperty("body", &Rigidbody::body)
            .addFunction("GetPosition", &Rigidbody::getPosition)
            .addFunction("GetRotation", &Rigidbody::getRotation)
            .addFunction("OnStart", &Rigidbody::onStart)
        .endClass();
}

// establish lua namespace: Text
// establish lua namespace: Image
// establish lua namespace: Camera
// establish lua class: Color
// establish lua class: UIRenderer
void setupVisuals() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        .beginNamespace("Text")
            .addFunction("Draw", &Artist::requestDrawText)
        .endNamespace()

        .beginNamespace("Image")
            .addFunction("DrawUI", &Artist::requestDrawUI)
            .addFunction("DrawUIEx", &Artist::requestDrawUIEx)
            .addFunction("Draw", &Artist::requestDrawImage)
            .addFunction("DrawEx", &Artist::requestDrawImageEx)
            .addFunction("DrawPixel", &Artist::requestDrawPixel)
            .addFunction("GetWidth", &Artist::getImageWidth)
            .addFunction("GetHeight", &Artist::getImageHeight)
        .endNamespace()

        .beginNamespace("Camera")
            .addFunction("SetPosition", &Camera::setPosition)
            .addFunction("GetPositionX", &Camera::getPositionX)
            .addFunction("GetPositionY", &Camera::getPositionY)
            .addFunction("SetZoom", &Camera::setZoom)
            .addFunction("GetZoom", &Camera::getZoom)
            .addFunction("GetWidth", &Camera::getWidth)
            .addFunction("GetHeight", &Camera::getHeight)
        .endNamespace()

        .beginClass<SDL_Color>("Color")
            .addProperty("r", &SDL_Color::r)
            .addProperty("g", &SDL_Color::g)
            .addProperty("b", &SDL_Color::b)
            .addProperty("a", &SDL_Color::a)
        .endClass();

//        .beginClass<UIRenderer>("UIRenderer")
//            .addProperty("key", &UIRenderer::key)
//            .addProperty("enabled", &UIRenderer::enabled)
//            .addProperty("actor", &UIRenderer::actor)
//            .addProperty("sprite", &UIRenderer::sprite)
//            .addProperty("tint", &UIRenderer::tint)
//            .addProperty("sortingOrder", &UIRenderer::sortingOrder)
//            .addProperty("transform", &UIRenderer::transform)
//            .addFunction("OnStart", &UIRenderer::onStart)
//            .addFunction("OnUpdate", &UIRenderer::onUpdate)
//        .endClass();
}

// establish lua namespace: Audio
void setupAudio() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        .beginNamespace("Audio")
            .addFunction("Play", &AudioPlayer::play)
            .addFunction("Halt", &AudioPlayer::halt)
            .addFunction("SetVolume", &AudioPlayer::setVolume)
        .endNamespace();
}

// establish lua class: SaveData
void setupSaveData() {
    luabridge::getGlobalNamespace(LuaStateSaver::luaState)
        .beginClass<SaveData>("SaveData")
            .addConstructor<void(*) (const std::string&)>()
            .addProperty("name", &SaveData::name)
            .addFunction("SetString", &SaveData::setString)
            .addFunction("SetInt", &SaveData::setInt)
            .addFunction("SetFloat", &SaveData::setFloat)
            .addFunction("SetBool", &SaveData::setBool)
            .addFunction("SetTable", &SaveData::setTable)
            .addFunction("SaveAll", &SaveData::saveAll)
            .addFunction("GetString", &SaveData::getString)
            .addFunction("GetInt", &SaveData::getInt)
            .addFunction("GetFloat", &SaveData::getFloat)
            .addFunction("GetBool", &SaveData::getBool)
            .addFunction("GetTable", &SaveData::getTable)
            .addFunction("HasMember", &SaveData::hasMember)
            .addFunction("Save", &SaveData::saveDocument)
            .addFunction("Clear", &SaveData::clear)
        .endClass();
}

// establish our lua_State* and all namespaces/classes
void LuaUtils::setupLua() {
    setupDebug();

    setupWorld();

    setupApplication();

    setupInput();

    setupAudio();

    setupPhysics();

    setupVisuals();

    setupSaveData();
}
