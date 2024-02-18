// std library
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>

// my code
#include "../gamedata/Scene.h"
#include "ConfigManager.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"

bool ConfigManager::fileExists(const std::string& path) {
	// checks whether the cache contains the entry
	if (fileExistsCache.count(path))
		return fileExistsCache[path];
	// if the cache doesn't contain an entry, create it and return it
	bool exists = std::filesystem::exists(path);
	fileExistsCache[path] = exists;
	return exists;
}

// initializes from game.config
void ConfigManager::initializeGame(rapidjson::Document& gameDocument) {
	// handle game title
	if (gameDocument.HasMember("game_title"))
		gameTitle = gameDocument["game_title"].GetString();

	// handle font
	if (gameDocument.HasMember("font")) {
		std::string fontName = gameDocument["font"].GetString();
		std::string fontPath = "resources/fonts/" + fontName + ".ttf";

		if (!fileExists(fontPath))
			Error::error("font " + fontName + " missing");

		// open the font specified
		font = TTF_OpenFont(fontPath.c_str(), 16);
	}

	// handle the intro
	if (gameDocument.HasMember("intro_image")) {
		rapidjson::GenericArray images = gameDocument["intro_image"].GetArray();
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
	if (gameDocument.HasMember("intro_text")) {
		if (font == nullptr) {
			Error::error("text render failed. No font configured");
		}
		rapidjson::GenericArray texts = gameDocument["intro_text"].GetArray();
		introText.reserve(texts.Size());
		
		for (rapidjson::Value& text : texts) {
			introText.emplace_back(text.GetString());
		}
	}
	if (gameDocument.HasMember("intro_bgm")) {
		introMusic = gameDocument["intro_bgm"].GetString();
	}

	// handle the gameplay
	if (gameDocument.HasMember("gameplay_audio")) {
		gameplayMusic = gameDocument["gameplay_audio"].GetString();
	}

	// HUD
	if (gameDocument.HasMember("hp_image")) {
		std::string hpImageName = gameDocument["hp_image"].GetString();
		std::string hpImagePath = "resources/images/" + hpImageName + ".png";

		if(!fileExists(hpImagePath)) Error::error(hpImageName + " missing");

		hpImage = hpImageName;
	}

	// handle the game over sequence
	if (gameDocument.HasMember("game_over_bad_image")) {
		gameOverBadImage = gameDocument["game_over_bad_image"].GetString();
	}
	if (gameDocument.HasMember("game_over_bad_audio")) {
		gameOverBadAudio = gameDocument["game_over_bad_audio"].GetString();
	}
	if (gameDocument.HasMember("game_over_good_image")) {
		gameOverGoodImage = gameDocument["game_over_good_image"].GetString();
	}
	if (gameDocument.HasMember("game_over_good_audio")) {
		gameOverGoodAudio = gameDocument["game_over_good_audio"].GetString();
	}
	
	if (gameDocument.HasMember("player_movement_speed")) {
		playerSpeed = gameDocument["player_movement_speed"].GetFloat();
	}
}

// initializes a scene from a .scene file
void ConfigManager::initializeScene(Scene &scene, rapidjson::Document& sceneDocument, bool isInitialScene = false) {
	// get the name of the initial scene
	if (isInitialScene) {
		if (!sceneDocument.HasMember("initial_scene")) {
			Error::error("initial_scene unspecified");
		}
		scene.name = sceneDocument["initial_scene"].GetString();

		std::string scenePath = "resources/scenes/" + initialScene.name + ".scene";
		if (!fileExists(scenePath)) {
			Error::error("scene " + initialScene.name + " is missing");
		}
		readJsonFile(scenePath, sceneDocument);
	}

	// get the path of the initial scene and read its file
	std::string scenePath = "resources/scenes/" + scene.name + ".scene";
	if (!fileExists(scenePath)) {
		Error::error("scene " + scene.name + " is missing");
	}
	readJsonFile(scenePath, sceneDocument);

	if (sceneDocument.HasMember("actors")) {
		rapidjson::GenericArray docActors = sceneDocument["actors"].GetArray();

		scene.actors.reserve(docActors.Size());
		scene.locToActors.reserve(docActors.Size());
		for (unsigned int i = 0; i < docActors.Size(); ++i) {
			Actor actor;

			// initialize the ActorProps based on a template, if there is one
			if (docActors[i].HasMember("template")) {
				std::string templateName = docActors[i]["template"].GetString();
				std::string templatePath = "resources/actor_templates/" + templateName + ".template";

				if (!fileExists(templatePath)) Error::error("template " + templateName + " is missing");

				readJsonFile(templatePath, sceneDocument);
				setActorProps(actor, sceneDocument);
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

// initializes the rendering configuration
void ConfigManager::initializeRendering(rapidjson::Document& renderingDocument) {
	// handle camera
	if (renderingDocument.HasMember("x_resolution"))
		renderSize.x = renderingDocument["x_resolution"].GetInt();
	if (renderingDocument.HasMember("y_resolution"))
		renderSize.y = renderingDocument["y_resolution"].GetInt();

	// get the zoom factor first, so it can be applied to our camera offset
	if (renderingDocument.HasMember("zoom_factor"))
		zoomFactor = renderingDocument["zoom_factor"].GetFloat();
	if (renderingDocument.HasMember("cam_offset_x"))
		cameraOffset.x = renderingDocument["cam_offset_x"].GetFloat() * zoomFactor;
	if (renderingDocument.HasMember("cam_offset_y"))
		cameraOffset.y = renderingDocument["cam_offset_y"].GetFloat() * zoomFactor;

	// handle bg color
	if (renderingDocument.HasMember("clear_color_r"))
		clearColor.r = renderingDocument["clear_color_r"].GetInt();
	if (renderingDocument.HasMember("clear_color_g"))
		clearColor.g = renderingDocument["clear_color_g"].GetInt();
	if (renderingDocument.HasMember("clear_color_b"))
		clearColor.b = renderingDocument["clear_color_b"].GetInt();
}

// initializes an actor from its configuration
void ConfigManager::setActorProps(Actor& actor, rapidjson::Value& actorDocument) {
	if (actorDocument.HasMember("name"))
		actor.name = actorDocument["name"].GetString();

	if (actorDocument.HasMember("view_image"))
		actor.view.imageName = actorDocument["view_image"].GetString();
	if (actorDocument.HasMember("view_pivot_offset_x")) {
		if(!actor.view.pivotOffset.x.has_value())
			actor.view.pivotOffset.x = std::make_optional<float>();

		actor.view.pivotOffset.x = actorDocument["view_pivot_offset_x"].GetFloat();
	}
	if (actorDocument.HasMember("view_pivot_offset_y")) {
		if (!actor.view.pivotOffset.y.has_value())
			actor.view.pivotOffset.y = std::make_optional<float>();

		actor.view.pivotOffset.y = actorDocument["view_pivot_offset_y"].GetFloat();
	}

	if (actorDocument.HasMember("transform_position_x"))
		actor.transform.pos.x = actorDocument["transform_position_x"].GetFloat();
	if (actorDocument.HasMember("transform_position_y"))
		actor.transform.pos.y = actorDocument["transform_position_y"].GetFloat();
	if (actorDocument.HasMember("transform_scale_x"))
		actor.transform.scale.x = actorDocument["transform_scale_x"].GetFloat();
	if (actorDocument.HasMember("transform_scale_y"))
		actor.transform.scale.y = actorDocument["transform_scale_y"].GetFloat();
	if (actorDocument.HasMember("transform_rotation_degrees"))
		actor.transform.rotationDegrees = actorDocument["transform_rotation_degrees"].GetFloat();

	if (actorDocument.HasMember("vel_x"))
		actor.velocity.x = actorDocument["vel_x"].GetFloat();
	if (actorDocument.HasMember("vel_y"))
		actor.velocity.y = actorDocument["vel_y"].GetFloat();

	if (actorDocument.HasMember("blocking"))
		actor.blocking = actorDocument["blocking"].GetBool();

	if (actorDocument.HasMember("nearby_dialogue"))
		actor.nearbyDialogue = actorDocument["nearby_dialogue"].GetString();
	if (actorDocument.HasMember("contact_dialogue"))
		actor.contactDialogue = actorDocument["contact_dialogue"].GetString();

	if(actorDocument.HasMember("render_order"))
		actor.renderOrder = actorDocument["render_order"].GetInt();
}
