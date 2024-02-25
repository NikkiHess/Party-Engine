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

	// handle the intro
	if (document.HasMember("intro_image")) {
		rapidjson::GenericArray images = document["intro_image"].GetArray();
		introImages.reserve(images.Size());

		// Copy the images over one by one, checking each along the way
		for (rapidjson::Value& image : images) {
			std::string imgString = image.GetString();
			if (!resourceManager.fileExists("resources/images/" + imgString + ".png")) {
				Error::error("missing image " + imgString);
			}

			introImages.emplace_back(image.GetString());
		}
	}
	if (document.HasMember("intro_text")) {
		if (resourceManager.font == nullptr) {
			Error::error("text render failed. No font configured");
		}
		rapidjson::GenericArray texts = document["intro_text"].GetArray();
		introText.reserve(texts.Size());

		for (rapidjson::Value& text : texts) {
			introText.emplace_back(text.GetString());
		}
	}
	if (document.HasMember("intro_bgm")) {
		introMusic = document["intro_bgm"].GetString();
	}
}