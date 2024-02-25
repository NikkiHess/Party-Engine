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
	void parse(rapidjson::Document& document, ResourceManager& resourceManager);
};

