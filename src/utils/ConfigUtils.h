#pragma once

// std stuff
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

// my code
#include "../errors/Error.h"
#include "../gamedata/Actor.h"
#include "../gamedata/Scene.h"

// dependencies
#include "glm/glm.hpp"
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_ttf.h"

class ConfigUtils {
public:
	// TODO: Separate these variables out into multiple classes, this is MESSY

	// the rapidjson Document to be used for reading in values
	rapidjson::Document document = nullptr;

	// relevant strings from game.config
	std::string gameTitle = "";
	std::vector<std::string> introImages;
	std::vector<std::string> introText;
	std::string introMusic = "";
	std::string gameplayMusic = "";
	std::string hpImage = "";

	// data cache
	std::unordered_map<std::string, SDL_Texture*> imageTextures, textTextures;
	std::unordered_map<std::string, Mix_Chunk*> sounds;
	TTF_Font* font = nullptr;
	std::unordered_map<std::string, bool> fileExistsCache;

	// The initial scene from game.config
	Scene initialScene;
	// Actor templates from the scene
	std::vector<Actor*> templates;

	// The render size, as defined by rendering.config
	glm::ivec2 renderSize;
	// the camera offset, as defined by rendering.config
	glm::dvec2 cameraOffset;
	glm::ivec3 clearColor;

	// initializes the config helper by verifying the resources directory as well as the game.config
	// reads the json from the given file and then loads the information into member variables
	ConfigUtils() : renderSize(640, 360), clearColor(255, 255, 255) {
		std::string resources = "resources/";
		std::string gameConfig = resources + "game.config";
		std::string renderingConfig = resources + "rendering.config";

		if (!fileExists(resources)) Error::error(resources + " missing");

		if (!fileExists(gameConfig)) Error::error(gameConfig + " missing");
		readJsonFile(resources + "game.config", document);
		initializeGame(document);
		initializeScene(initialScene, document, true);

		if (fileExists(renderingConfig)) {
			readJsonFile(resources + "rendering.config", document);
			initializeRendering(document);
		}
	}

	bool fileExists(const std::string& path);

	// checks that a file exists, and if not prints an error message and exits with code 1
	//void checkFile(const std::string& path, std::optional<std::string> print = std::nullopt);

	// initializes the scene from its scene file
	// utilizes Scene class
	void initializeScene(Scene& scene, rapidjson::Document& document, bool isInitialScene);
private:
	// initializes data from the resources/game.config file
	void initializeGame(rapidjson::Document& document);

	// initializes the render_size loaded in from the resources/rendering.config file
	void initializeRendering(rapidjson::Document& document);

	// set Actor props from a document
	void setActorProps(Actor& actor, rapidjson::Value& document);

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