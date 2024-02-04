#include "ConfigHelper.h"

// std stuff
#include <string>
#include <iostream>
#include <filesystem>
#include <optional>

// my code
#include "../gamedata/Scene.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

bool ConfigHelper::checkFile(const std::string& path, std::optional<std::string> print) {
	if (!std::filesystem::exists(path)) {
		if (print.has_value())
			std::cout << "error: " + print.value() + " missing";
		else
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
			ActorProps props;

			// initialize the ActorProps based on a template, if there is one
			if (docActors[i].HasMember("template")) {
				std::string templateName = docActors[i]["template"].GetString();
				std::string templatePath = resources + "actor_templates/" + templateName + ".template";
				checkFile(templatePath, "template " + templateName);
				// HOPEFULLY this leaves docActors intact
				readJsonFile(templatePath, document);
				setActorProps(props, document);
			}

			// override any template properties redefined by the scene document
			setActorProps(props, docActors[i]);

			// instantiate a new actor based on these props
			initialScene.instantiateActor(props);
		}
	}
	else {
		std::cout << "error: \"actors\" is missing from " + initialScene.name;
		exit(0);
	}
}

void ConfigHelper::setActorProps(ActorProps& props, rapidjson::Value& document) {
	if (document.HasMember("name"))
		props.name = document["name"].GetString();
	if (document.HasMember("view"))
		props.view = document["view"].GetString()[0];
	if (document.HasMember("x"))
		props.x = document["x"].GetInt();
	if (document.HasMember("y"))
		props.y = document["y"].GetInt();
	if (document.HasMember("vel_x"))
		props.velX = document["vel_x"].GetInt();
	if (document.HasMember("vel_y"))
		props.velY = document["vel_y"].GetInt();
	if (document.HasMember("blocking"))
		props.blocking = document["blocking"].GetBool();
	if (document.HasMember("nearby_dialogue"))
		props.nearbyDialogue = document["nearby_dialogue"].GetString();
	if (document.HasMember("contact_dialogue"))
		props.contactDialogue = document["contact_dialogue"].GetString();
}