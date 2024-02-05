// std library
#include <iostream>
#include <sstream>
#include <cmath>
#include <unordered_map>
#include <algorithm>

// include my code
#include "Renderer.h"
#include "GameEngine.h"
#include "gamedata/GameInfo.h"
#include "gamedata/Actor.h"
#include "utils/StringUtils.h"

// dependencies
#include "rapidjson/document.h"
#include "glm/glm.hpp"

void Renderer::render(GameInfo& gameInfo) {
	std::stringstream render; // the rendered view

	// render bounds
	glm::ivec2 topLeft(gameInfo.player->position.x - (renderSize.x / 2), gameInfo.player->position.y - (renderSize.y / 2));
	glm::ivec2 bottomRight(gameInfo.player->position.x + (renderSize.x / 2), gameInfo.player->position.y + (renderSize.y / 2));

	// perform the render of the current view given the bounds
	for (int y = topLeft.y; y <= bottomRight.y; ++y) {
		for (int x = topLeft.x; x <= bottomRight.x; ++x) {
			auto it = gameInfo.currentScene.locToActors.find(glm::ivec2(x, y));
			if (it != gameInfo.currentScene.locToActors.end()) {
				Actor* highest_id = it->second.back();
				for (Actor* actor : it->second) {
					if (actor->id > highest_id->id)
						highest_id = actor;
				}

				render << highest_id->view;
			}
			else {
				render << " ";
			}
		}
		render << "\n";
	}

	// display the render
	std::cout << render.str();
}

void Renderer::printDialogue(GameInfo& gameInfo) {
	std::stringstream dialogue; // the dialogue to be printed
	auto& locToActors = gameInfo.currentScene.locToActors;
	std::vector<Actor*> nearby;

	// loop over nearby locations and see if there are actors there, if so, play their dialogue
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			auto actorIt = locToActors.find(glm::ivec2(x, y) + gameInfo.player->position);
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
			if (actor->position == gameInfo.player->position) {
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
			printStats(gameInfo);
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
	printStats(gameInfo);
}

void Renderer::printStats(GameInfo &gameInfo) {
	std::cout << "health : " << gameInfo.playerHealth << ", score : " << gameInfo.playerScore << "\n";
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
		--gameInfo.playerHealth;
		if (gameInfo.playerHealth <= 0) {
			return LOSE;
		}
	}
	if (dialogue.find("score up") != std::string::npos) {
		// an NPC Actor may only trigger a score increase once
		if (!trigger.triggeredScoreUp) {
			++gameInfo.playerScore;
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