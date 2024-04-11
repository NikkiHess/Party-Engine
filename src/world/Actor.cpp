// std library
#include <cmath>
#include <memory>

// my code
#include "Actor.h"
#include "../utils/LuaUtils.h"
#include "../utils/LuaStateSaver.h"
#include "../errors/Error.h"
#include "../utils/Requests.h"

const std::string& Actor::getName() const {
	return name;
}

int Actor::getID() const {
	return id;
}

luabridge::LuaRef Actor::getComponentByKey(const std::string& key) {
	luabridge::LuaRef outRef = luabridge::LuaRef(LuaStateSaver::luaState);

	if (componentsByKey.find(key) != componentsByKey.end()) {
		std::shared_ptr comp = componentsByKey[key];
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

	LuaUtils::currentScene->actorsWithNewComponents.emplace(LuaUtils::currentScene->actorsById[this->id]);

	std::unique_ptr<CppComponent> cppComponent = Component::getCppComponent(type);
	
	Component component(key, type, cppComponent);
	std::shared_ptr compPtr = std::make_shared<Component>(component);

	componentsToAdd.emplace(compPtr);

	return compPtr->instanceTable;
}

void Actor::addComponent(std::shared_ptr<Component> compPtr, std::optional<rapidjson::Value*>& properties) {
	//// if the component is not cached already, we need to cache it
	//if (Component::components.find(type) == Component::components.end()) {
	//	// construct and cache the component
	//	Component::components[type] = Component(key, type);
	//}

	//// regardless, load it to the actor
	//Component component = Component::components[type];

	std::string& key = compPtr->key;
	std::string& type = compPtr->type;

	componentsByKey[key] = compPtr;

	// load properties from the config
	if (properties.has_value()) {
		componentsByKey[key]->loadProperties(*properties.value());
	}
	//// copy to its instance table if we don't have properties to load
	//else {
	//	componentsByKey[key].instanceTable = Component::components[type].instanceTable;
	//}

	// update the key to match from config
	componentsByKey[key]->key = key;
	componentsByKey[key]->instanceTable["key"] = key;
	componentsByKey[key]->instanceTable["actor"] = this;

	// put it in componentsByType
	componentsByType[type].emplace(componentsByKey[key]);

	updateLifecycleFunctions(componentsByKey[key]);
}

void Actor::updateLifecycleFunctions(const std::shared_ptr<Component> ptr) {
	std::string& key = ptr->key;

	try {
		// if we have OnStart, make sure the actor knows that
		if (!componentsByKey[key]->instanceTable["OnStart"].isNil()) {
			if(LuaUtils::currentScene != nullptr)
				LuaUtils::currentScene->actorsWithOnStart.emplace(LuaUtils::currentScene->actorsById[this->id]);
			componentsWithOnStart[key] = ptr;
			hasOnStart = true;
		}

		// if we have OnUpdate, make sure the actor knows that
		if (!componentsByKey[key]->instanceTable["OnUpdate"].isNil()) {
			if (LuaUtils::currentScene != nullptr)
				LuaUtils::currentScene->actorsWithOnUpdate.emplace(LuaUtils::currentScene->actorsById[this->id]);
			componentsWithOnUpdate[key] = ptr;
			hasOnUpdate = true;
		}
		// if we have OnLateUpdate, make sure the actor knows that
		if (!componentsByKey[key]->instanceTable["OnLateUpdate"].isNil()) {
			if (LuaUtils::currentScene != nullptr)
				LuaUtils::currentScene->actorsWithOnLateUpdate.emplace(LuaUtils::currentScene->actorsById[this->id]);
			componentsWithOnLateUpdate[key] = ptr;
			hasOnLateUpdate = true;
		}
	}
	catch(const luabridge::LuaException& e) {
		std::cerr << "LuaException caught: " << e.what() << std::endl;
	}
}

void Actor::requestRemoveComponent(const luabridge::LuaRef& componentRef) {
	std::string key = componentRef["key"];
	if (componentsByKey.find(key) != componentsByKey.end()) {
		std::shared_ptr<Component>& component = componentsByKey[key];
		component->instanceTable["enabled"] = false;
		component->willBeRemoved = true;
		component->instanceTable = luabridge::LuaRef(LuaStateSaver::luaState);

		componentsToRemove.emplace(componentsByKey[key]);
		componentsByKey[key]->willBeRemoved = true;
		LuaUtils::currentScene->actorsWithComponentsToRemove.emplace(LuaUtils::currentScene->actorsById[this->id]);
	}
}

void Actor::removeComponent(const std::shared_ptr<Component>& compPtr) {
	// remove Component
	componentsByKey.erase(compPtr->key);
	
	// remove Component pointers (there has to be a better way!)
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