#pragma once

// std library
#include <string>

// my code
#include "GameInfo.h"
#include "world/Actor.h"
#include "world/Scene.h"
#include "GameState.h"
#include "utils/config/ConfigManager.h"

// lua
#include "lua/lua.h"
#include "LuaBridge/LuaBridge.h"

void GameInfo::loadScene(const std::string& sceneName) {
	JsonUtils::readJsonFile("resources/scenes/" + sceneName + ".scene", configManager->document);

	Scene loadedScene;

	configManager->sceneConfig.parse(configManager->document, *resourceManager, loadedScene, sceneName);

	for (std::shared_ptr<Actor> actor : scene.dontDestroy) {
		loadedScene.actors.emplace_back(actor);
		loadedScene.actorsById[actor->id] = actor;
		loadedScene.actorsByName[actor->name].emplace(actor);
		loadedScene.actorsByRenderOrder.emplace(actor);

		if (actor->hasOnUpdate)
			loadedScene.actorsWithOnUpdate.emplace(actor);
		if (actor->hasOnLateUpdate)
			loadedScene.actorsWithOnLateUpdate.emplace(actor);
	}

	newScene = loadedScene;
}

const std::string& GameInfo::getCurrentScene() {
	return scene.name;
}

void GameInfo::dontDestroy(luabridge::LuaRef actorRef) {
	Actor actor = actorRef.cast<Actor>();
	std::shared_ptr<Actor> actorPtr = scene.actorsById[actor.id];

	actorPtr->dontDestroy = true;
	scene.dontDestroy.emplace_back(actorPtr);
}