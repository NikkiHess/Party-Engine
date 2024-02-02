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

void ConfigHelper::initialize_messages() {
	game_start_message, game_over_bad_message, game_over_good_message = "";
	if (document.HasMember("game_start_message"))
		game_start_message = document["game_start_message"].GetString();
	if (document.HasMember("game_over_bad_message"))
		game_over_bad_message = document["game_over_bad_message"].GetString();
	if (document.HasMember("game_over_good_message"))
		game_over_good_message = document["game_over_good_message"].GetString();
}