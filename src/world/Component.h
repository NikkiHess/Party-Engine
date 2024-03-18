#pragma once

// std library
#include <string>

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Component {
public:
	std::string key;
	std::string name;
	lua_State* luaState;

	luabridge::LuaRef baseTable = nullptr;
	luabridge::LuaRef instanceTable = nullptr;

	Component() {}

	// construct with our name and lua_State
	Component(const std::string& key, const std::string& name, lua_State* luaState) : key(key), name(name), luaState(luaState) {
		establishBaseTable();
		instanceTable = luabridge::getGlobal(luaState, name.c_str());
		establishInheritance(instanceTable, baseTable);

		// set the instance's key to be referenced in scripts
		instanceTable["key"] = key;
	}

	void onStart(luabridge::LuaRef& instanceTable);

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