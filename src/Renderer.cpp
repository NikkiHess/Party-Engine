// std libraries
#include <iostream>
#include <sstream>
#include <cmath>
#include <map>

// include my code
#include "Renderer.h"
#include "GameEngine.h"
#include "gamedata/GameInfo.h"
#include "gamedata/Actor.h"

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
				render << it->second.back()->view;
			}
			else {
				render << " ";
			}
		}
		render << "\n";
	}

	// render, then display dialogue and command output
	std::cout << render.str();
}

void Renderer::printDialogue(GameInfo& gameInfo) {
	std::stringstream dialogue; // the dialogue to be printed
	auto& locToActors = gameInfo.currentScene.locToActors;

	// loop over nearby locations and see if there are actors there, if so, play their dialogue
	for (int y = -1; y < 1; ++y) {
		for (int x = -1; x < 1; ++x) {
			auto actorIt = locToActors.find(glm::ivec2(x, y));
			if (actorIt != locToActors.end()) {
				if (x == 0 && y == 0) {
					for (Actor* collided : locToActors[glm::ivec2(x, y)]) {
						collided->printContactDialogue();

						gameInfo.state = executeCommands(*collided, collided->contactDialogue, gameInfo);
					}
				}
				else {
					for (Actor* nearby : locToActors[glm::ivec2(x, y)]) {
						nearby->printNearbyDialogue();

						gameInfo.state = executeCommands(*nearby, nearby->nearbyDialogue, gameInfo);
					}
				}
			}
		}
	}

	std::cout << dialogue.str();
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
	else if (dialogue.find("score up") != std::string::npos) {
		// an NPC Actor may only trigger a score increase once
		if (!gameInfo.triggeredScoreUp[&trigger]) {
			++gameInfo.playerScore;
			gameInfo.triggeredScoreUp[&trigger] = true;
		}
	}
	else if (dialogue.find("you win") != std::string::npos) {
		return WIN;
	}
	else if (dialogue.find("game over") != std::string::npos) {
		return LOSE;
	}
	return gameInfo.state;
}