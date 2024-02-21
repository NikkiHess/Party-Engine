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
	std::vector<std::string> introImages;
	std::vector<std::string> introText;
	std::string introMusic = "";
	std::string gameplayMusic = "";
	std::string hpImage = "";
	std::string gameOverBadImage = "", gameOverBadAudio = "",
		gameOverGoodImage = "", gameOverGoodAudio = "";

	std::string damageSfx = "", scoreSfx = "";

	float playerSpeed = 0.02f;

	// initializes from game.config
	void parse(rapidjson::Document& document, ResourceManager& resourceManager);


};

