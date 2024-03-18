#pragma once

// std library
#include <string>

// lua
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h"

class Component {
public:
	std::string name;
	lua_State* luaState;

	luabridge::LuaRef baseTable = nullptr;
	luabridge::LuaRef instanceTable = nullptr;

	Component() {}

	// construct with our name and lua_State
	Component(std::string name, lua_State* luaState) : name(name), luaState(luaState) {
		establishBaseTable();
		instanceTable = luabridge::getGlobal(luaState, name.c_str());
		establishInheritance(instanceTable, baseTable);
	}

	void onStart(luabridge::LuaRef& instanceTable);
private:
	void establishBaseTable();

	void establishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& parentTable);
};