#include "ConfigHelper.h"

// std stuff
#include <string>
#include <iostream>
#include <filesystem>

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

bool ConfigHelper::check_file(const std::string& path) {
	if (!std::filesystem::exists(path)) {
		std::cout << "error: " + path + " missing";
		exit(0);
	}
}

void ConfigHelper::initialize_messages(rapidjson::Document& document) {
	if (document.HasMember("game_start_message"))
		game_start_message = document["game_start_message"].GetString();
	if (document.HasMember("game_over_bad_message"))
		game_over_bad_message = document["game_over_bad_message"].GetString();
	if (document.HasMember("game_over_good_message"))
		game_over_good_message = document["game_over_good_message"].GetString();
}

void ConfigHelper::initialize_rendering(rapidjson::Document& document) {
	// we've been told in the spec we can assume it will always be odd, but...
	if (document.HasMember("x_resolution"))
		render_size.x = document["x_resolution"].GetInt();
	if (document.HasMember("y_resolution"))
		render_size.y = document["y_resolution"].GetInt();

	if (render_size.x % 2 == 0 || render_size.y % 2 == 0) {
		std::cout << "Render size must be odd.";
		exit(0); // should this be a different exit code?
	}
}