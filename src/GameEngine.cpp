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

#define OSX_DEBUGPATH 0

void Engine::start() {
    size_t currentIntroIndex = 0;
    isGameRunning = true;
    bool introMusicPlaying = false, gameplayMusicPlaying = false;

    GameConfig& gameConfig = configManager.gameConfig;
    RenderingConfig& renderConfig = configManager.renderingConfig;

    if (gameConfig.introMusic != "" && !introMusicPlaying) {
        audioPlayer.play(gameConfig.introMusic, -1, 0);
        introMusicPlaying = true;
    }

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

            // intro click handlinga
            if (currentIntroIndex < gameConfig.introImages.size() || currentIntroIndex < gameConfig.introText.size()) {
                // buttons to proceed are: space, return, left click
                if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
                    ++currentIntroIndex;
                }
            }
        }

        // intro button handling handling
        if (currentIntroIndex < gameConfig.introImages.size() || currentIntroIndex < gameConfig.introText.size()) {
            // buttons to proceed are: space, return, left click
            if (input.getKeyDown(SDL_SCANCODE_SPACE) || input.getKeyDown(SDL_SCANCODE_RETURN)) {
                ++currentIntroIndex;
            }
        }

        // gameplay handling
        else if (player) {
            // upward movement (up/w)
            if (input.getKey(SDL_SCANCODE_UP) || input.getKey(SDL_SCANCODE_W)) {
                player->velocity += Direction::UP;
            }
            // downward movement (down/s)
            if (input.getKey(SDL_SCANCODE_DOWN) || input.getKey(SDL_SCANCODE_S)) {
                player->velocity += Direction::DOWN;
            }
            // leftward movement (left/a)
            if (input.getKey(SDL_SCANCODE_LEFT) || input.getKey(SDL_SCANCODE_A)) {
                player->velocity += Direction::LEFT;
            }
            // rightward movement (right/d)
            if (input.getKey(SDL_SCANCODE_RIGHT) || input.getKey(SDL_SCANCODE_D)) {
                player->velocity += Direction::RIGHT;
            }

            // if the player has velocity, move them and reset their velocity
            if (std::abs(player->velocity.x) > 0 || std::abs(player->velocity.y) > 0) {
                // start by normalizing and multiplying by speed
                player->velocity = glm::normalize(player->velocity);

            }
        }

        // make the input not "newly down" or "newly up" anymore
        input.lateUpdate();
        // handle and render gameplay
        if (!gameOver) {
            // halt the intro music if it's playing
            if (introMusicPlaying) {
                AudioHelper::Mix_HaltChannel498(0);
                introMusicPlaying = false;
            }

            // start the gameplay music if there is any
            if (!gameplayMusicPlaying && gameConfig.gameplayMusic != "") {
                audioPlayer.play(gameConfig.gameplayMusic, -1, 0);
                gameplayMusicPlaying = true;
            }

            int frame = Helper::GetFrameNumber();

            // move all actors according to their velocity
            gameInfo.state = gameInfo.scene.moveAllActors(renderConfig.actorFlipping, gameInfo.state, gameConfig, audioPlayer);

            // update the camera position to match where the player is (because all actors have moved by now)
            camera.update(gameInfo.player, renderConfig.easeFactor);

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
#if defined(OSX_DEBUGPATH) && OSX_DEBUGPATH == 1
    // for MacOS local runs
    std::filesystem::current_path("/Users/lindsaygreig/Desktop/game_engine_nkhess");
#endif
    
	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	// Initialize SDL_image to handle PNGs
	IMG_Init(IMG_INIT_PNG);

	// Initialize SDL_ttf
	TTF_Init();

	// Initialize SDL_mixer
	Mix_Init(MIX_INIT_OGG);

    AudioHelper::Mix_AllocateChannels498(50);

	// Open the default audio device for playback
	AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 1, 2048);

    ResourceManager resourceManager;
	ConfigManager configManager(resourceManager);
	Renderer renderer(configManager, resourceManager);
	AudioPlayer audioPlayer(resourceManager);
    Input input;
    Camera camera(configManager);

	Engine engine(renderer, configManager, audioPlayer, input, camera, resourceManager);
	engine.start();

	// quit SDL at the very end
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
	Mix_Quit();

	return 0;
}
