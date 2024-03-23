// std libraries
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <filesystem>

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

void Engine::start() {
    size_t currentIntroIndex = 0;
    isGameRunning = true;

    GameConfig& gameConfig = configManager.gameConfig;
    RenderingConfig& renderConfig = configManager.renderingConfig;

    for (Actor& actor : gameInfo.scene.actors) {
        // load actor's images early to calculate extents for collision
        actor.loadTextures(resourceManager);

        // store the actor as a convenience reference in the component
        for (auto& [key, component] : actor.componentsByKey) {
            component.instanceTable["actor"] = actor;
        }
    }

    for (Actor* actor : gameInfo.scene.actorsWithOnStart) {
        for (auto& [key, component] : actor->componentsWithOnStart) {
            component->callLuaFunction("OnStart", actor->name);
        }
    }

    // main game loop
    // see function declaration/docs for order of events
    while (isGameRunning) {
        // Process events
        SDL_Event sdlEvent;
        while (Helper::SDL_PollEvent498(&sdlEvent)) {
            Input::processEvent(sdlEvent);
            // handle a quit event
            if (sdlEvent.type == SDL_QUIT) {
                queueStop();
            }
        }

        // do OnUpdate for all actors
        for (Actor* actor : gameInfo.scene.actorsWithOnUpdate) {
            for (auto& [key, component] : actor->componentsWithOnUpdate) {
                component->callLuaFunction("OnUpdate", actor->name);
            }
        }

        // do OnLateUpdate for all actors
        for (Actor* actor : gameInfo.scene.actorsWithOnLateUpdate) {
            for (auto& [key, component] : actor->componentsWithOnLateUpdate) {
                component->callLuaFunction("OnLateUpdate", actor->name);
            }
        }

        // make the input not "newly down" or "newly up" anymore
        Input::lateUpdate();

        // handle and render gameplay
        if (!gameOver) {
            int frame = Helper::GetFrameNumber();
            //gameInfo.state = gameInfo.scene.moveAllActors(renderConfig.actorFlipping, gameInfo.state, gameConfig, audioPlayer);

            //camera.update(gameInfo.player, renderConfig.easeFactor);

            // render the game first
            renderer.render(gameInfo);

            // set the scale back to normal
            SDL_RenderSetScale(renderer.sdlRenderer, renderConfig.zoomFactor, renderConfig.zoomFactor);
        }

        // Present the render AND DELAY, apparently it does that for us
        Helper::SDL_RenderPresent498(renderer.sdlRenderer);
    }
}

void Engine::queueStop() {
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
	AudioPlayer audioPlayer(resourceManager);
    Camera camera(configManager);

	Engine engine(renderer, configManager, audioPlayer, camera, resourceManager, luaState);
    LuaUtils::setupLua(luaState);
    LuaUtils::currentScene = engine.gameInfo.scene;

	engine.start();

	return 0;
}
