#include "ConfigUtils.h"

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
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"

bool ConfigUtils::fileExists(const std::string& path) {
	// checks whether the cache contains an entry
	if (fileExistsCache.count(path))
		return fileExistsCache[path];
	// if the cache doesn't contain an entry, create it and return it
	bool exists = std::filesystem::exists(path);
	fileExistsCache[path] = exists;
	return exists;
}

void ConfigUtils::checkFile(const std::string& path, std::optional<std::string> print) {
	if (!fileExists(path)) {
		if (print.has_value())
			std::cout << "error: " + print.value() + " missing";
		else
			std::cout << "error: " + path + " missing";
		exit(0);
	}
}

void ConfigUtils::initializeGame(rapidjson::Document& document) {
	// handle game title
	if (document.HasMember("game_title"))
		gameTitle = document["game_title"].GetString();

	// handle messages
	if (document.HasMember("font")) {
		std::string fontName = document["font"].GetString();
		std::string fontPath = "resources/fonts/" + fontName + ".ttf";

		checkFile(fontPath, "font " + fontName);

		// open the font specified
		font = TTF_OpenFont(fontPath.c_str(), 16);
	}

	// handle the intro
	if (document.HasMember("intro_image")) {
		rapidjson::GenericArray images = document["intro_image"].GetArray();
		introImages.reserve(images.Size());

		// Copy the images over one by one, checking each along the way
		for (rapidjson::Value& image : images) {
			std::string imgString = image.GetString();
			if (!fileExists("resources/images/" + imgString + ".png")) {
				std::cout << "error: missing image " + imgString;
				exit(0);
			}
			
			introImages.emplace_back(image.GetString());
		}
	}
	if (document.HasMember("intro_text")) {
		if (font == nullptr) {
			std::cout << "error: text render failed. No font configured";
			exit(0);
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
}

void ConfigUtils::initializeRendering(rapidjson::Document& document) {
	if (document.HasMember("x_resolution"))
		renderSize.x = document["x_resolution"].GetInt();
	if (document.HasMember("y_resolution"))
		renderSize.y = document["y_resolution"].GetInt();

	if (document.HasMember("clear_color_r"))
		clearColor.r = document["clear_color_r"].GetInt();
	if (document.HasMember("clear_color_g"))
		clearColor.g = document["clear_color_g"].GetInt();
	if (document.HasMember("clear_color_b"))
		clearColor.b = document["clear_color_b"].GetInt();
}

void ConfigUtils::initializeScene(Scene &scene, rapidjson::Document& document, bool isInitialScene = false) {
	// get the name of the initial scene
	if (isInitialScene) {
		if (!document.HasMember("initial_scene")) {
			std::cout << "error: initial_scene unspecified";
			exit(0);
		}
		scene.name = document["initial_scene"].GetString();

		std::string scenePath = "resources/scenes/" + initialScene.name + ".scene";
		if (!fileExists(scenePath)) {
			std::cout << "error: scene " + initialScene.name + " is missing";
			exit(0);
		}
		readJsonFile(scenePath, document);
	}

	// get the path of the initial scene and read its file
	std::string scenePath = "resources/scenes/" + scene.name + ".scene";
	if (!fileExists(scenePath)) {
		std::cout << "error: scene " + scene.name + " is missing";
		exit(0);
	}
	readJsonFile(scenePath, document);

	if (document.HasMember("actors")) {
		rapidjson::GenericArray docActors = document["actors"].GetArray();

		scene.actors.reserve(docActors.Size());
		for (unsigned int i = 0; i < docActors.Size(); ++i) {
			ActorProps props;

			// initialize the ActorProps based on a template, if there is one
			if (docActors[i].HasMember("template")) {
				std::string templateName = docActors[i]["template"].GetString();
				std::string templatePath = "resources/actor_templates/" + templateName + ".template";
				checkFile(templatePath, "template " + templateName + " is");
				// HOPEFULLY this leaves docActors intact
				readJsonFile(templatePath, document);
				setActorProps(props, document);
			}

			// override any template properties redefined by the scene document
			setActorProps(props, docActors[i]);

			// instantiate a new actor based on these props
			scene.instantiateActor(props);
			//std::cout << &scene.actors.back() << "\n"; // print actor address
		}
	}
	else {
		std::cout << "error: \"actors\" is missing from " + scene.name;
		exit(0);
	}
}

void ConfigUtils::setActorProps(ActorProps& props, rapidjson::Value& document) {
	if (document.HasMember("name"))
		props.name = document["name"].GetString();

	if (document.HasMember("view_image"))
		props.view.imageName = document["view_image"].GetString();
	if (document.HasMember("view_pivot_offset_x"))
		props.view.pivotOffset->x = document["view_pivot_offset_x"].GetDouble();
	if (document.HasMember("view_pivot_offset_y"))
		props.view.pivotOffset->y = document["view_pivot_offset_y"].GetDouble();

	if (document.HasMember("transform_position_x"))
		props.transform.pos.x = document["transform_position_x"].GetDouble();
	if (document.HasMember("transform_position_y"))
		props.transform.pos.y = document["transform_position_y"].GetDouble();
	if (document.HasMember("transform_scale_x"))
		props.transform.scale.x = document["transform_scale_x"].GetDouble();
	if (document.HasMember("transform_scale_y"))
		props.transform.scale.y = document["transform_scale_y"].GetDouble();
	if (document.HasMember("transform_rotation_degrees"))
		props.transform.rotationDegrees = document["transform_rotation_degrees"].GetDouble();

	if (document.HasMember("vel_x"))
		props.velocity.x = document["vel_x"].GetInt();
	if (document.HasMember("vel_y"))
		props.velocity.y = document["vel_y"].GetInt();

	if (document.HasMember("blocking"))
		props.blocking = document["blocking"].GetBool();

	if (document.HasMember("nearby_dialogue"))
		props.nearbyDialogue = document["nearby_dialogue"].GetString();
	if (document.HasMember("contact_dialogue"))
		props.contactDialogue = document["contact_dialogue"].GetString();
}