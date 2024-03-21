#pragma once

// std library
#include <string>
#include <set>
#include <map>

// rapidjson
#include "rapidjson/document.h"

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Component {
public:
	// store by component TYPE, not key
	// if you store by key, bad things happen since keys are only unique to actors, not globally
	static std::map<std::string, Component> components;

	std::string key;
	std::string type;
	lua_State* luaState;

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
	}

	// for copied components, we basically have to reconstruct again
	Component& operator=(const Component& other) {
		type = other.type;
		key = other.key;
		luaState = other.luaState;

		establishBaseTable();

		instanceTable = luabridge::getGlobal(luaState, type.c_str());
		
		establishInheritance(instanceTable, baseTable);

		instanceTable["key"] = key;

		return *this;
	}

	// run the OnStart function from Lua, if there is one
	void onStart();

	// load the Components properties, if there are any
	void loadProperties(rapidjson::Value& data);

	static void log(const std::string& message) {
		std::cout << message << "\n";
	}

	static void logError(const std::string& message) {
		std::cerr << message << "\n";
	}
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
