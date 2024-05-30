#pragma once

// std library
#include <string>

// my code
#include "../ResourceManager.h"

// dependencies
#include "rapidjson/document.h"

class GameConfig {
public:
	std::string gameTitle = "";

	// initializes from game.config
	// returns the name of the initial_scene
	std::string parse(rapidjson::Document& document, ResourceManager& resourceManager);
};

