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

void Engine::updatePositions() {
	for (Actor& actor : gameInfo.currentScene.actors) {
		if (actor.velocity != glm::dvec2(0)) {
			// if this is our player Actor, perform our player actor movement
			if (actor.name == "player") {
				if (!wouldCollide(actor)) {
					updateActorPosition(actor);
				}
				// stop the player's movement so they only move by 1
				actor.velocity = glm::dvec2(0);
			}
			// move NPC Actors
			// if no collision, keep moving
			// if collision, reverse velocity (move next turn)
			else {
				// make sure they're moving so we don't do unnecessary calculations
				if (!wouldCollide(actor))
					updateActorPosition(actor);
				else
					actor.velocity = -actor.velocity;
			}
		}
	}
}

void Engine::updateActorPosition(Actor& actor) {
	// remove the old position of the actor from the unordered_map
	auto& locToActors = gameInfo.currentScene.locToActors;
	auto oldLocIt = locToActors.find(actor.transform.pos);
	if (oldLocIt != locToActors.end()) {
		auto& actorsAtPos = oldLocIt->second;
		actorsAtPos.erase(std::remove_if(actorsAtPos.begin(), actorsAtPos.end(),
			[&actor](const Actor* a) {
				return a->id == actor.id;
			}), actorsAtPos.end());
		if (actorsAtPos.empty()) {
			locToActors.erase(actor.transform.pos);
		}
	}

	// update the instanced actor's positon
	actor.transform.pos += actor.velocity;

	// add the updated position of the actor to the unordered_map
	auto& actorsAtPos = locToActors[actor.transform.pos];
	actorsAtPos.emplace_back(&actor);
}

// check if an Actor would collide given its velocity
bool Engine::wouldCollide(Actor& actor) {
	glm::ivec2 futurePosition = actor.transform.pos + actor.velocity;

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
		stop();
		break;
	case LOSE:
		stop();
		break;
	default:
		break;
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
	bool introMusicPlaying = false;
	bool gameplayMusicPlaying = false;

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
			else {
				// only allow player motion if there IS a player
				if (player) {
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

			// render the game first
			renderer.render(gameInfo);

			if (player) {
				// render dialogue on top of the game
				renderer.renderDialogue(gameInfo);
				handleState();

				// render HUD on top of the game
				renderer.renderHUD(gameInfo);
			}
		}
		
		// if there's an intro, render it on top of the game
		if (currentIntroIndex < configUtils.introImages.size() || currentIntroIndex < configUtils.introText.size()) {
			renderer.renderIntro(currentIntroIndex);
		}
		// this ensures that the intro music is only halted once
		// and then we start playing the gameplay music, because gameplay has begun
		else if(introMusicPlaying){
			// Halt music playback on channel 0 (intro music)
			AudioHelper::Mix_HaltChannel498(0);
			introMusicPlaying = false;

			if (configUtils.gameplayMusic != "" && !gameplayMusicPlaying) {
				audioPlayer.play(configUtils.gameplayMusic, -1);
				gameplayMusicPlaying = true;
			}
		}
		else if (configUtils.gameplayMusic != "" && !gameplayMusicPlaying) {
			audioPlayer.play(configUtils.gameplayMusic, -1);
			gameplayMusicPlaying = true;
		}

		// Present the render
		Helper::SDL_RenderPresent498(renderer.sdlRenderer);

		if (!gameInfo.discardFrame) {
			SDL_Delay(1);
		}
		else {
			gameInfo.discardFrame = false;
		}

		//// update Actor positions
		//updatePositions();
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
