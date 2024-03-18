#include "Component.h"

// my code
#include "../errors/Error.h"

luabridge::LuaRef Component::establishBaseTable() {
	const std::string& path = "resources/component_types/" + name;

	// load the Lua script
	int result = luaL_dofile(luaState, path.c_str());

	// if there was an error, end the program with the error message
	if (result != LUA_OK) {
		const std::string& errorMessage = lua_tostring(luaState, -1);

		Error::error("Couldn't load Lua script - " + errorMessage);
	}

	// retrieve global base table
	return luabridge::getGlobal(luaState, "base_table");
}

void Component::establishInheritance(luabridge::LuaRef* instanceTable) {
	if (!baseTable) {
		Error::error("please establish base table before inheritance for component " + name);
	}

	// create a new component instance
	luabridge::LuaRef newMetaTable = luabridge::newTable(luaState);
	newMetaTable["__index"] = baseTable;

	// use the raw lua api to set the metatable
	instanceTable->push(luaState);
	newMetaTable.push(luaState);
	lua_setmetatable(luaState, -2);
	lua_pop(luaState, 1);
}