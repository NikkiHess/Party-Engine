// std libraries
#include <iostream>
#include <sstream>

// include my code
#include "GameEngine.h"

// dependencies
#include "dependencies/MapHelper.h"
#include "glm/glm.hpp"

void Engine::render() {
	// create a temp map for rendering
	char render_map[HARDCODED_MAP_HEIGHT][HARDCODED_MAP_WIDTH + 1];

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

	if (selection == "quit") {
		std::cout << game_over_bad_message;
		game_running = false;
	}
	// movement
	else if (selection == "n") {
		--hardcoded_actors.back().position.y;
	}
	else if (selection == "e") {
		++hardcoded_actors.back().position.x;
	}
	else if (selection == "s") {
		++hardcoded_actors.back().position.y;
	}
	else if (selection == "w") {
		--hardcoded_actors.back().position.x;
	}
}

void Engine::start() {
	while (game_running) {
		// print the initial render of the world
		render();

		// print stats
		show_stats();

		// prompt the player to take an action
		prompt_player();
	}
}

int main() {
	// print the starting message
	std::cout << game_start_message << "\n";

	Engine engine;
	engine.start();

	return 0;
}
