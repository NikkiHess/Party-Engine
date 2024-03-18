#include "Component.h"

// my code
#include "../errors/Error.h"

void Component::establishBaseTable() {
	const std::string& path = "resources/component_types/" + name + ".lua";

	// load the Lua script, verify no errors
	if (luaL_dofile(luaState, path.c_str()) != LUA_OK) {
		const std::string& errorMessage = lua_tostring(luaState, -1);

		Error::error("problem with lua file " + name);
	}

	// retrieve global base table
	baseTable = luabridge::getGlobal(luaState, "base_table");
}

void Component::establishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& parentTable) {
	// create a new component instance
	luabridge::LuaRef newMetaTable = luabridge::newTable(luaState);
	newMetaTable["__index"] = instanceTable;

	// use the raw lua api to set the metatable
	instanceTable.push(luaState);
	newMetaTable.push(luaState);
	lua_setmetatable(luaState, -2);
	lua_pop(luaState, 1);
}

void Component::onStart(luabridge::LuaRef& instanceTable) {
	try {
		instanceTable["OnStart"](instanceTable);
	}
	catch (luabridge::LuaException const& e) {
		Error::error(e.what());
	}
}