// std libraries
#include <iostream>
#include <sstream>

// include my code
#include "GameEngine.h"

// dependencies
#include "dependencies/MapHelper.h"
#include "glm/glm.hpp"

static void print_render() {
	// create a temp map for rendering
	char render_map[HARDCODED_MAP_HEIGHT][HARDCODED_MAP_WIDTH + 1];

	// copy hardcoded map into render map
	for (unsigned int y = 0; y < HARDCODED_MAP_HEIGHT; ++y) {
		for (unsigned int x = 0; x < HARDCODED_MAP_WIDTH + 1; ++x) {
			render_map[y][x] = hardcoded_map[y][x];
		}
	}

	// add the actors to the map
	for (const Actor &actor : hardcoded_actors) {
		render_map[actor.position.y][actor.position.x] = actor.view;
	}
	
	// perform the render of the current view
	std::stringstream render;

	// initial playerPos is (19, 15) and renderSize is (13, 9)
	for (unsigned int y = 11; y <= 19; ++y) {
		for (unsigned int x = 13; x <= 25; ++x) {
			render << render_map[y][x];
		}
		render << "\n";
	}

	std::cout << render.str();
}

static void print_stats() {
	std::cout << "health : 3, score : 0" << "\n";
}

void prompt_player(bool &game_running) {
	std::cout << "Please make a decision..." << "\n";
	std::cout << "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"" << "\n";

	// receive the player's selection
	std::string selection;
	std::cin >> selection;

	if (selection == "quit") {
		std::cout << game_over_bad_message;
		game_running = false;
	}
}

int main() {
	// print the starting message
	std::cout << game_start_message << "\n";

	bool game_running = true;

	while (game_running) {
		// print the initial render of the world
		print_render();

		// print stats
		print_stats();

		// prompt the player to move
		prompt_player(game_running);
	}
	return 0;
}
