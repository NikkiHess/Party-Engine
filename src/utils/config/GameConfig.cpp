// my code
#include "GameConfig.h"

std::string GameConfig::parse(rapidjson::Document& document, ResourceManager& resourceManager) {
	// handle game title
	if (document.HasMember("game_title"))
		gameTitle = document["game_title"].GetString();

	// handle initial scene's .scene file
	if (!document.HasMember("initial_scene")) {
		Error::error("initial_scene unspecified");
	}
	// the name of the initial scene
	std::string sceneName = document["initial_scene"].GetString();
	if (!resourceManager.fileExists("resources/scenes/" + sceneName + ".scene")) {
		Error::error("scene " + sceneName + " is missing");
	}

	return sceneName;
}