#pragma once

// std stuff
#include <string>
#include <iostream>
#include <filesystem>
#include <vector>
#include <optional>

// my code
#include "../gamedata/Scene.h"
#include "../gamedata/Actor.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "glm/glm.hpp"

class ConfigUtils {
public:
	rapidjson::Document document = nullptr;

	std::string gameStartMessage = "";
	std::string gameOverBadMessage, gameOverGoodMessage = "";

	Scene initialScene;
	std::vector<Actor*> templates;

	glm::ivec2 renderSize;

	std::unordered_map<std::string, bool> fileExistsCache;

	// initializes the config helper by verifying the resources directory as well as the game.config
	// reads the json from the given file and then loads the information into member variables
	ConfigUtils() : renderSize(13, 9) {
		std::string resources = "resources/";
		std::string gameConfig = resources + "game.config";
		std::string renderingConfig = resources + "rendering.config";

		checkFile(resources);

		checkFile(gameConfig);
		readJsonFile(resources + "game.config", document);
		initializeMessages(document);
		initializeScene(initialScene, document, true);

		if (fileExists(renderingConfig)) {
			readJsonFile(resources + "rendering.config", document);
			initializeRendering(document);
		}
	}

	bool fileExists(const std::string& path);

	// checks that a file exists, and if not prints an error message and exits with code 1
	void checkFile(const std::string& path, std::optional<std::string> print = std::nullopt);

	// initializes the scene from its scene file
	// utilizes Scene class
	void initializeScene(Scene& scene, rapidjson::Document& document, bool isInitialScene);
private:
	// initializes the messages loaded in from the resources/game.config file
	void initializeMessages(rapidjson::Document& document);

	// initializes the render_size loaded in from the resources/rendering.config file
	void initializeRendering(rapidjson::Document& document);

	// set Actor props from a document
	void setActorProps(ActorProps& props, rapidjson::Value& document);;

	// reads a json file from path and puts it in the out_document
	static void readJsonFile(const std::string& path, rapidjson::Document& outDocument) {
		FILE* filePointer = nullptr;
#ifdef _WIN32
		fopen_s(&filePointer, path.c_str(), "rb");
#else
		filePointer = fopen(path.c_str(), "rb");
#endif
		char buffer[65536];
		rapidjson::FileReadStream stream(filePointer, buffer, sizeof(buffer));
		outDocument.ParseStream(stream);
		std::fclose(filePointer);

		if (outDocument.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = outDocument.GetParseError();
			std::cout << "error parsing json at [" << path << "]" << std::endl;
			exit(0);
		}
	}
};