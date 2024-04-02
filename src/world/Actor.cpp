// std library
#include <cmath>
#include <memory>

// my code
#include "Actor.h"
#include "../utils/LuaUtils.h"
#include "../utils/LuaStateSaver.h"

const std::string& Actor::getName() const {
	return name;
}

int Actor::getID() const {
	return id;
}

luabridge::LuaRef Actor::getComponentByKey(const std::string& key) {
	luabridge::LuaRef outRef = luabridge::LuaRef(LuaStateSaver::luaState);

	if (componentsByKey.find(key) != componentsByKey.end()) {
		std::shared_ptr comp = componentPtrsByKey[key];
		// if the component isn't requested for removal, we can use it
		if (!comp->willBeRemoved) {
			outRef = comp->instanceTable;
		}
	}

	return outRef;
}

luabridge::LuaRef Actor::getComponent(const std::string& type) {
	luabridge::LuaRef outRef = luabridge::LuaRef(LuaStateSaver::luaState);

	auto it = componentsByType.find(type);
	if (it != componentsByType.end()) {
		std::shared_ptr component = *(it->second.begin());
		// if the component isn't requested for removal, we can use it
		if (!component->willBeRemoved) {
			outRef = component->instanceTable;
		}
	}

	return outRef;
}

luabridge::LuaRef Actor::getComponents(const std::string& type) {
	luabridge::LuaRef outRef = luabridge::newTable(LuaStateSaver::luaState);

	auto it = componentsByType.find(type);
	if (it != componentsByType.end()) {
		int index = 1; // lua tables are 1-indexed :(

		// add each component to the table and increment the index
		for (const auto& component : it->second) {
		// if the component isn't requested for removal, we can use it
			if (!component->willBeRemoved) {
				outRef[index] = component->instanceTable;
				++index;
			}
		}
	}

	return outRef;
}

// the actor here somehow isn't the same as the actor in the actor sets in our scene
luabridge::LuaRef Actor::requestAddComponent(const std::string& type) {
	// key is r + # of times addComponent has been called globally
	std::string key = "r" + std::to_string(LuaUtils::componentsAdded);
	++LuaUtils::componentsAdded;

	std::optional<rapidjson::Value*> properties = std::nullopt;

	std::shared_ptr<Component> ptr = createComponentWithoutProperties(type, key);

	ptr->key = key;
	ptr->instanceTable["actor"] = this;

	LuaUtils::currentScene->actorsWithNewComponents.emplace(LuaUtils::currentScene->actorsById[this->id]);
	componentsToAdd.emplace(ptr);

	return ptr->instanceTable;
}

std::shared_ptr<Component> Actor::createComponentWithoutProperties(const std::string& type, const std::string& key) {
	// if the component is not cached already, we need to cache it
	if (Component::components.find(type) == Component::components.end()) {
		// get the component and match the key to it
		Component component = Component(key, type);

		// cache our component
		Component::components[type] = component;
	}

	// regardless, load it to the actor
	Component component = Component::components[type];

	std::shared_ptr<Component> ptr;
	if(componentPtrsByKey.find(key) != componentPtrsByKey.end()) {
		ptr = componentPtrsByKey[key];
	}
	else {
		ptr = std::make_shared<Component>(component);
	}

	return ptr;
}

void Actor::addComponent(const std::string& type, const std::string& key, std::optional<rapidjson::Value*>& properties) {
	// if the component is not cached already, we need to cache it
	if (Component::components.find(type) == Component::components.end()) {
		// get the component and match the key to it
		Component component = Component(key, type);

		// cache our component
		Component::components[type] = component;
	}

	// regardless, load it to the actor
	componentsByKey[key].instanceTable = luabridge::newTable(LuaStateSaver::luaState);

	// make a copy from the component list
	componentsByKey[key] = Component::components[type];
	// copy to its instance table if we don't have properties to load
	if (!properties.has_value()) {
		componentsByKey[key].establishInheritance(componentsByKey[key].instanceTable, Component::components[type].instanceTable);
	}
	// update the key to match from config
	componentsByKey[key].key = key;
	componentsByKey[key].instanceTable["key"] = key;
	// load properties from the config
	if (properties.has_value()) {
		componentsByKey[key].loadProperties(*properties.value());
	}

	// get the address of the copy we made
	std::shared_ptr<Component> ptr = std::make_shared<Component>(componentsByKey[key]);
	// put it in componentsByType
	componentsByType[type].emplace(ptr);
	componentPtrsByKey[key] = ptr;

	updateLifecycleFunctions(ptr);
}

void Actor::updateLifecycleFunctions(const std::shared_ptr<Component> ptr) {
	std::string& key = ptr->key;

	// if we have OnStart, make sure the actor knows that
	if (!componentsByKey[key].instanceTable["OnStart"].isNil()) {
		if(LuaUtils::currentScene != nullptr)
			LuaUtils::currentScene->actorsWithOnStart.emplace(LuaUtils::currentScene->actorsById[this->id]);
		componentsWithOnStart[key] = ptr;
		hasOnStart = true;
	}

	// if we have OnUpdate, make sure the actor knows that
	if (!componentsByKey[key].instanceTable["OnUpdate"].isNil()) {
		if (LuaUtils::currentScene != nullptr)
			LuaUtils::currentScene->actorsWithOnUpdate.emplace(LuaUtils::currentScene->actorsById[this->id]);
		componentsWithOnUpdate[key] = ptr;
		hasOnUpdate = true;
	}

	// if we have OnLateUpdate, make sure the actor knows that
	if (!componentsByKey[key].instanceTable["OnLateUpdate"].isNil()) {
		if (LuaUtils::currentScene != nullptr)
			LuaUtils::currentScene->actorsWithOnLateUpdate.emplace(LuaUtils::currentScene->actorsById[this->id]);
		componentsWithOnLateUpdate[key] = ptr;
		hasOnLateUpdate = true;
	}
}

void Actor::requestRemoveComponent(const luabridge::LuaRef& componentRef) {
	std::string key = componentRef["key"];
	if (componentsByKey.find(key) != componentsByKey.end()) {
		Component& component = componentsByKey[key];
		component.instanceTable["enabled"] = false;
		component.willBeRemoved = true;
		component.instanceTable = luabridge::LuaRef(LuaStateSaver::luaState);

		componentsToRemove.emplace(componentPtrsByKey[key]);
		componentPtrsByKey[key]->willBeRemoved = true;
		LuaUtils::currentScene->actorsWithComponentsToRemove.emplace(LuaUtils::currentScene->actorsById[this->id]);
	}
}

void Actor::removeComponent(const std::shared_ptr<Component>& compPtr) {
	// remove Component
	componentsByKey.erase(compPtr->key);
	
	// remove Component pointers (there has to be a better way!)
	componentPtrsByKey.erase(compPtr->key);
	componentsByType.erase(compPtr->type);
	componentsWithOnStart.erase(compPtr->key);
	componentsWithOnUpdate.erase(compPtr->key);
	componentsWithOnLateUpdate.erase(compPtr->key);
}

bool ActorComparator::operator()(const std::shared_ptr<Actor> actor1, const std::shared_ptr<Actor> actor2) const {
	return actor1->id < actor2->id;
}

bool RenderOrderComparator::operator()(const std::shared_ptr<Actor> actor1, const std::shared_ptr<Actor>actor2) const {
	// render orders equal? render by y pos
	if (actor1->renderOrder == actor2->renderOrder) {
		// y pos equal? render by id
		if (actor1->transform.pos.y == actor2->transform.pos.y) {
			return actor1->id < actor2->id;
		}

		// y pos not equal? compare
		return actor1->transform.pos.y < actor2->transform.pos.y;
	}
	// render order not equal? compare
	return actor1->renderOrder < actor2->renderOrder;
}