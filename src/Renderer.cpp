// std libraries
#include <iostream>
#include <sstream>
#include <cmath>
#include <map>

// include my code
#include "Renderer.h"
#include "GameEngine.h"
#include "GameInfo.h"

// dependencies
#include "MapHelper.h"
#include "rapidjson/document.h"
#include "glm/glm.hpp"

void Renderer::render(GameInfo& game_info) {
	std::stringstream render; // the rendered view

	// copy hardcoded map into render map
	for (int y = 0; y < HARDCODED_MAP_HEIGHT; ++y) {
		for (int x = 0; x < HARDCODED_MAP_WIDTH + 1; ++x) {
			render_map[y][x] = hardcoded_map[y][x];
		}
	}

	// add the actors to the map
	for (Actor& actor : hardcoded_actors) {
		render_map[actor.position.y][actor.position.x] = actor.view;
	}

	// perform the render of the current view
	glm::ivec2 renderSize(13, 9);

	// render bounds
	glm::ivec2 top_left(game_info.player.position.x - (renderSize.x / 2), game_info.player.position.y - (renderSize.y / 2));
	glm::ivec2 bottom_right(game_info.player.position.x + (renderSize.x / 2), game_info.player.position.y + (renderSize.y / 2));

	for (int y = top_left.y; y <= bottom_right.y; ++y) {
		for (int x = top_left.x; x <= bottom_right.x; ++x) {
			// if within bounds, print
			if (y >= 0 && y < HARDCODED_MAP_HEIGHT &&
				x >= 0 && x < HARDCODED_MAP_WIDTH)
				render << render_map[y][x];
			else render << ' ';
		}
		render << "\n";
	}

	// render, then display dialogue and command output
	std::cout << render.str();
}

void Renderer::print_dialogue(GameInfo& game_info) {
	std::stringstream dialogue; // the dialogue to be printed
	std::string command_output; // the output from commands

	for (Actor& actor : hardcoded_actors) {
		glm::ivec2 dist(abs(actor.position.x - game_info.player.position.x), abs(actor.position.y - game_info.player.position.y));
		// actor within 1 x and y of the player? print nearby dialogue
		if ((dist.y == 1 && dist.x <= 1) ||
			(dist.y <= 1 && dist.x == 1)) {
			if (actor.nearby_dialogue != "") {
				dialogue << actor.nearby_dialogue << "\n";

				std::string str = dialogue.str();
				game_info.state = execute_commands(actor, str, game_info);
			}
		}
		// within 0? print contact dialogue
		else if (dist.y == 0 && dist.x == 0) {
			if (actor.contact_dialogue != "") {
				dialogue << actor.contact_dialogue << "\n";

				std::string str = dialogue.str();
				game_info.state = execute_commands(actor, str, game_info);
			}
		}
	}

	std::cout << dialogue.str();
	print_stats(game_info);
	switch (game_info.state) {
	case WIN:
		std::cout << game_over_good_message;
		break;
	case LOSE:
		std::cout << game_over_bad_message;
		break;
	}
}

void Renderer::print_stats(GameInfo &game_info) {
	std::cout << "health : " << game_info.player_health << ", score : " << game_info.player_score << "\n";
}

void Renderer::prompt_player(GameInfo& game_info) {
	std::cout << "Please make a decision..." << "\n";
	std::cout << "Your options are \"n\", \"e\", \"s\", \"w\", \"quit\"" << "\n";

	// receive the player's selection
	std::string selection;
	std::cin >> selection;

	if (selection == "quit") {
		std::cout << game_over_bad_message;
		game_info.state = LOSE;
	}
	// do movement (update player velocity)
	else if (selection == "n") {
		--game_info.player.velocity.y;
	}
	else if (selection == "e") {
		++game_info.player.velocity.x;
	}
	else if (selection == "s") {
		++game_info.player.velocity.y;
	}
	else if (selection == "w") {
		--game_info.player.velocity.x;
	}
}

// TODO: Can I get a different class to handle this?
// This doesn't really feel right for Renderer

// we've been told we can assume there will not be multiple commands at once
// execute all game commands from the given dialogue given the trigger Actor
// returns the game state generated from executing the command
GameState Renderer::execute_commands(Actor& trigger, const std::string& dialogue, GameInfo& game_info) {
	if (dialogue.find("health down") != std::string::npos) {
		// if decreasing the player's health makes it <= 0, return a lose state
		if (--game_info.player_health <= 0) {
			return LOSE;
		}
	}
	else if (dialogue.find("score up") != std::string::npos) {
		// an NPC Actor may only trigger a score increase once
		if (!game_info.triggered_score_up[&trigger]) {
			++game_info.player_score;
			game_info.triggered_score_up[&trigger] = true;
		}
	}
	else if (dialogue.find("you win") != std::string::npos) {
		return WIN;
	}
	else if (dialogue.find("game over") != std::string::npos) {
		return LOSE;
	}
	return NORMAL;
}