// std libraries
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <filesystem>

// include my code
#include "GameEngine.h"
#include "utils/ConfigUtils.h"
#include "visuals/Renderer.h"
#include "gamedata/Input.h"
#include "gamedata/Direction.h"

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

void Engine::start() {
	// a window with proprties as defined by configUtils
	SDL_Window* window = SDL_CreateWindow(
		configUtils.gameTitle.c_str(),	// window title
		SDL_WINDOWPOS_CENTERED,			// initial x
		SDL_WINDOWPOS_CENTERED,			// iniital y
		configUtils.renderSize.x,		// width, in pixels
		configUtils.renderSize.y,		// height, in pixels
		SDL_WINDOW_SHOWN				// flags
	);

	// Create our Renderer using our window, -1 (go find a display), and VSYNC/GPU rendering enabled
	SDL_Renderer* sdlRenderer = Helper::SDL_CreateRenderer498(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	renderer.sdlRenderer = sdlRenderer;
	renderer.artist.sdlRenderer = sdlRenderer;

	// run the game loop
	doGameLoop();
}

void Engine::doGameLoop() {
    int currentIntroIndex = 0;
    isGameRunning = true;
    bool introMusicPlaying = false, gameplayMusicPlaying = false;

    if (configUtils.introMusic != "" && !introMusicPlaying) {
        audioPlayer.play(configUtils.introMusic, -1);
        introMusicPlaying = true;
    }

    while (isGameRunning) {
        // Process events
        SDL_Event sdlEvent;
        while (Helper::SDL_PollEvent498(&sdlEvent)) {
            input.processEvent(sdlEvent);
            // handle a quit event
            if (sdlEvent.type == SDL_QUIT) {
                stop();
            }

            // intro handling
            if (currentIntroIndex < configUtils.introImages.size() || currentIntroIndex < configUtils.introText.size()) {
                // buttons to proceed are: space, return, left click
                if (sdlEvent.type == SDL_MOUSEBUTTONDOWN || input.getKeyDown(SDL_SCANCODE_SPACE) || input.getKeyDown(SDL_SCANCODE_RETURN)) {
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
                    player->velocity = glm::normalize(player->velocity) * player->speed;
                    gameInfo.scene.moveActor(player);
                    player->velocity = glm::vec2(0);
                }
            }

            // make the input not "newly down" or "newly up" anymore
            input.lateUpdate();
        }

        // if there's an intro, render it
        if (currentIntroIndex < configUtils.introImages.size() || currentIntroIndex < configUtils.introText.size()) {
            renderer.renderIntro(currentIntroIndex);
        }
        // handle and render gameplay
        else if(!gameOver) {
            // halt the intro music if it's playing
            if (introMusicPlaying) {
                AudioHelper::Mix_HaltChannel498(0);
                introMusicPlaying = false;
            }

            // start the gameplay music if there is any
            if (!gameplayMusicPlaying && configUtils.gameplayMusic != "") {
                audioPlayer.play(configUtils.gameplayMusic, -1);
                gameplayMusicPlaying = true;
            }

            // move NPCs before render, just like we move player before render
            // only move on frames divisible by 60, but not frame 0
            if (Helper::GetFrameNumber() % 60 == 0 && Helper::GetFrameNumber() != 0) {
                gameInfo.scene.moveNPCActors();
            }

            // render the game first
            renderer.render(gameInfo);

            if (player) {
                // render dialogue on top of the game
                renderer.renderDialogue(gameInfo);

                switch (state) {
                case WIN:
                    if (!gameOverMusicPlaying && 
                        configUtils.gameOverGoodAudio != "") {
                        AudioHelper::Mix_HaltChannel498(0);
                        audioPlayer.play(configUtils.gameOverGoodAudio, 0);
                        gameOverMusicPlaying = true;
                        gameOver = true;
                    }
                    continue;
                case LOSE:
                    if (!gameOverMusicPlaying &&
                        configUtils.gameOverBadAudio != "") {
                        AudioHelper::Mix_HaltChannel498(0);
                        audioPlayer.play(configUtils.gameOverBadAudio, 0);
                        gameOverMusicPlaying = true;
                        gameOver = true;
                    }
                    continue;
                case PROCEED:
                    state = NORMAL;
                    continue;
                default:
                    break;
                }

                // render HUD on top of the game
                renderer.renderHUD(gameInfo);
            }
        }
        // game over
        else {
            if (gameInfo.state == WIN) {
                if (configUtils.gameOverGoodImage != "") {
                    renderer.artist.drawUIImage(
                        configUtils.gameOverGoodImage,
                        { 0, 0 },
                        { configUtils.renderSize.x, configUtils.renderSize.y }
                    );
                }
            }
            else if (gameInfo.state == LOSE) {
                if (configUtils.gameOverBadImage != "") {
                    renderer.artist.drawUIImage(
                        configUtils.gameOverBadImage,
                        { 0, 0 },
                        { configUtils.renderSize.x, configUtils.renderSize.y }
                    );
                }
            }
        }

        // Present the render AND DELAY, apparently it does that for us
        Helper::SDL_RenderPresent498(renderer.sdlRenderer);
    }
}

void Engine::stop() {
	isGameRunning = false;
}

int main(int argc, char* argv[]) {
    // for MacOS local runs
    // std::filesystem::current_path("/Users/lindsaygreig/Desktop/game_engine_nkhess");
    
	// Initialize SDL
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

	// Initialize SDL_image to handle PNGs
	IMG_Init(IMG_INIT_PNG);

	// Initialize SDL_ttf
	TTF_Init();

	// Initialize SDL_mixer
	Mix_Init(MIX_INIT_OGG);
	// Open the default audio device for playback
	AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 1, 2048);

	ConfigUtils configUtils;
	Renderer renderer(configUtils);
	AudioPlayer audioPlayer(configUtils);
    Input input;

	Engine engine(renderer, configUtils, audioPlayer, input);
	engine.start();

	// quit at the very end
	// this is best practice, but staff doesn't do them
	//SDL_Quit();
	//IMG_Quit();
	//TTF_Quit();
	//Mix_Quit();

	return 0;
}
