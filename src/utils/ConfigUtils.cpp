// std library
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

// my code
#include "../gamedata/Scene.h"
#include "ConfigUtils.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"

bool ConfigUtils::fileExists(const std::string& path) {
	// checks whether the cache contains the entry
	if (fileExistsCache.count(path))
		return fileExistsCache[path];
	// if the cache doesn't contain an entry, create it and return it
	bool exists = std::filesystem::exists(path);
	fileExistsCache[path] = exists;
	return exists;
}

void ConfigUtils::initializeGame(rapidjson::Document& document) {
	// handle game title
	if (document.HasMember("game_title"))
		gameTitle = document["game_title"].GetString();

	// handle font
	if (document.HasMember("font")) {
		std::string fontName = document["font"].GetString();
		std::string fontPath = "resources/fonts/" + fontName + ".ttf";

		if (!fileExists(fontPath))
			Error::error("font " + fontName + " missing");

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
				Error::error("missing image " + imgString);
			}
			
			introImages.emplace_back(image.GetString());
		}
	}
	if (document.HasMember("intro_text")) {
		if (font == nullptr) {
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

		if(!fileExists(hpImagePath)) Error::error(hpImageName + " missing");

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
}

void ConfigUtils::initializeScene(Scene &scene, rapidjson::Document& document, bool isInitialScene = false) {
	// get the name of the initial scene
	if (isInitialScene) {
		if (!document.HasMember("initial_scene")) {
			Error::error("initial_scene unspecified");
		}
		scene.name = document["initial_scene"].GetString();

		std::string scenePath = "resources/scenes/" + initialScene.name + ".scene";
		if (!fileExists(scenePath)) {
			Error::error("scene " + initialScene.name + " is missing");
		}
		readJsonFile(scenePath, document);
	}

	// get the path of the initial scene and read its file
	std::string scenePath = "resources/scenes/" + scene.name + ".scene";
	if (!fileExists(scenePath)) {
		Error::error("scene " + scene.name + " is missing");
	}
	readJsonFile(scenePath, document);

	if (document.HasMember("actors")) {
		rapidjson::GenericArray docActors = document["actors"].GetArray();

		scene.actors.reserve(docActors.Size());
		scene.locToActors.reserve(docActors.Size());
		for (unsigned int i = 0; i < docActors.Size(); ++i) {
			Actor actor;

			// initialize the ActorProps based on a template, if there is one
			if (docActors[i].HasMember("template")) {
				std::string templateName = docActors[i]["template"].GetString();
				std::string templatePath = "resources/actor_templates/" + templateName + ".template";

				if (!fileExists(templatePath)) Error::error("template " + templateName + " is missing");

				readJsonFile(templatePath, document);
				setActorProps(actor, document);
			}

			// override any template properties redefined by the scene document
			setActorProps(actor, docActors[i]);

			// instantiate the actor in the scene
			scene.instantiateActor(actor);
			
			// if the player is defined
			if (actor.name == "player") {
				// if hpImage doesn't exist, error out
				if (hpImage == "") {
					Error::error("player actor requires an hp_image be defined");
				}
			}
		}
	}
	else {
		Error::error("\"actors\" is missing from " + scene.name);
	}
}

void ConfigUtils::initializeRendering(rapidjson::Document& document) {
	// handle camera
	if (document.HasMember("x_resolution"))
		renderSize.x = document["x_resolution"].GetInt();
	if (document.HasMember("y_resolution"))
		renderSize.y = document["y_resolution"].GetInt();

	// get the zoom factor first, so it can be applied to our camera offset
	if (document.HasMember("zoom_factor"))
		zoomFactor = document["zoom_factor"].GetDouble();
	if (document.HasMember("cam_offset_x"))
		cameraOffset.x = document["cam_offset_x"].GetDouble() * zoomFactor;
	if (document.HasMember("cam_offset_y"))
		cameraOffset.y = document["cam_offset_y"].GetDouble() * zoomFactor;

	// handle bg color
	if (document.HasMember("clear_color_r"))
		clearColor.r = document["clear_color_r"].GetInt();
	if (document.HasMember("clear_color_g"))
		clearColor.g = document["clear_color_g"].GetInt();
	if (document.HasMember("clear_color_b"))
		clearColor.b = document["clear_color_b"].GetInt();
}

void ConfigUtils::setActorProps(Actor& actor, rapidjson::Value& document) {
	if (document.HasMember("name"))
		actor.name = document["name"].GetString();

	if (document.HasMember("view_image"))
		actor.view.imageName = document["view_image"].GetString();
	if (document.HasMember("view_pivot_offset_x")) {
		if(!actor.view.pivotOffset.x.has_value())
			actor.view.pivotOffset.x = std::make_optional<double>();

		actor.view.pivotOffset.x = document["view_pivot_offset_x"].GetDouble();
	}
	if (document.HasMember("view_pivot_offset_y")) {
		if (!actor.view.pivotOffset.y.has_value())
			actor.view.pivotOffset.y = std::make_optional<double>();

		actor.view.pivotOffset.y = document["view_pivot_offset_y"].GetDouble();
	}

	if (document.HasMember("transform_position_x"))
		actor.transform.pos.x = document["transform_position_x"].GetDouble();
	if (document.HasMember("transform_position_y"))
		actor.transform.pos.y = document["transform_position_y"].GetDouble();
	if (document.HasMember("transform_scale_x"))
		actor.transform.scale.x = document["transform_scale_x"].GetDouble();
	if (document.HasMember("transform_scale_y"))
		actor.transform.scale.y = document["transform_scale_y"].GetDouble();
	if (document.HasMember("transform_rotation_degrees"))
		actor.transform.rotationDegrees = document["transform_rotation_degrees"].GetDouble();

	if (document.HasMember("vel_x"))
		actor.velocity.x = document["vel_x"].GetInt();
	if (document.HasMember("vel_y"))
		actor.velocity.y = document["vel_y"].GetInt();

	if (document.HasMember("blocking"))
		actor.blocking = document["blocking"].GetBool();

	if (document.HasMember("nearby_dialogue"))
		actor.nearbyDialogue = document["nearby_dialogue"].GetString();
	if (document.HasMember("contact_dialogue"))
		actor.contactDialogue = document["contact_dialogue"].GetString();

	if(document.HasMember("render_order"))
		actor.renderOrder = document["render_order"].GetInt();

	// if we're working with the player and want them to have speed, set it from config
	if (actor.name == "player") {
		if (document.HasMember("player_movement_speed")) {
			actor.speed = document["player_movement_speed"].GetFloat();
		}
	}
}
