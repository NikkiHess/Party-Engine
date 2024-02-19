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
    size_t currentIntroIndex = 0;
    isGameRunning = true;
    bool introMusicPlaying = false, gameplayMusicPlaying = false;

    GameConfig& gameConfig = configManager.gameConfig;
    RenderingConfig& renderConfig = configManager.renderingConfig;

    if (gameConfig.introMusic != "" && !introMusicPlaying) {
        audioPlayer.play(gameConfig.introMusic, -1);
        introMusicPlaying = true;
    }

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

            if (player->velocity == glm::vec2(0)) {
                player->transform.bounce = false;
            }

            // if the player has velocity, move them and reset their velocity
            if (std::abs(player->velocity.x) > 0 || std::abs(player->velocity.y) > 0) {
                // start by normalizing and multiplying by speed
                player->velocity = glm::normalize(player->velocity) * player->speed;
                gameInfo.scene.moveActor(player, renderConfig.actorFlipping);
            }
            player->velocity = glm::vec2(0);
        }

        // make the input not "newly down" or "newly up" anymore
        input.lateUpdate();

        // if there's an intro, render it
        if (currentIntroIndex < gameConfig.introImages.size() || currentIntroIndex < gameConfig.introText.size()) {
            renderer.renderIntro(currentIntroIndex);
        }
        // handle and render gameplay
        else if (!gameOver) {
            // halt the intro music if it's playing
            if (introMusicPlaying) {
                AudioHelper::Mix_HaltChannel498(0);
                introMusicPlaying = false;
            }

            // start the gameplay music if there is any
            if (!gameplayMusicPlaying && gameConfig.gameplayMusic != "") {
                audioPlayer.play(gameConfig.gameplayMusic, -1);
                gameplayMusicPlaying = true;
            }

            // move all npc actors according to their velocity
            gameInfo.scene.moveNPCActors(renderConfig.actorFlipping);

            // update the camera position to match where the player is (because all actors have moved by now)
            camera.update(gameInfo.player, renderConfig.easeFactor);

            // render the game first
            renderer.render(gameInfo);

            if (player) {
                // render dialogue on top of the game
                renderer.renderDialogue(gameInfo);

                switch (state) {
                    case WIN:
                        if (!gameOverMusicPlaying &&
                            gameConfig.gameOverGoodAudio != "") {
                            AudioHelper::Mix_HaltChannel498(0);
                            audioPlayer.play(gameConfig.gameOverGoodAudio, 0);
                            gameOverMusicPlaying = true;
                            gameOver = true;
                        }
                        continue;
                    case LOSE:
                        if (!gameOverMusicPlaying &&
                            gameConfig.gameOverBadAudio != "") {
                            AudioHelper::Mix_HaltChannel498(0);
                            audioPlayer.play(gameConfig.gameOverBadAudio, 0);
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
                if (gameConfig.gameOverGoodImage != "") {
                    renderer.artist.drawUIImage(
                        gameConfig.gameOverGoodImage,
                        { 0, 0 },
                        { renderConfig.renderSize.x, renderConfig.renderSize.y }
                    );
                }
            }
            else if (gameInfo.state == LOSE) {
                if (gameConfig.gameOverBadImage != "") {
                    renderer.artist.drawUIImage(
                        gameConfig.gameOverBadImage,
                        { 0, 0 },
                        { renderConfig.renderSize.x, renderConfig.renderSize.y }
                    );
                }
            }
        }

        // Present the render AND DELAY, apparently it does that for us
        Helper::SDL_RenderPresent498(renderer.sdlRenderer);
    }
}

void Engine::queueStop() {
	isGameRunning = false;
}

int main(int argc, char* argv[]) {
    // for MacOS local runs
    //std::filesystem::current_path("/Users/lindsaygreig/Desktop/game_engine_nkhess");
    
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
