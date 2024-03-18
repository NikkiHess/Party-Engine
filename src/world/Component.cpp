// std library
#include <iostream>

// my code
#include "Component.h"
#include "../errors/Error.h"

// rapidjson
#include "rapidjson/document.h"

// I hate this but I have to do it apparently
std::map<std::string, Component> Component::components;

void Component::establishBaseTable() {
	const std::string& path = "resources/component_types/" + name + ".lua";

	// load the Lua script, verify no errors
	if (luaL_dofile(luaState, path.c_str()) != LUA_OK) {
		const std::string& errorMessage = lua_tostring(luaState, -1);

		// TODO: return to Error::error once the semester is over
		// this is necessary for now
		std::cout << "problem with lua file " << name;
		exit(0);
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
		luabridge::LuaRef onStartFunction = instanceTable["OnStart"];
		if (onStartFunction.isFunction()) {
			onStartFunction(instanceTable);
		}
	}
	catch (const luabridge::LuaException& e) {
		Error::error(e.what());
	}
}

void Component::loadProperties(rapidjson::Value& properties) {
	if (properties.IsObject()) {
		// loop over properties
		for (auto& data : properties.GetObject()) {
			const std::string& name = data.name.GetString();

			// make sure we don't add "type" to our properties
			// we already used it to get the Lua file we needed
			if (name != "type") {
				if (data.value.IsString())
					instanceTable[name] = data.value.GetString();
				if (data.value.IsFloat())
					instanceTable[name] = data.value.GetFloat();
				if (data.value.IsInt())
					instanceTable[name] = data.value.GetInt();
				if (data.value.IsBool())
					instanceTable[name] = data.value.GetBool();
			}
		}
	}
}