// std libraries
#include <iostream>
#include <sstream>
#include <cmath>

// include my code
#include "GameEngine.h"
#include "GameState.h"

// dependencies
#include "../dependencies/MapHelper.h"
#include "../dependencies/rapidjson/document.h"
#include "glm/glm.hpp"

// ---------- BEGIN MOTION FUNCTIONS ----------

void Engine::update_positions() {
	for (Actor& actor : hardcoded_actors) {
		// if this is our player Actor, perform our player actor movement
		if (actor.actor_name == "player") {
			if (!would_collide(actor)) {
				actor.position += actor.velocity;
			}
			// stop the player's movement so they only move by 1
			actor.velocity = glm::ivec2(0, 0);
		}
		// move NPC Actors
		// if no collision, keep moving
		// if collision, reverse velocity (move next turn)
		else {
			glm::ivec2 updated_actor_pos(actor.position.x + actor.velocity.x, actor.position.y + actor.velocity.y);
			if (!would_collide(actor))
				actor.position = updated_actor_pos;
			else
				actor.velocity = -actor.velocity;
		}
	}
}

// check if an Actor would collide given its velocity
bool Engine::would_collide(Actor& actor) {
	glm::ivec2 future_position = actor.position + actor.velocity;
	// if the movement isn't blocked, allow the Actor to move
	bool is_blocked_by_actor = false;
	// this might be awful for performance?
	for (Actor& other_actor : hardcoded_actors) {
		if (other_actor.blocking) {
			if (future_position == other_actor.position) {
				is_blocked_by_actor = true;
				break;
			}
		}
	}

	return hardcoded_map[future_position.y][future_position.x] == 'b' ||
		is_blocked_by_actor;
}

// ----------- END MOTION FUNCTIONS -----------

// ----------- BEGIN CORE FUNCTIONS -----------

void Engine::start() {
	// store all actors in triggered_score_up (false)
	for (Actor& actor : hardcoded_actors) {
		triggered_score_up[&actor] = false;
	}

	game_running = true;

	while (game_running) {
		// print the initial render of the world
		renderer.render(player);

		GameState dialogue_state = renderer.print_dialogue(player, player_health, player_score, triggered_score_up);
		if (dialogue_state != NORMAL) {
			stop();
			break;
		}

		// prompt the player to take an action
		GameState prompt_state = renderer.prompt_player(player);
		if (prompt_state != NORMAL) {
			stop();
			break;
		}
		
		// update Actor positions
		update_positions();
	}
}

void Engine::stop() {
	game_running = false;
}

// ----------- END CORE FUNCTIONS ------------

int main() {
	// print the starting message
	std::cout << game_start_message << "\n";

	glm::ivec2 renderSize((13, 9));

	Renderer renderer(renderSize);
	Engine engine(renderer);
	engine.start();

	return 0;
}
