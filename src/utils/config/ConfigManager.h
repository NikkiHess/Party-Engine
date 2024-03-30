#pragma once

// std stuff
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// my code
#include "../../errors/Error.h"
#include "../../world/Actor.h"
#include "../../world/Scene.h"
#include "GameConfig.h"
#include "SceneConfig.h"
#include "RenderingConfig.h"
#include "../ResourceManager.h"
#include "JsonUtils.h"
#include "../LuaUtils.h"

// dependencies
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

// lua
#include "lua/lua.h"

class ConfigManager {
public:
	GameConfig gameConfig;
	SceneConfig sceneConfig;
	RenderingConfig renderingConfig;
	ResourceManager& resourceManager;

	// the rapidjson Document to be used for reading in values
	rapidjson::Document document = nullptr;

	ConfigManager() : resourceManager(resourceManager), sceneConfig(LuaUtils::luaState) {}

	// initializes the config helper by verifying the resources directory as well as the game.config
	// reads the json from the given file and then loads the information into member variables
	ConfigManager(ResourceManager& resourceManager, lua_State* luaState) : resourceManager(resourceManager), sceneConfig(luaState) {
		if (!resourceManager.fileExists("resources/")) {
			Error::error("resources/ missing");
		}

		// handle game.config
		if (!resourceManager.fileExists("resources/game.config")) {
			Error::error("resources/game.config missing");
		}
		JsonUtils::readJsonFile("resources/game.config", document);
		const std::string& sceneName = gameConfig.parse(document, resourceManager);

		JsonUtils::readJsonFile("resources/scenes/" + sceneName + ".scene", document);
		sceneConfig.parse(document, resourceManager, sceneConfig.initialScene, sceneName);

		// handle rendering.config, which may or may not exist
		if (resourceManager.fileExists("resources/rendering.config")) {
			JsonUtils::readJsonFile("resources/rendering.config", document);
			renderingConfig.parse(document);
		}
	}
};