#pragma once

// lua
#include "lua/lua.hpp" 

class LuaStateSaver {
public:
	static inline lua_State* luaState = nullptr;
};