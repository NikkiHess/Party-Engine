// std library
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <fstream>
#include <streambuf>

// my code
#include "Component.h"
#include "../errors/Error.h"
#include "../utils/LuaUtils.h"
#include "../utils/LuaStateSaver.h"

// rapidjson
#include "rapidjson/document.h"

bool LuaScriptCache::loadAndCache(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		return false;
	}

	std::string content((std::istreambuf_iterator<char>(file)),
		std::istreambuf_iterator<char>());
	cache[path] = content;
	return true;
}

const std::string* LuaScriptCache::getScript(const std::string& path) {
	auto it = cache.find(path);
	if (it != cache.end()) {
		// cache hit, return the contents of the script
		return &it->second;
	} else {
		// cache miss, load the script
		if (loadAndCache(path)) {
			return &cache[path];
		}
	}
	// can't read? return nullptr
	return nullptr;
}

void Component::establishBaseTable() {
	const std::string& path = "resources/component_types/" + type + ".lua";

	// load the script from cache, to save on file reads
	const std::string* scriptContent = scriptCache.getScript(path);
	if (scriptContent && luaL_loadstring(LuaStateSaver::luaState, scriptContent->c_str()) == LUA_OK) {
		// execute the loaded Lua chunk
		if (lua_pcall(LuaStateSaver::luaState, 0, LUA_MULTRET, 0) != LUA_OK) {
			// TODO: return to Error::error once the semester is over
			// this is necessary for now
			std::cout << "problem with lua file " << type;
			exit(0);
		}
	} 
	else {
		// load the Lua script, verify no errors
		if (luaL_dofile(LuaStateSaver::luaState, path.c_str()) != LUA_OK) {
			// TODO: return to Error::error once the semester is over
			// this is necessary for now
			std::cout << "problem with lua file " << type;
			exit(0);
		}
	}

	// retrieve global base table
	baseTable = luabridge::getGlobal(LuaStateSaver::luaState, "base_table");
}

void Component::establishInheritance(luabridge::LuaRef& instanceTable, luabridge::LuaRef& parentTable) {
	// create a new component instance
	luabridge::LuaRef newMetaTable = luabridge::newTable(LuaStateSaver::luaState);
	newMetaTable["__index"] = parentTable;

	// use the raw lua api to set the metatable
	instanceTable.push(LuaStateSaver::luaState);
	newMetaTable.push(LuaStateSaver::luaState);
	lua_setmetatable(LuaStateSaver::luaState, -2);
	lua_pop(LuaStateSaver::luaState, 1);
}

void Component::callLuaFunction(const std::string& name, const std::string& actorName) {
	bool enabled = instanceTable["enabled"];
	// only call Lua functions if this component is enabled and not new this frame
	if (enabled) {
		// if this is OnStart and we've already called that, don't
		if (name == "OnStart" && onStartCalled) return;
		try {
			luabridge::LuaRef luaFunction = instanceTable[name];
			if (luaFunction.isFunction()) {
				luaFunction(instanceTable);
			}
		}
		catch (const luabridge::LuaException& error) {
			LuaUtils::printLuaException(error, actorName, type);
		}
		// if this is OnStart and we've made it this far, mark so we don't do it again
		if (name == "OnStart") onStartCalled = true;
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