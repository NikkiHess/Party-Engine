#pragma once

// std stuff
#include <string>

// my code
#include "../../errors/Error.h"
#include "../ResourceManager.h"
#include "GameConfig.h"
#include "JsonUtils.h"
#include "RenderingConfig.h"
#include "SceneConfig.h"

// dependencies
#include "rapidjson/document.h"

class ConfigManager {
public:
	GameConfig gameConfig;
	SceneConfig sceneConfig;
	RenderingConfig renderingConfig;
	ResourceManager& resourceManager;

	// the rapidjson Document to be used for reading in values
	rapidjson::Document document = nullptr;

	// initializes the config helper by verifying the resources directory as well as the game.config
	// reads the json from the given file and then loads the information into member variables
	ConfigManager(ResourceManager& resourceManager) : resourceManager(resourceManager) {
		if (!resourceManager.fileExists("resources/")) {
			Error::error("resources/ missing");
		}

		// handle rendering.config, which may or may not exist
		if (resourceManager.fileExists("resources/rendering.config")) {
			JsonUtils::readJsonFile("resources/rendering.config", document);
			renderingConfig.parse(document);
		}

		// handle game.config
		if (!resourceManager.fileExists("resources/game.config")) {
			Error::error("resources/game.config missing");
		}
		JsonUtils::readJsonFile("resources/game.config", document);
		const std::string& sceneName = gameConfig.parse(document, resourceManager);

		JsonUtils::readJsonFile("resources/scenes/" + sceneName + ".scene", document);
		sceneConfig.parse(document, resourceManager, sceneConfig.initialScene, sceneName);

	}
};
