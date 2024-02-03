// std libraries
#include <iostream>
#include <sstream>
#include <cmath>
#include <algorithm>

// include my code
#include "GameEngine.h"
#include "ConfigHelper.h"

// dependencies
#include "../dependencies/rapidjson/document.h"
#include "glm/glm.hpp"

// ---------- BEGIN MOTION FUNCTIONS ----------

// TODO: MOVE THIS TO THE ACTOR CLASS
void Engine::update_positions() {
	for (Actor& actor : game_info.current_scene.actors) {
		// if this is our player Actor, perform our player actor movement
		if (actor.actor_name == "player") {
			if (!would_collide(actor)) {
				update_actor_position(actor);
			}
			// stop the player's movement so they only move by 1
			actor.velocity = glm::ivec2(0, 0);
		}
		// move NPC Actors
		// if no collision, keep moving
		// if collision, reverse velocity (move next turn)
		else {
			if (!would_collide(actor))
				update_actor_position(actor);
			else
				actor.velocity = -actor.velocity;
		}
	}
}

void Engine::update_actor_position(Actor& actor) {
	// remove the old position of the actor from the unordered_map
	auto& loc_to_actors = game_info.current_scene.loc_to_actors;
	auto old_it = loc_to_actors.find(actor.position);
	if (old_it != loc_to_actors.end()) {
		auto& at_pos = old_it->second;
		at_pos.erase(std::remove_if(at_pos.begin(), at_pos.end(),
			[&actor](const Actor* a) { 
				return a->id == actor.id; 
			}), at_pos.end());
		if (at_pos.empty()) {
			loc_to_actors.erase(actor.position);
		}
	}

	// update the instanced actor's positon
	actor.position += actor.velocity;

	// add the new position of the actor to the unordered_map
	auto new_it = loc_to_actors.find(actor.position);
	if (new_it != loc_to_actors.end())
		new_it->second.push_back(&actor);
	else
		loc_to_actors.insert({ actor.position, {&actor} });
}

// TODO: MOVE THIS TO THE ACTOR CLASS
// check if an Actor would collide given its velocity
bool Engine::would_collide(Actor& actor) {
	glm::ivec2 future_position = actor.position + actor.velocity;

	auto it = game_info.current_scene.loc_to_actors.find(future_position);
	if (it != game_info.current_scene.loc_to_actors.end()) {
		for (Actor* other : it->second) {
			if (other->blocking)
				return true;
		}
	}

	return false;
}

// ----------- END MOTION FUNCTIONS -----------

// ----------- BEGIN CORE FUNCTIONS -----------

void Engine::handle_state() {
	// Should the Engine class handle printing these messages, or should the Renderer?
	switch (game_info.state) {
	case WIN:
		if(game_info.game_over_good_message != "")
			std::cout << game_info.game_over_good_message;
		stop();
		break;
	case LOSE:
		if (game_info.game_over_bad_message != "")
			std::cout << game_info.game_over_bad_message;
		stop();
		break;
	}
}

void Engine::start() {
	// print the starting message
	if (game_info.game_start_message != "")
		std::cout << game_info.game_start_message << "\n";

	// store all actors in triggered_score_up (false)
	for (Actor& actor : game_info.current_scene.actors) {
		triggered_score_up[&actor] = false;
	}

	game_running = true;

	while (game_running) {
		// print the initial render of the world
		renderer.render(game_info);

		renderer.print_dialogue(game_info);
		handle_state();

		// prompt the player to take an action
		renderer.prompt_player(game_info);
		handle_state();
		
		// update Actor positions
		update_positions();
	}
}

void Engine::stop() {
	game_running = false;
	exit(0);
}

// ----------- END CORE FUNCTIONS ------------

int main() {
	ConfigHelper config_helper;
	Renderer renderer(config_helper.render_size);

	Engine engine(renderer, config_helper);
	engine.start();

	return 0;
}
