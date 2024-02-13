// std libraries
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

// include my code
#include "Constants.h"
#include "GameEngine.h"
#include "utils/ConfigUtils.h"
#include "visuals/Renderer.h"

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

// ---------- BEGIN MOTION FUNCTIONS ----------

void Engine::updateNPCPositions() {
	for (Actor* actor : gameInfo.currentScene.motionActors) {
		// if no collision, keep moving
		// if collision, reverse velocity (move next turn)
		// make sure they're moving so we don't do unnecessary calculations
        // if no collision, keep moving
		if (!wouldCollide(actor)) {
			updateActorPosition(actor);
		}
		else {
			actor->velocity = -actor->velocity;
		}
	}
}

void Engine::updateActorPosition(Actor* actor) {
	// remove the old position of the actor from the unordered_map
	auto& locToActors = gameInfo.currentScene.locToActors;
    locToActors[actor->transform.pos].erase(actor);

	// update the instanced actor's positon
    actor->transform.pos += actor->velocity;

	// add the updated position of the actor to the unordered_map
    locToActors[actor->transform.pos].emplace(actor);
}

// check if an Actor would collide given its velocity
bool Engine::wouldCollide(Actor* actor) {
	glm::ivec2 futurePosition = actor->transform.pos + actor->velocity;

	auto it = gameInfo.currentScene.locToActors.find(futurePosition);
	if (it != gameInfo.currentScene.locToActors.end()) {
		for (Actor* other : it->second) {
			if (other->blocking)
				return true;
		}
	}

	return false;
}

// ----------- END MOTION FUNCTIONS -----------

// ----------- BEGIN CORE FUNCTIONS -----------

void Engine::handleState() {
	// Should the Engine class handle printing these messages, or should the Renderer?
    switch (gameInfo.state) {
    case WIN:
        AudioHelper::Mix_HaltChannel498(0);
        gameOver = true;
		break;
	case LOSE:
        AudioHelper::Mix_HaltChannel498(0);
        gameOver = true;
		break;
	default:
		break;
	}
}

void Engine::preloadResources() {
    // preload intro images
    for (std::string introImage : configUtils.introImages) {
        renderer.artist.loadImageTexture(introImage);
    }

    // preload intro text
    for (std::string introText : configUtils.introText) {
        renderer.artist.loadTextTexture(introText, {255, 255, 255, 255});
    }

    // preload actor images
    for (Actor& actor : gameInfo.currentScene.actors) {
        actor.view.image = renderer.artist.loadImageTexture(actor.view.imageName);
    }
}

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
    bool introMusicPlaying = false, gameplayMusicPlaying = false,
        gameOverMusicPlaying = false;

    if (configUtils.introMusic != "" && !introMusicPlaying) {
        audioPlayer.play(configUtils.introMusic, -1);
        introMusicPlaying = true;
    }

    while (isGameRunning) {
        // Process events
        SDL_Event nextEvent;
        while (Helper::SDL_PollEvent498(&nextEvent)) {
            if (nextEvent.type == SDL_QUIT) {
                stop();
            }
            // Intro handling
            if (currentIntroIndex < configUtils.introImages.size() || currentIntroIndex < configUtils.introText.size()) {
                // if a key is pressed
                if (nextEvent.type == SDL_KEYDOWN) {
                    SDL_Scancode scancode = nextEvent.key.keysym.scancode;

                    // Go to the next intro image if we press space or enter
                    switch (scancode) {
                    case SDL_SCANCODE_SPACE:
                    case SDL_SCANCODE_RETURN:
                        ++currentIntroIndex;
                        break;
                    default:
                        break;
                    }
                }
                // Go to the next intro image if we click
                if (nextEvent.type == SDL_MOUSEBUTTONDOWN) {
                    ++currentIntroIndex;
                }
            }
            // gameplay handling
            else if (player) {
                // only allow player motion if there IS a player
                if (nextEvent.type == SDL_KEYDOWN) {
                    SDL_Scancode scancode = nextEvent.key.keysym.scancode;
                    glm::dvec2 newPos = player->transform.pos;

                    // handle movement
                    switch (scancode) {
                    case SDL_SCANCODE_UP:
                    case SDL_SCANCODE_W:
                        newPos += glm::dvec2(0, -1);
                        gameInfo.currentScene.moveActor(player, newPos);
                        break;
                    case SDL_SCANCODE_DOWN:
                    case SDL_SCANCODE_S:
                        newPos += glm::dvec2(0, 1);
                        gameInfo.currentScene.moveActor(player, newPos);
                        break;
                    case SDL_SCANCODE_LEFT:
                    case SDL_SCANCODE_A:
                        newPos += glm::dvec2(-1, 0);
                        gameInfo.currentScene.moveActor(player, newPos);
                        break;
                    case SDL_SCANCODE_RIGHT:
                    case SDL_SCANCODE_D:
                        newPos += glm::dvec2(1, 0);
                        gameInfo.currentScene.moveActor(player, newPos);
                        break;
                    default:
                        break;
                    }
                }
            }
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
                updateNPCPositions();
            }

            // render the game first
            renderer.render(gameInfo);

            if (player) {
                // render dialogue on top of the game
                renderer.renderDialogue(gameInfo);
                handleState();
                if (gameInfo.state == PROCEED) {
                    gameInfo.state = NORMAL;
                    continue;
                }

                // render HUD on top of the game
                renderer.renderHUD(gameInfo);
            }
        }
        // game over
        else {
            if (gameInfo.state == WIN) {
                if (configUtils.gameOverGoodImage != "") {
                    renderer.artist.drawStaticImage(
                        configUtils.gameOverGoodImage,
                        { 0, 0 },
                        { configUtils.renderSize.x, configUtils.renderSize.y }
                    );
                }

                if (!gameOverMusicPlaying && configUtils.gameOverGoodAudio != "") {
                    audioPlayer.play(configUtils.gameOverGoodAudio, 0);
                    gameOverMusicPlaying = true;
                }
            }
            else if (gameInfo.state == LOSE) {
                if (configUtils.gameOverBadImage != "") {
                    renderer.artist.drawStaticImage(
                        configUtils.gameOverBadImage,
                        { 0, 0 },
                        { configUtils.renderSize.x, configUtils.renderSize.y }
                    );
                }

                if (!gameOverMusicPlaying && configUtils.gameOverBadAudio != "") {
                    audioPlayer.play(configUtils.gameOverBadAudio, 0);
                    gameOverMusicPlaying = true;
                }
            }
        }

        // Present the render
        Helper::SDL_RenderPresent498(renderer.sdlRenderer);

        SDL_Delay(1);
    }
}

void Engine::stop() {
	isGameRunning = false;
}

// ----------- END CORE FUNCTIONS ------------

int main(int argc, char* argv[]) {
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

	Engine engine(renderer, configUtils, audioPlayer);
	engine.start();

	// quit at the very end
	// this is best practice, but staff doesn't do them
	//SDL_Quit();
	//IMG_Quit();
	//TTF_Quit();
	//Mix_Quit();

	return 0;
}
