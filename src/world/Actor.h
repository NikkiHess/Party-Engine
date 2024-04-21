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
#include "../components/Component.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_render.h"

class Actor {
public:
	static inline int currentId = 0;

	std::string name = "";
	int id = 0;
	bool dontDestroy = false;
	// the actor's x and y velocity
	glm::vec2 velocity;

	// the actor's location in the render order
	int renderOrder = 0;

	// maps component key to component
	std::map<std::string, std::shared_ptr<Component>> componentsByKey;

	// maps component type to a list of components of that type
	std::map<std::string, std::set<std::shared_ptr<Component>, KeyComparator>> componentsByType;
	
	// component sorting maps, by key
	std::map<std::string, std::shared_ptr<Component>> componentsWithOnStart;
	std::map<std::string, std::shared_ptr<Component>> componentsToRemoveFromOnStart;

	std::map<std::string, std::shared_ptr<Component>> componentsWithOnUpdate;
	std::map<std::string, std::shared_ptr<Component>> componentsWithOnLateUpdate;
	std::map<std::string, std::shared_ptr<Component>> componentsWithOnExit;

	std::set<std::shared_ptr<Component>> componentsToAdd;
	std::set<std::shared_ptr<Component>> componentsToRemove;

	// whether we have onStart, onUpdate, or onLateUpdate functions to worry about
	bool hasOnStart = false, hasOnUpdate = false, hasOnLateUpdate = false, hasOnExit = false;
    
    Actor() : velocity(0, 0) {}

	// for lua
	const std::string& getName() const;

	// for lua
	int getID() const;

	// by key
	// returns nil if not found
	luabridge::LuaRef getComponentByKey(const std::string& key);

	// by type
	// returns nil if not found
	luabridge::LuaRef getComponent(const std::string& type);

	// TABLE by type
	// returns empty table if not found
	luabridge::LuaRef getComponents(const std::string& type);

	// requests adding a component to this actor by its type
	// key is calculated by r + # calls to addComponent
	luabridge::LuaRef requestAddComponent(const std::string& type);

	void addComponent(std::shared_ptr<Component> compPtr, std::optional<rapidjson::Value*>& properties);

	void updateLifecycleFunctions(const std::shared_ptr<Component>& ptr);

	// requests removal of a component by LuaRef (for Lua only)
	void requestRemoveComponent(const luabridge::LuaRef& componentRef);

	// actually remove the component
	void removeComponent(const std::shared_ptr<Component>& compPtr);

	glm::vec2 getWorldPos(RenderingConfig& renderConfig, glm::vec2 pos);

	glm::vec2 getScreenPos(RenderingConfig& renderConfig, glm::vec2 cameraPos);
};

class ActorComparator {
public:
	bool operator()(const std::shared_ptr<Actor> actor1, const std::shared_ptr<Actor> actor2) const;
};

class RenderOrderComparator {
public:
	bool operator()(const std::shared_ptr<Actor> actor1, const std::shared_ptr<Actor> actor2) const;
};