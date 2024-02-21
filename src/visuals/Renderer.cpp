// std library
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>

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

std::string Renderer::renderNearbyDialogue(GameInfo& gameInfo) {
	std::vector<std::string> dialogue; // the dialogue to be printed
	std::stringstream dialogueStream;


	for (size_t i = 0; i < dialogue.size(); ++i) {
		artist.drawUIText(
			dialogue[i],
			{ 255, 255, 255, 255 },
			{ 25, configManager.renderingConfig.renderSize.y - 50 - (dialogue.size() - 1 - i) * 50 }
		);
	}

	return dialogueStream.str();
}