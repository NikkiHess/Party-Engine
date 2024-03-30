// std libraries
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <memory>

// include my code
#include "GameEngine.h"
#include "utils/config/ConfigManager.h"
#include "utils/config/GameConfig.h"
#include "utils/config/RenderingConfig.h"
#include "visuals/Renderer.h"
#include "input/Input.h"
#include "input/Direction.h"
#include "utils/StringUtils.h"
#include "utils/LuaUtils.h"

// dependencies
#include "glm/glm.hpp"
#include "rapidjson/document.h"

// SDL2
#include "AudioHelper.h"
#include "Helper.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

void Engine::runLifecycleFunctions() {
    // do OnStart for all actors with NEW OnStart components
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnStart) {
        for (auto& [key, component] : actor->componentsWithOnStart) {
            component->callLuaFunction("OnStart", actor->name);
            if (component->onStartCalled) {
                actor->componentsToRemoveFromOnStart[component->key] = component;
            }
        }

        for (auto& [key, component] : actor->componentsToRemoveFromOnStart) {
            actor->componentsWithOnStart.erase(key);
        }
        actor->componentsToRemoveFromOnStart.clear();
    }

    // do OnUpdate for all actors
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnUpdate) {
        for (auto& [key, component] : actor->componentsWithOnUpdate) {
            component->callLuaFunction("OnUpdate", actor->name);
        }
    }

    // do OnLateUpdate for all actors
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnLateUpdate) {
        for (auto& [key, component] : actor->componentsWithOnLateUpdate) {
            component->callLuaFunction("OnLateUpdate", actor->name);
        }
    }
}

void Engine::runtimeAlterations() {
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsToAdd) {
        LuaUtils::instantiateActor(actor);
    }
    GameInfo::scene.actorsToAdd.clear();

    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsToRemove) {
        LuaUtils::destroyActor(actor);
    }
    GameInfo::scene.actorsToRemove.clear();

    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithNewComponents) {
        for (std::shared_ptr<Component> component : actor->componentsToAdd) {
            std::optional<rapidjson::Value*> opt = std::nullopt;
            actor->addComponent(component->type, component->key, opt);
        }
        actor->componentsToAdd.clear();
    }
    GameInfo::scene.actorsWithNewComponents.clear();

    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithComponentsToRemove) {
        for (std::shared_ptr<Component> component : actor->componentsToRemove) {
            actor->removeComponent(component);
        }
        actor->componentsToRemove.clear();
    }
    GameInfo::scene.actorsWithComponentsToRemove.clear();
}

void Engine::start() {
    size_t currentIntroIndex = 0;
    isGameRunning = true;

    GameConfig& gameConfig = configManager.gameConfig;
    RenderingConfig& renderConfig = configManager.renderingConfig;

    for (std::shared_ptr<Actor>& actor : GameInfo::scene.actors) {
        // store the actor as a convenience reference in the component
        for (auto& [key, component] : actor->componentsByKey) {
            component.instanceTable["actor"] = actor.get();
        }
    }

    // do OnStart for all actors, then CLEAR actorsWithOnStart to allow for new
    // OnStarts to be added in the middle of the game
    for (std::shared_ptr<Actor> actor : GameInfo::scene.actorsWithOnStart) {
        for (auto& [key, component] : actor->componentsWithOnStart) {
            component->callLuaFunction("OnStart", actor->name);
        }
    }
    GameInfo::scene.actorsWithOnStart.clear();

    // main game loop
    // see function declaration/docs for order of events
    while (isGameRunning) {
        // if there is a scene switch, we need to do that right at the start of the frame
        if (GameInfo::scene.id != GameInfo::newScene.id) {
            GameInfo::scene = GameInfo::newScene;
            LuaUtils::currentScene = &GameInfo::scene;

            for (std::shared_ptr<Actor>& actor : GameInfo::scene.actors) {
                // store the actor as a convenience reference in the component
                for (auto& [key, component] : actor->componentsByKey) {
                    component.instanceTable["actor"] = actor.get();
                }
            }
        }

        // Process events
        SDL_Event sdlEvent;
        while (Helper::SDL_PollEvent498(&sdlEvent)) {
            Input::processEvent(sdlEvent);
            // handle a quit event
            if (sdlEvent.type == SDL_QUIT) {
                requestStop();
            }
        }

        runLifecycleFunctions();

        runtimeAlterations();

        // make the input not "newly down" or "newly up" anymore
        Input::lateUpdate();

        // handle and render gameplay
        if (!gameOver) {
            int frame = Helper::GetFrameNumber();
            //GameInfo::state = GameInfo::scene.moveAllActors(renderConfig.actorFlipping, GameInfo::state, gameConfig, audioPlayer);

            //camera.update(GameInfo::player, renderConfig.easeFactor);

            // render the game first
            renderer.render();

            // set the scale back to normal
            SDL_RenderSetScale(renderer.sdlRenderer, renderConfig.zoomFactor, renderConfig.zoomFactor);
        }

        // Present the render AND DELAY, apparently it does that for us
        Helper::SDL_RenderPresent498(renderer.sdlRenderer);
    }
}

void Engine::requestStop() {
	isGameRunning = false;
}

int main(int argc, char* argv[]) {
	// Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        const std::string& sdlError = SDL_GetError();
        Error::error("SDL could not initialize. SDL error: " + sdlError);
    }
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();
	Mix_Init(MIX_INIT_OGG);

    AudioHelper::Mix_AllocateChannels498(50);

	// Open the default audio device for playback
	AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 1, 2048);

    // open a new state for the lua and open some default libraries
    lua_State* luaState = luaL_newstate();
    luaL_openlibs(luaState);

    LuaUtils::luaState = luaState;
    Input::init();

    ResourceManager resourceManager;
	ConfigManager configManager(resourceManager, luaState);
	Renderer renderer(configManager, resourceManager);

	Engine engine(renderer, configManager, resourceManager, luaState);
    LuaUtils::setupLua(luaState);
    LuaUtils::currentScene = &GameInfo::scene;
    LuaUtils::sceneConfig = &configManager.sceneConfig;
    LuaUtils::resourceManager = &resourceManager;

    AudioPlayer::resourceManager = &resourceManager;

    Camera::renderConfig = &configManager.renderingConfig;

	engine.start();

	return 0;
}
