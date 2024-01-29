// std libraries
#include <iostream>
#include <sstream>
#include <cmath>

// include my code
#include "GameEngine.h"

// dependencies
#include "dependencies/MapHelper.h"
#include "glm/glm.hpp"

void Engine::render() {
	Actor& player = hardcoded_actors.back();
	std::stringstream render;
	std::stringstream dialogue;

	// copy hardcoded map into render map
	for (int y = 0; y < HARDCODED_MAP_HEIGHT; ++y) {
		for (int x = 0; x < HARDCODED_MAP_WIDTH + 1; ++x) {
			render_map[y][x] = hardcoded_map[y][x];
		}
	}

	// add the actors to the map
	// also handle any necessary dialogue to be printed later
	for (const Actor& actor : hardcoded_actors) {
		render_map[actor.position.y][actor.position.x] = actor.view;

		glm::ivec2 dist(abs(actor.position.x - player.position.x), abs(actor.position.y - player.position.y));
		// actor within 1 x and y of the player? print nearby dialogue
		if ((dist.y == 1 && dist.x <= 1) ||
			(dist.y <= 1 && dist.x == 1)) {
			if(actor.nearby_dialogue != "") 
				dialogue << actor.nearby_dialogue << "\n";
		}
		// within 0? print contact dialogue
		else if (dist.y == 0 && dist.x == 0) {
			if (actor.contact_dialogue != "")
				dialogue << actor.contact_dialogue << "\n";
		}
	}

	// perform the render of the current view
	glm::ivec2 renderSize(13, 9);

	// render bounds
	glm::ivec2 top_left(player.position.x - (renderSize.x / 2), player.position.y - (renderSize.y / 2));
	glm::ivec2 bottom_right(player.position.x + (renderSize.x / 2), player.position.y + (renderSize.y / 2));

	for (int y = top_left.y; y <= bottom_right.y; ++y) {
		for (int x = top_left.x; x <= bottom_right.x; ++x) {
			// if within bounds, print
			if (y >= 0 && y < HARDCODED_MAP_HEIGHT &&
				x >= 0 && x < HARDCODED_MAP_WIDTH + 1)
				render << render_map[y][x];
			else render << " ";
		}
		render << "\n";
	}

	// render, then display dialogue
	std::cout << render.str();
	std::cout << dialogue.str();
}

void Engine::show_stats() {
	std::cout << "health : 3, score : 0" << "\n";
}

void Engine::prompt_player() {
	std::cout << "Please make a decision..." << "\n";
	std::cout << "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"" << "\n";

	// receive the player's selection
	std::string selection;
	std::cin >> selection;

	Actor& player = hardcoded_actors.back();
	// a temporary variable to check updates to the player's position
	updated_player_pos = player.position;

	if (selection == "quit") {
		std::cout << game_over_bad_message;
		stop();
	}
	// movement
	else if (selection == "n") {
		--updated_player_pos.y;
	}
	else if (selection == "e") {
		++updated_player_pos.x;
	}
	else if (selection == "s") {
		++updated_player_pos.y;
	}
	else if (selection == "w") {
		--updated_player_pos.x;
	}
}

void Engine::update_positions() {
	for (Actor& actor : hardcoded_actors) {
		// if this is our player Actor, perform our player actor movement
		if (actor.actor_name == "player") {
			if(!would_collide(actor, updated_player_pos))
				actor.position = updated_player_pos;
		}
		// move NPC Actors
		// if no collision, keep moving
		// if collision, reverse velocity (move next turn)
		else {
			glm::ivec2 updated_actor_pos(actor.position.x + actor.velocity.x, actor.position.y + actor.velocity.y);
			if (!would_collide(actor, updated_actor_pos))
				actor.position = updated_actor_pos;
			else
				actor.velocity = -actor.velocity;
		}
	}
}

bool Engine::would_collide(Actor& actor, glm::ivec2& position) {
	// if the movement isn't blocked, allow the Actor to move
	bool is_blocked_by_actor = false;
	// this might be awful for performance?
	for (Actor& other_actor : hardcoded_actors) {
		if (other_actor.blocking) {
			if (position == other_actor.position) {
				is_blocked_by_actor = true;
				break;
			}
		}
	}

	return hardcoded_map[position.y][position.x] == 'b' || is_blocked_by_actor;
}

void Engine::start() {
	game_running = true;

	while (game_running) {
		// print the initial render of the world
		render();

		// print stats
		show_stats();

		// prompt the player to take an action
		prompt_player();
		
		// update Actor positions
		update_positions();
	}
}

void Engine::stop() {
	game_running = false;
}

int main() {
	// print the starting message
	std::cout << game_start_message << "\n";

	Engine engine;
	engine.start();

	return 0;
}
