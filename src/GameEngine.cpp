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

void Engine::start() {
    size_t currentIntroIndex = 0;
    isGameRunning = true;

    GameConfig& gameConfig = configManager.gameConfig;
    RenderingConfig& renderConfig = configManager.renderingConfig;

    for (Actor& actor : gameInfo.scene.actors) {
        // load actor's images early to calculate extents for collision
        actor.loadTextures(resourceManager);
    }

    // main game loop
    // see function declaration/docs for order of events
    while (isGameRunning) {
        // Process events
        SDL_Event sdlEvent;
        while (Helper::SDL_PollEvent498(&sdlEvent)) {
            input.processEvent(sdlEvent);
            // handle a quit event
            if (sdlEvent.type == SDL_QUIT) {
                queueStop();
            }
        }

        // make the input not "newly down" or "newly up" anymore
        input.lateUpdate();
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

// establish our lua_State* and all namespaces
lua_State* setupLua() {
    // open a new state for the lua and open some default libraries
    lua_State* luaState = luaL_newstate();
    luaL_openlibs(luaState);

    // establish lua Debug namespace
    // Debug.Log and Debug.LogError
    luabridge::getGlobalNamespace(luaState)
        .beginNamespace("Debug")
        .addFunction("Log", Component::log)
        .addFunction("LogError", Component::logError)
        .endNamespace();

    return luaState;
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

    lua_State* luaState = setupLua();

    ResourceManager resourceManager;
	ConfigManager configManager(resourceManager, luaState);
	Renderer renderer(configManager, resourceManager);
	AudioPlayer audioPlayer(resourceManager);
    Input input;
    Camera camera(configManager);

	Engine engine(renderer, configManager, audioPlayer, input, camera, resourceManager);
	engine.start();

	return 0;
}
