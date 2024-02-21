// std library
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

// my code
#include "GameState.h"
#include "Actor.h"
#include "Scene.h"
#include "../utils/StringUtils.h"
#include "../errors/Error.h"
#include "../utils/ResourceManager.h"

// dependencies
#include "Helper.h"


void Scene::instantiateActor(Actor& actor) {
	glm::vec2 actorPos(actor.transform.pos.x, actor.transform.pos.y);
	actor.id = actors.size();
	
	// insert the actor into the list of actors
	actors.emplace_back(actor);

	// insert the actor into the "sorted-by-render-order" list
	actorsByRenderOrder.emplace(&actors.back());

	// if an actor has velocity, insert into motion list
	if (std::abs(actor.velocity.x) > 0 || std::abs(actor.velocity.y) > 0 || actor.name == "player") {
		motionActors.emplace(&actors.back());
	}

	// all actors that can collide need to be kept track of as well
	if (actor.boxCollider) {
		collisionActors.emplace(&actors.back());
	}

	// all actors that can trigger need to be kept track of as well
	if (actor.boxTrigger) {
		triggerActors.emplace(&actors.back());
	}
}

GameState Scene::moveAllActors(bool flipping, GameState& currentState, GameConfig& gameConfig, AudioPlayer& audioPlayer) {
	GameState outState = currentState;
	std::map<std::string, Actor*> dialogue;

	Actor* player = nullptr;

	for (Actor* actor : motionActors) {
		// possibly rendundant check, leaving it here just in case :)
		if (std::abs(actor->velocity.x) > 0 || std::abs(actor->velocity.y) > 0) {
			std::map<std::string, Actor*> newDialogue = moveActor(actor, flipping);
			for (auto& it : newDialogue) {
				dialogue[it.first] = it.second;
			}

			if (actor->name == "player") {
				player = actor;
				actor->velocity = glm::vec2(0);
			}
		}
	}

	if (player) {
		for (auto& it : dialogue) {
			outState = executeCommands(player, it.second, it.first, player->health, currentState, gameConfig, audioPlayer);
			// triggers print dialogue
			if (it.second->nearbyDialogue == it.first) {
				dialogueToRender.emplace(it.first);
				if (it.second->nearbySfx != "") {
					audioPlayer.play(it.second->nearbySfx, 0, Helper::GetFrameNumber() % 48 + 2);
				}
			}
		}
	}

	return outState;
}

std::map<std::string, Actor*> Scene::moveActor(Actor* actor, bool flipping) {
	std::map<std::string, Actor*> outDialogue;

	// check collisions
	checkCollisions(actor);

	// check triggers
	checkTriggers(actor);

	// NPCS: if collision, reverse velocity + move next turn
	// PLAYER: if collision, don't move
	// if no collision, keep moving
	if (actor->collidingActorsThisFrame.size() == 0) {
		// remove the old position of the actor from the unordered_map
		actorsByRenderOrder.erase(actor);

		// update the instanced actor's position if they wouldn't6 collide
		actor->transform.pos += actor->velocity;

		actor->handleFlipping(flipping);
		actor->handleVerticalFacing();

		if (actor->movementBounce) {
			actor->transform.bounce = true;
		}

		// add the updated position of the actor to the unordered_map
		actorsByRenderOrder.emplace(actor);
	}
	else {
		// players stand still
		if (actor->name == "player") {
			actor->velocity = glm::vec2(0);

			for (Actor* colliding : actor->collidingActorsThisFrame)
				outDialogue[colliding->contactDialogue] = colliding;
		}
		// NPCs reverse velocity
		else {
			actor->velocity = -actor->velocity;
		}
	}

	if (actor->triggeringActorsThisFrame.size() != 0) {
		for (Actor* triggering : actor->triggeringActorsThisFrame)
			outDialogue[triggering->nearbyDialogue] = triggering;
	}

	actor->collidingActorsThisFrame.clear();

	return outDialogue;
}

void Scene::checkCollisions(Actor* actor) {
	if (!actor->boxCollider)
		return;

	SDL_FRect future = *actor->boxCollider;
	// multiply by pixels-per-unit because velocity is in scene units
	future.x += actor->velocity.x * 100;
	future.y += actor->velocity.y * 100;

	for (Actor* other : collisionActors) {
		// make sure we don't check an actor against itself
		if (!other->boxCollider || actor == other) continue;

		if (SDL_HasIntersectionF(&future, &*other->boxCollider)) {
			actor->collidingActorsThisFrame.emplace(other);
			other->collidingActorsThisFrame.emplace(actor);
		}
	}
}

void Scene::checkTriggers(Actor* actor) {
	if (!actor->boxTrigger)
		return;

	SDL_FRect future = *actor->boxTrigger;
	// multiply by pixels-per-unit because velocity is in scene units
	future.x += actor->velocity.x * 100;
	future.y += actor->velocity.y * 100;

	for (Actor* other : triggerActors) {
		// make sure we don't check an actor against itself
		if (!other->boxTrigger || actor == other) continue;

		if (SDL_HasIntersectionF(&future, &*other->boxTrigger)) {
			actor->triggeringActorsThisFrame.emplace(other);
			other->triggeringActorsThisFrame.emplace(actor);
		}
	}
}

GameState Scene::executeCommands(Actor* player, Actor* trigger, const std::string& dialogue, int& health, GameState& currentState, GameConfig& gameConfig, AudioPlayer& audioPlayer) {
	// if the player can take damage/game over, execute these commands
	int cooldownOver = player->lastHealthDownFrame + player->healthDownCooldown;
	if (Helper::GetFrameNumber() >= cooldownOver) {
		if (dialogue.find("health down") != std::string::npos) {
			// if decreasing the player's health makes it <= 0, return a lose state
			--player->health;

			if (player->damageSfx != "") {
				audioPlayer.play(player->damageSfx, 0, Helper::GetFrameNumber() % 48 / 2);
			}
			// set this frame as the last time health was taken away
			player->lastHealthDownFrame = Helper::GetFrameNumber();
			trigger->lastAttackFrame = Helper::GetFrameNumber();

			// show damage on player and attack on 
			if(player->view.imageDamage.image)
				player->showDamage = true;
			if (trigger->view.imageAttack.image)
				trigger->showAttack = true;

			if (player->health <= 0) {
				return LOSE;
			}
		}
		if (dialogue.find("game over") != std::string::npos) {
			return LOSE;
		}
	}
	if (dialogue.find("score up") != std::string::npos) {
		// an NPC Actor may only trigger a score increase once
		if (!trigger->triggeredScoreUp) {
			if (gameConfig.scoreSfx != "") {
				audioPlayer.play(gameConfig.scoreSfx, 0, 1);
			}
			++player->score;
			trigger->triggeredScoreUp = true;
		}
	}
	if (dialogue.find("you win") != std::string::npos) {
		return WIN;
	}
	if (dialogue.find("proceed to") != std::string::npos) {
		return PROCEED;
	}
	return currentState;
}