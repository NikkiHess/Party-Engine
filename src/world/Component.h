#pragma once

// std library
#include <string>
#include <set>
#include <map>
#include <memory>

// my code
#include "../utils/LuaStateSaver.h"

// rapidjson
#include "rapidjson/document.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

// dependencies
#include "Helper.h"

class LuaScriptCache {
private:
	// path: script contents
	std::unordered_map<std::string, std::string> cache;
public:
	// loads a script file and saves its contents to the cache
	bool loadAndCache(const std::string& path);

	// gets a script, either from the cache or by loading it (via loadAndCache)
	const std::string* getScript(const std::string& path);
};

class Component {
public:
	static inline LuaScriptCache scriptCache;

	// store by component TYPE, not key
	// if you store by key, bad things happen since keys are only unique to actors, not globally
	static inline std::map<std::string, Component> components;

	std::string key;
	std::string type;

	bool onStartCalled = false;
	bool willBeRemoved = false;

	luabridge::LuaRef baseTable = nullptr;
	luabridge::LuaRef instanceTable = nullptr;

	Component() {}

	// constructs with:
	// key - from config
	// type - the .lua file to use
	Component(const std::string& key, const std::string& type) : key(key), type(type) {
		establishBaseTable();

		instanceTable = luabridge::getGlobal(LuaStateSaver::luaState, type.c_str());

		establishInheritance(instanceTable, baseTable);

		// set the instance's key to be referenced in scripts
		instanceTable["key"] = key;
		instanceTable["enabled"] = true;
	}

	// for copied components, we basically have to reconstruct again
	Component& operator=(const Component& other) {
		type = other.type;

		establishBaseTable();

		instanceTable = luabridge::getGlobal(LuaStateSaver::luaState, type.c_str());

		establishInheritance(instanceTable, baseTable);

		// transfer the enabled property from the template
		bool enabled = other.instanceTable["enabled"];
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

	void establishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& parentTable);
private:
	void establishBaseTable();
};

class KeyComparator {
public:
	bool operator()(const std::shared_ptr<Component> component1, const std::shared_ptr<Component> component2) const {
		return component1->key < component2->key;
	}
};
