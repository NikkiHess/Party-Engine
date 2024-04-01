#pragma once

// std library
#include <string>
#include <vector>

// my code
#include "../../errors/Error.h"
#include "../ResourceManager.h"
#include "../../world/Scene.h"
#include "JsonUtils.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

// lua
#include "lua/lua.h"

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

