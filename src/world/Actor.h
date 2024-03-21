#pragma once

// std library
#include <optional>
#include <string>
#include <set>
#include <memory>

// my code
#include "../utils/ResourceManager.h"
#include "../utils/OptionalVec2.h"
#include "../utils/config/RenderingConfig.h"
#include "Component.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_render.h"

class Transform {
public:
	glm::vec2 pos = { 0.0, 0.0 };
	glm::vec2 scale = { 1.0, 1.0 };
	float rotationDegrees = 0.0;
};

class TextureImage {
public:
	SDL_Texture* image = nullptr;
	std::string name = "";
	glm::ivec2 size = glm::ivec2(0);
};

struct View {
	// the various images used
	TextureImage image;

	// the pivot offset, in pixels
	OptionalVec2 pivot;
};

class Actor {
public:
	std::string name = "";
	int id = 0;

	// the actor's transform (pos, scale, rotation)
	Transform transform;
	// the actor's view (imageFront, imageBack, and pivot)
	View view;
	// the actor's x and y velocity
	glm::vec2 velocity;

	// the actor's location in the render order
	int renderOrder = 0;

	lua_State* luaState;

	// maps component key to component
	std::map<std::string, Component> componentsByKey;
	std::map<std::string, std::set<std::shared_ptr<Component>, KeyComparator>> componentsByType;
	std::map<std::string, std::shared_ptr<Component>> componentsWithOnStart;
    
    Actor(lua_State* luaState) : luaState(luaState), velocity(0, 0) {}

	// for lua
	const std::string& getName() const {
		return name;
	}

	// for lua
	int getID() const {
		return id;
	}

	// by key
	// returns nil if not found
	luabridge::LuaRef getComponentByKey(const std::string& key) {
		luabridge::LuaRef outRef = luabridge::LuaRef(luaState);

		if (componentsByKey.find(key) != componentsByKey.end()) {
			outRef = componentsByKey[key].instanceTable;
		}

		return outRef;
	}

	// by type
	// returns nil if not found
	luabridge::LuaRef getComponent(const std::string& type) {
		luabridge::LuaRef outRef = luabridge::LuaRef(luaState);

		auto it = componentsByType.find(type);
		if (it != componentsByType.end()) {
			std::shared_ptr comp = *(it->second.begin());
			outRef = comp->instanceTable;
		}

		return outRef;
	}

	// TABLE by type
	// returns empty table if not found
	luabridge::LuaRef getComponents(const std::string& type) {
		luabridge::LuaRef outRef = luabridge::newTable(luaState);

		auto it = componentsByType.find(type);
		if (it != componentsByType.end()) {
			int index = 1; // lua tables are 1-indexed :(

			// add each component to the table and increment the index
			for (const auto& component : it->second) {
				outRef[index] = component->instanceTable;
				++index;
			}
		}

		return outRef;
	}

	// load relevant view texture
	void loadTextures(ResourceManager& resourceManager);

	glm::vec2 getWorldPos(RenderingConfig& renderConfig, glm::vec2 pos);

	glm::vec2 getScreenPos(RenderingConfig& renderConfig, glm::vec2 cameraPos);
};

class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) const;
};

class RenderOrderComparator {
public:
	bool operator()(const Actor* actor1, const Actor* actor2) const;
};