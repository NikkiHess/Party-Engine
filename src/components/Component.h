#pragma once

// std library
#include <map>
#include <memory>
#include <string>

// my code
#include "../components/CppComponent.h"
#include "../utils/LuaStateSaver.h"
#include "Rigidbody.h"
#include "UIRenderer.h"

// rapidjson
#include "rapidjson/document.h"

class LuaScriptCache {
public:
	// loads a script file and saves its contents to the cache
	bool loadAndCache(const std::string& path);

	// gets a script, either from the cache or by loading it (via loadAndCache)
	const std::string* getScript(const std::string& path);
private:
	// path -> script contents
	std::unordered_map<std::string, std::string> cache;
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

	bool mouseEntered = false;
	bool mouseHovered = false;
	bool mouseExited = false;

	luabridge::LuaRef baseTable = nullptr;
	luabridge::LuaRef instanceTable = nullptr;

	Component() {}

	// key - from config
	// type - the .lua file to use
	Component(const std::string& key, const std::string& type, std::shared_ptr<CppComponent>& cppComponent) : key(key), type(type) {
		try {
			if (!cppComponent.get()) {
				establishBaseTable();

				instanceTable = luabridge::getGlobal(LuaStateSaver::luaState, type.c_str());

				establishInheritance(instanceTable, baseTable);
			}
			else {
				// push com
				if (type == "UIRenderer") {
					UIRenderer* rawPtr = dynamic_cast<UIRenderer*>(cppComponent.get());
					luabridge::push(LuaStateSaver::luaState, rawPtr);
				}
				else if (type == "Rigidbody") {
					Rigidbody* rawPtr = dynamic_cast<Rigidbody*>(cppComponent.get());
					luabridge::push(LuaStateSaver::luaState, rawPtr);
				}

				instanceTable = luabridge::LuaRef::fromStack(LuaStateSaver::luaState, -1);
				
				lua_pop(LuaStateSaver::luaState, 1);
			}

			// set the instance's key to be referenced in scripts
			instanceTable["key"] = key;
			instanceTable["enabled"] = true;
		}
		catch (luabridge::LuaException e) {
			std::cout << e.what() << "\n";
		}
	}

	// call a Lua function (OnStart, OnUpdate, OnLateUpate)
	// checking for errors along the way
	// name - the lua function to call
	// actorName - the actor that holds this component
	void callLuaFunction(const std::string& name, const std::string& actorName);

	// load the Component's properties, if there are any
	void loadProperties(rapidjson::Value& data);

	static std::shared_ptr<CppComponent> getCppComponent(const std::string& type) {
		std::shared_ptr<CppComponent> cppComponent = nullptr;
		if (type == "UIRenderer") {
			cppComponent = std::make_shared<UIRenderer>(UIRenderer("", {}, 0));
		}
		else if (type == "Rigidbody") {
			cppComponent = std::make_shared<Rigidbody>(Rigidbody());
		}
		return cppComponent;
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
