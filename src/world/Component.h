#pragma once

// std library
#include <string>
#include <set>
#include <map>
#include <memory>

// rapidjson
#include "rapidjson/document.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

// dependencies
#include "Helper.h"

class Component {
public:
	// store by component TYPE, not key
	// if you store by key, bad things happen since keys are only unique to actors, not globally
	static std::map<std::string, Component> components;

	std::string key;
	std::string type;
	lua_State* luaState;

	bool onStartCalled = false;

	luabridge::LuaRef baseTable = nullptr;
	luabridge::LuaRef instanceTable = nullptr;

	Component() {}

	// constructs with:
	// key - from config
	// type - the .lua file to use
	// luaState - the shared lua state throughout the engine
	Component(const std::string& key, const std::string& type, lua_State* luaState) : key(key), type(type), luaState(luaState) {
		establishBaseTable();

		instanceTable = luabridge::getGlobal(luaState, type.c_str());

		establishInheritance(instanceTable, baseTable);

		// set the instance's key to be referenced in scripts
		instanceTable["key"] = key;
		instanceTable["enabled"] = true;
	}

	// for copied components, we basically have to reconstruct again
	Component& operator=(const Component& other) {
		type = other.type;
		luaState = other.luaState;

		establishBaseTable();

		instanceTable = luabridge::getGlobal(luaState, type.c_str());

		// transfer the enabled property from the template
		bool enabled = other.instanceTable["enabled"].cast<bool>();
		instanceTable["enabled"] = enabled;
		
		return *this;
	}

	// call a Lua function (OnStart, OnUpdate, OnLateUpate)
	// checking for errors along the way
	// name - the lua function to call
	// actorName - the actor that holds this component
	void callLuaFunction(const std::string& name, const std::string& actorName);

	// load the Component's properties, if there are any
	void loadProperties(rapidjson::Value& data);

	// Copy properties from another component's ptr
	void copyProperties(std::shared_ptr<Component> componentPtr);
private:
	void establishBaseTable();

	void establishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& parentTable);
};

class KeyComparator {
public:
	bool operator()(const std::shared_ptr<Component> component1, const std::shared_ptr<Component> component2) const {
		return component1->key < component2->key;
	}
};
