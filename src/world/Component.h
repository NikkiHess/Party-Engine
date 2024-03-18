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

	luabridge::LuaRef* baseTable = nullptr;

	// construct with our name and lua_State
	Component(std::string name, lua_State* luaState) : name(name), luaState(luaState), baseTable(establishBaseTable()) {
		establishInheritance(baseTable);
	}

	luabridge::LuaRef establishBaseTable();

	void establishInheritance(luabridge::LuaRef* instanceTable);
};