#include "ConfigHelper.h"

// std stuff
#include <string>
#include <iostream>
#include <filesystem>

// my code
#include "../gamedata/Scene.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

bool ConfigHelper::checkFile(const std::string& path) {
	if (!std::filesystem::exists(path)) {
		std::cout << "error: " + path + " missing";
		exit(0);
	}
}

void ConfigHelper::initializeMessages(rapidjson::Document& document) {
	if (document.HasMember("game_start_message"))
		gameStartMessage = document["game_start_message"].GetString();
	if (document.HasMember("game_over_bad_message"))
		gameOverBadMessage = document["game_over_bad_message"].GetString();
	if (document.HasMember("game_over_good_message"))
		gameOverGoodMessage = document["game_over_good_message"].GetString();
}

void ConfigHelper::initializeRendering(rapidjson::Document& document) {
	if (document.HasMember("x_resolution"))
		renderSize.x = document["x_resolution"].GetInt();
	if (document.HasMember("y_resolution"))
		renderSize.y = document["y_resolution"].GetInt();

	// we've been told in the spec we can assume it will always be odd, but...
	if (renderSize.x % 2 == 0 || renderSize.y % 2 == 0) {
		std::cout << "error: render size must be odd.";
		exit(0); // should this be a different exit code?
	}
}

void ConfigHelper::initializeScene(std::string& resources, rapidjson::Document& document) {
	// get the name of the initial scene
	if (!document.HasMember("initial_scene")) {
		std::cout << "error: initial_scene unspecified";
		exit(0);
	}
	initialScene.name = document["initial_scene"].GetString();

	// get the path of the initial scene and read its file
	std::string initialScenePath = resources + "scenes/" + initialScene.name + ".scene";
	if (!std::filesystem::exists(initialScenePath)) {
		std::cout << "error: scene " + initialScene.name + " is missing";
		exit(0);
	}
	readJsonFile(initialScenePath, document);

	// TODO: Make this not do if else, do like above
	if (document.HasMember("actors")) {
		rapidjson::GenericArray docActors = document["actors"].GetArray();

		for (unsigned int i = 0; i < docActors.Size(); ++i) {
			std::string name = "";
			char view = '?';
			int x = 0, y = 0, velX = 0, velY = 0;
			bool blocking = false;
			std::string nearbyDialogue = "", contactDialogue = "";

			if (docActors[i].HasMember("name"))
				name = docActors[i]["name"].GetString();
			if (docActors[i].HasMember("view"))
				view = docActors[i]["view"].GetString()[0];
			if (docActors[i].HasMember("x"))
				x = docActors[i]["x"].GetInt();
			if (docActors[i].HasMember("y"))
				y = docActors[i]["y"].GetInt();
			if (docActors[i].HasMember("vel_x"))
				velX = docActors[i]["vel_x"].GetInt();
			if (docActors[i].HasMember("vel_y"))
				velY = docActors[i]["vel_y"].GetInt();
			if (docActors[i].HasMember("blocking"))
				blocking = docActors[i]["blocking"].GetBool();
			if (docActors[i].HasMember("nearby_dialogue"))
				nearbyDialogue = docActors[i]["nearby_dialogue"].GetString();
			if (docActors[i].HasMember("contact_dialogue"))
				contactDialogue = docActors[i]["contact_dialogue"].GetString();

			initialScene.createActor(name, view, x, y, velX, velY, blocking, nearbyDialogue, contactDialogue);
		}
	}
	else {
		std::cout << "error: \"actors\" is missing from " + initialScene.name;
		exit(0);
	}
}