#include "GameConfig.h"

void GameConfig::parse(rapidjson::Document& document, ResourceManager& resourceManager) {
	// handle game title
	if (document.HasMember("game_title"))
		gameTitle = document["game_title"].GetString();

	// handle font
	if (document.HasMember("font")) {
		std::string fontName = document["font"].GetString();
		std::string fontPath = "resources/fonts/" + fontName + ".ttf";

		if (!resourceManager.fileExists(fontPath))
			Error::error("font " + fontName + " missing");

		// open the font specified
		resourceManager.font = TTF_OpenFont(fontPath.c_str(), 16);
	}
}