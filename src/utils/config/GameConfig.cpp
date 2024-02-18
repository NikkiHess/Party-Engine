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

	// handle the gameplay
	if (document.HasMember("gameplay_audio")) {
		gameplayMusic = document["gameplay_audio"].GetString();
	}

	// HUD
	if (document.HasMember("hp_image")) {
		std::string hpImageName = document["hp_image"].GetString();
		std::string hpImagePath = "resources/images/" + hpImageName + ".png";

		if (!resourceManager.fileExists(hpImagePath)) Error::error(hpImageName + " missing");

		hpImage = hpImageName;
	}

	// handle the game over sequence
	if (document.HasMember("game_over_bad_image")) {
		gameOverBadImage = document["game_over_bad_image"].GetString();
	}
	if (document.HasMember("game_over_bad_audio")) {
		gameOverBadAudio = document["game_over_bad_audio"].GetString();
	}
	if (document.HasMember("game_over_good_image")) {
		gameOverGoodImage = document["game_over_good_image"].GetString();
	}
	if (document.HasMember("game_over_good_audio")) {
		gameOverGoodAudio = document["game_over_good_audio"].GetString();
	}

	if (document.HasMember("player_movement_speed")) {
		playerSpeed = document["player_movement_speed"].GetFloat();
	}
}