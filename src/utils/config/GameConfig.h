#pragma once

// std library
#include <string>
#include <vector>

// my code
#include "../../errors/Error.h"
#include "../ResourceManager.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class GameConfig {
public:
	std::string gameTitle = "";

	// initializes from game.config
	// returns the name of the initial_scene
	std::string parse(rapidjson::Document& document, ResourceManager& resourceManager);
};

