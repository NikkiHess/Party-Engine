// std library
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>

// include my code
#include "../gamedata/Actor.h"
#include "../gamedata/GameInfo.h"
#include "../GameEngine.h"
#include "../utils/StringUtils.h"
#include "Renderer.h"

// dependencies
#include "AudioHelper.h"
#include "glm/glm.hpp"
#include "Helper.h"
#include "rapidjson/document.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"

void Renderer::renderIntro(size_t& index) {
	GameConfig& gameConfig = configManager.gameConfig;
	RenderingConfig& renderConfig = configManager.renderingConfig;

	// Clear the frame buffer at the beginning of a frame
	SDL_SetRenderDrawColor(
		sdlRenderer,
		renderConfig.clearColor.r,
		renderConfig.clearColor.g,
		renderConfig.clearColor.b,
		1
	);
	SDL_RenderClear(sdlRenderer);


	// Display any intro images
	if (!gameConfig.introImages.empty()) {
		artist.drawUIImage(
			// exhausted introImages? continue to render last one
			(index < gameConfig.introImages.size() ? gameConfig.introImages[index] : gameConfig.introImages[gameConfig.introImages.size() - 1]),
			{0, 0}, // the position should be (0, 0)
			{ renderConfig.renderSize.x, renderConfig.renderSize.y } // stretch to fit render size
		);
	}
	// Display any intro text
	if (!gameConfig.introText.empty()) {
		artist.drawUIText(
			// exhausted introText? continue to render last one
			(index < gameConfig.introText.size() ? gameConfig.introText[index] : gameConfig.introText[gameConfig.introText.size() - 1]),
			{ 255, 255, 255, 255 },
			{ 25, renderConfig.renderSize.y - 50 } // the pos will be {25, 50 higher than the bottom of the screen}
		);
	}
}

void Renderer::render(GameInfo& gameInfo) {
	RenderingConfig& renderConfig = configManager.renderingConfig;

	// Clear the frame buffer at the beginning of a frame
	SDL_SetRenderDrawColor(
		sdlRenderer, 
		renderConfig.clearColor.r,
		renderConfig.clearColor.g,
		renderConfig.clearColor.b,
		1
	);
	SDL_RenderClear(sdlRenderer);

	// set the render scale according to the configured zoom factor
	SDL_RenderSetScale(sdlRenderer, renderConfig.zoomFactor, renderConfig.zoomFactor);

	// draw all actors in order of transform_position_y
	for (Actor* actor : gameInfo.scene.actorsByRenderOrder) {
		artist.drawActor(*actor, gameInfo.camera);
	}
}

void Renderer::renderHUD(GameInfo& gameInfo) {
	// set the render scale according to the configured zoom factor
	SDL_RenderSetScale(sdlRenderer, 1, 1);

	// render the player's score
	std::string scoreText = "score : " + std::to_string(gameInfo.player->score);
	artist.drawUIText(scoreText, { 255, 255, 255, 255 }, { 5, 5 });

	GameConfig& gameConfig = configManager.gameConfig;

	// render the player's hp
	for (int i = 0; i < gameInfo.player->health; ++i) {
		glm::ivec2 size(0, 0);
		SDL_QueryTexture(artist.resourceManager.loadImageTexture(gameConfig.hpImage), nullptr, nullptr, &size.x, &size.y);

		glm::ivec2 startPos{ 5, 25 };
		glm::ivec2 offset{ i * (size.x + 5), 0 };
		artist.drawUIImage(
			gameConfig.hpImage,
			startPos + offset,
			{ size.x, size.y }
		);
	}
}

void Renderer::renderDialogue(GameInfo& gameInfo) {
	std::vector<std::string> dialogue; // the dialogue to be printed
	std::vector<Actor*> nearby;

	// loop over nearby locations and see if there are actors there, if so, play their dialogue
	// these actors SHOULD already be in order considering locToActors uses a set of actors in order of id
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			auto actorIt = gameInfo.scene.locToActors.find(gameInfo.player->transform.pos + glm::vec2(x, y));
			if (actorIt != gameInfo.scene.locToActors.end()) {
				for (Actor* actor : actorIt->second) {
					// only push back if they have dialogue and ARE NOT the player
					if (actor->name != "player" && (actor->nearbyDialogue != "" || actor->contactDialogue != "")) {
						nearby.push_back(actor);
					}
				}
			}
		}
	}

	for (Actor* actor : nearby) {
		std::string name = actor->name;
		if (actor->transform.pos == gameInfo.player->transform.pos) {
			gameInfo.state = executeCommands(*actor, actor->contactDialogue, gameInfo);
			dialogue.push_back(actor->contactDialogue);
		}
		else {
			gameInfo.state = executeCommands(*actor, actor->nearbyDialogue, gameInfo);
			dialogue.push_back(actor->nearbyDialogue);
		}
	}
	
	if (gameInfo.state == PROCEED) {
		for (std::string& dialogueStr : dialogue) {
			std::string sceneName = StringUtils::getWordAfterPhrase(dialogueStr, "proceed to");
			if (sceneName != "") {
				std::string scenePath = "resources/scenes/" + sceneName + ".scene";

				if (!resourceManager.fileExists(scenePath)) Error::error("scene " + sceneName + " is missing");

				gameInfo.scene = Scene();
				gameInfo.scene.name = sceneName;
				// initialize the new scene immediately
				configManager.sceneConfig.parse(configManager.document, resourceManager, gameInfo.scene, configManager.gameConfig.hpImage);

				auto playerIt = std::find_if(gameInfo.scene.actors.begin(), gameInfo.scene.actors.end(), [](Actor actor) { return actor.name == "player"; });

				if (playerIt != gameInfo.scene.actors.end()) {
					gameInfo.player = &*playerIt;
				}
			}
		}
	}

	for (size_t i = 0; i < dialogue.size(); ++i) {
		artist.drawUIText(
			dialogue[i],
			{ 255, 255, 255, 255 },
			{ 25, configManager.renderingConfig.renderSize.y - 50 - (dialogue.size() - 1 - i) * 50 }
		);
	}
}

// we've been told we can assume there will not be multiple commands at once
// execute all game commands from the given dialogue given the trigger Actor
// returns the game state generated from executing the command
GameState Renderer::executeCommands(Actor& trigger, const std::string& dialogue, GameInfo& gameInfo) {
	// if the player can take damage/game over, execute these commands
	int cooldownOver = gameInfo.player->lastHealthDownFrame + gameInfo.player->healthDownCooldown;
	if (Helper::GetFrameNumber() >= cooldownOver) {
		if (dialogue.find("health down") != std::string::npos) {
			// if decreasing the player's health makes it <= 0, return a lose state
			--gameInfo.player->health;
			// set this frame as the last time health was taken away
			gameInfo.player->lastHealthDownFrame = Helper::GetFrameNumber();
			if (gameInfo.player->health <= 0) {
				return LOSE;
			}
		}
		if (dialogue.find("game over") != std::string::npos) {
			return LOSE;
		}
	}
	if (dialogue.find("score up") != std::string::npos) {
		// an NPC Actor may only trigger a score increase once
		if (!trigger.triggeredScoreUp) {
			++gameInfo.player->score;
			trigger.triggeredScoreUp = true;
		}
	}
	if (dialogue.find("you win") != std::string::npos) {
		return WIN;
	}
	if (dialogue.find("proceed to") != std::string::npos) {
		return PROCEED;
	}
	return gameInfo.state;
}