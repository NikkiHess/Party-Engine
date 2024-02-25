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

class SceneConfig {
public:
	// The initial scene from game.config
	Scene initialScene;
	// Actor templates from the scene
	std::vector<Actor*> templates;

	// initializes the scene from its scene file
	// utilizes Scene class
	// called at the start of the game and when we proceed to a new scene
	void parse(rapidjson::Document& document, ResourceManager& resourceManager, Scene& scene, std::string hpImage);

	// set Actor props from a document
	void setActorProps(Actor& actor, rapidjson::Value& actorDocument);
};

