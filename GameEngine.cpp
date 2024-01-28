// std libraries
#include <iostream>
#include <sstream>

// include my code
#include "GameEngine.h"

// dependencies
#include "dependencies/MapHelper.h"
#include "glm/glm.hpp"

void Engine::render() {
	// copy hardcoded map into render map
	for (int y = 0; y < HARDCODED_MAP_HEIGHT; ++y) {
		for (int x = 0; x < HARDCODED_MAP_WIDTH + 1; ++x) {
			render_map[y][x] = hardcoded_map[y][x];
		}
	}

	// add the actors to the map
	for (const Actor& actor : hardcoded_actors) {
		render_map[actor.position.y][actor.position.x] = actor.view;
	}

	// perform the render of the current view
	std::stringstream render;

	glm::ivec2 renderSize(13, 9);
	Actor& player = hardcoded_actors.back();

	// render bounds
	// this is awful
	glm::ivec2 topLeft(player.position.x - (renderSize.x / 2), player.position.y - (renderSize.y / 2));
	glm::ivec2 bottomRight(player.position.x + (renderSize.x / 2), player.position.y + (renderSize.y / 2));

	for (int y = topLeft.y; y <= bottomRight.y; ++y) {
		for (int x = topLeft.x; x <= bottomRight.x; ++x) {
			render << render_map[y][x];
		}
		render << "\n";
	}

	std::cout << render.str();
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
	glm::ivec2 playerPos = player.position;

	if (selection == "quit") {
		std::cout << game_over_bad_message;
		stop();
	}
	// movement
	else if (selection == "n") {
		--playerPos.y;
	}
	else if (selection == "e") {
		++playerPos.x;
	}
	else if (selection == "s") {
		++playerPos.y;
	}
	else if (selection == "w") {
		--playerPos.x;
	}

	// if the movement isn't blocked, allow the player to move
	if (hardcoded_map[playerPos.y][playerPos.x] != 'b') {
		player.position = playerPos;
	}
}

void Engine::print_dialogue() {
	Actor& player = hardcoded_actors.back();

	// check around the player for NPCs
	for (int y = -1; y <= 1; ++y) {
		for (int x = -1; x <= 1; ++x) {
			// collision dialogue
			if (x == 0 && y == 0) {

			}
			// nearby dialogue
			else {

			}
		}
	}
}

void Engine::start() {
	while (game_running) {
		// print the initial render of the world
		render();

		// print any dialogue we may have run into
		print_dialogue();

		// print stats
		show_stats();

		// prompt the player to take an action
		prompt_player();
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
