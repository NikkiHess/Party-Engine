#pragma once

// std library
#include <string>

// my code
#include "../../world/Scene.h"
#include "../ResourceManager.h"

// dependencies
#include "rapidjson/document.h"

class SceneConfig {
public:
	// The initial scene from game.config
	Scene initialScene;

	SceneConfig() {}

	// initializes the scene from its scene file
	// utilizes Scene class
	// called at the start of the engine and when we proceed to a new scene
	void parse(rapidjson::Document& document, ResourceManager& resourceManager, Scene& scene, const std::string& sceneName);

	// set Actor props from a document
	// actor - the actor to set properties for
	// actorDocument - the rapidjson data we're using to set the actor's props
	// resourceManager - the resource manager shared throughout the engine, to check if files exist
	void setActorProps(Actor& actor, rapidjson::Value& actorDocument, ResourceManager& resourceManager);
};

