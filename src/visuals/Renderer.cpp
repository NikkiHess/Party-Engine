// std library
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <algorithm>

// include my code
#include "Renderer.h"
#include "../GameEngine.h"
#include "../gamedata/GameInfo.h"
#include "../gamedata/Actor.h"
#include "../utils/StringUtils.h"

// dependencies
#include "rapidjson/document.h"
#include "glm/glm.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
#include "Helper.h"
#include "AudioHelper.h"

void Renderer::renderIntro(int& index) {
	// Clear the frame buffer at the beginning of a frame
	SDL_SetRenderDrawColor(
		sdlRenderer,
		configUtils.clearColor.r,
		configUtils.clearColor.g,
		configUtils.clearColor.b,
		1
	);
	SDL_RenderClear(sdlRenderer);

	// Display any intro images
	if (!configUtils.introImages.empty()) {
		artist.drawStaticImage(
			// exhausted introImages? continue to render last one
			(index < configUtils.introImages.size() ? configUtils.introImages[index] : configUtils.introImages[configUtils.introImages.size() - 1]),
			{0, 0}, // the position should be (0, 0)
			{ configUtils.renderSize.x, configUtils.renderSize.y } // stretch to fit render size
		);
	}
	// Display any intro text
	if (!configUtils.introText.empty()) {
		artist.drawText(
			// exhausted introText? continue to render last one
			(index < configUtils.introText.size() ? configUtils.introText[index] : configUtils.introText[configUtils.introText.size() - 1]),
			16,
			{ 255, 255, 255, 255 },
			{ 25, configUtils.renderSize.y - 50 } // the pos will be {25, 50 higher than the bottom of the screen}
		);
	}
}

void Renderer::render(GameInfo& gameInfo) {
	// Clear the frame buffer at the beginning of a frame
	SDL_SetRenderDrawColor(
		sdlRenderer, 
		configUtils.clearColor.r, 
		configUtils.clearColor.g, 
		configUtils.clearColor.b, 
		1
	);
	SDL_RenderClear(sdlRenderer);

	for (Actor& actor : gameInfo.currentScene.actors) {
		artist.drawActor(actor);
	}
}

void Renderer::renderHUD(GameInfo& gameInfo) {
	// only do any of this if the player exists
	if (gameInfo.player) {
		// render the player's score
		std::string scoreText = "score : " + std::to_string(gameInfo.player->score);
		artist.drawText(scoreText, 16, { 255, 255, 255, 255 }, { 5, 5 });

		// render the player's hp
		for (int i = 0; i < gameInfo.player->health; ++i) {
			glm::ivec2 size(0, 0);
			SDL_QueryTexture(artist.loadImageTexture(configUtils.hpImage), nullptr, nullptr, &size.x, &size.y);

			glm::ivec2 startPos{ 5, 25 };
			glm::ivec2 offset{ i * (size.x + 5), 0 };
			artist.drawStaticImage(
				configUtils.hpImage,
				startPos + offset,
				{ size.x, size.y }
			);
		}
	}
}

void Renderer::printDialogue(GameInfo& gameInfo) {
	std::stringstream dialogue; // the dialogue to be printed
	auto& locToActors = gameInfo.currentScene.locToActors;
	std::vector<Actor*> nearby;

	// loop over nearby locations and see if there are actors there, if so, play their dialogue
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			auto actorIt = locToActors.find(glm::dvec2(x, y) + gameInfo.player->transform.pos);
			if (actorIt != locToActors.end()) {
				nearby.insert(nearby.end(), actorIt->second.begin(), actorIt->second.end());
			}
		}
	}

	// sort the actors we are near
	if(nearby.size() > 1)
		std::sort(nearby.begin(), nearby.end(), ActorComparator());
	for (Actor* actor : nearby) {
		// let's not let the player do dialogue at all
		if (actor->name != "player") {
			if (actor->transform.pos == gameInfo.player->transform.pos) {
				actor->printContactDialogue();
				gameInfo.state = executeCommands(*actor, actor->contactDialogue, gameInfo);
				dialogue << actor->contactDialogue << "\n";
			}
			else {
				actor->printNearbyDialogue();
				gameInfo.state = executeCommands(*actor, actor->nearbyDialogue, gameInfo);
				dialogue << actor->nearbyDialogue << "\n";
			}
		}
	}
	
	if (gameInfo.state == PROCEED) {
		std::string sceneName = StringUtils::getWordAfterPhrase(dialogue.str(), "proceed to");
		if (sceneName != "") {
			//printStats(gameInfo);
			std::string scenePath = "resources/scenes/" + sceneName + ".scene";
			configUtils.checkFile(scenePath, "scene " + sceneName + " is");

			gameInfo.currentScene = Scene();
			gameInfo.currentScene.name = sceneName;
			configUtils.initializeScene(gameInfo.currentScene, configUtils.document, false);

			auto playerIt = std::find_if(gameInfo.currentScene.actors.begin(), gameInfo.currentScene.actors.end(), [](Actor actor) { return actor.name == "player"; });

			if (playerIt != gameInfo.currentScene.actors.end()) {
				gameInfo.player = &*playerIt;
			}
			render(gameInfo);
		}
	}
	//printStats(gameInfo);
}

void Renderer::promptPlayer(GameInfo& gameInfo) {
	std::cout << "Please make a decision..." << "\n";
	std::cout << "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"" << "\n";

	// receive the player's selection
	std::string selection;
	std::cin >> selection;

	if (selection == "quit") {
		gameInfo.state = LOSE;
	}
	// do movement (update player velocity)
	else if (selection == "n") {
		--gameInfo.player->velocity.y;
	}
	else if (selection == "e") {
		++gameInfo.player->velocity.x;
	}
	else if (selection == "s") {
		++gameInfo.player->velocity.y;
	}
	else if (selection == "w") {
		--gameInfo.player->velocity.x;
	}
}

// we've been told we can assume there will not be multiple commands at once
// execute all game commands from the given dialogue given the trigger Actor
// returns the game state generated from executing the command
GameState Renderer::executeCommands(Actor& trigger, const std::string& dialogue, GameInfo& gameInfo) {
	if (dialogue.find("health down") != std::string::npos) {
		// if decreasing the player's health makes it <= 0, return a lose state
		--gameInfo.player->health;
		if (gameInfo.player->health <= 0) {
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
	if (dialogue.find("game over") != std::string::npos) {
		return LOSE;
	}
	if (dialogue.find("proceed to") != std::string::npos) {
		return PROCEED;
	}
	return gameInfo.state;
}