// std library
#include <memory>

// my code
#include "SceneConfig.h"
#include "../../world/Component.h"

void SceneConfig::parse(rapidjson::Document& document, ResourceManager& resourceManager, Scene& scene) {
	if (document.HasMember("actors")) {
		rapidjson::GenericArray docActors = document["actors"].GetArray();

		scene.actors.reserve(docActors.Size());
		for (unsigned int i = 0; i < docActors.Size(); ++i) {
			Actor actor(luaState);

			// initialize the ActorProps based on a template, if there is one
			if (docActors[i].HasMember("template")) {
				std::string templateName = docActors[i]["template"].GetString();
				std::string templatePath = "resources/actor_templates/" + templateName + ".template";

				if (!resourceManager.fileExists(templatePath)) {
					Error::error("template " + templateName + " is missing");
				}

				JsonUtils::readJsonFile(templatePath, document);
				setActorProps(actor, document, resourceManager);
			}

			// override any template properties redefined by the scene document
			setActorProps(actor, docActors[i], resourceManager);

			// instantiate the actor in the scene
			scene.instantiateActor(actor);
		}
	}
	else {
		Error::error("\"actors\" is missing from " + scene.name);
	}
}

// initializes an actor from its configuration
void SceneConfig::setActorProps(Actor& actor, rapidjson::Value& actorDocument, ResourceManager& resourceManager) {
	// handle name
	if (actorDocument.HasMember("name") && actorDocument["name"].IsString()) {
		const std::string& name = actorDocument["name"].GetString();
		actor.name = name;
	}

	// verify that we have components
	if (actorDocument.HasMember("components") && actorDocument["components"].IsObject()) {
		// loop over component strings
		// match component keys to component types
		for (auto& componentObject : actorDocument["components"].GetObject()) {
			// get the string name/key of the component
			const std::string& key = componentObject.name.GetString();

			// if there is a type, we're not coming from a template
			if (componentObject.value.HasMember("type") && componentObject.value["type"].IsString()) {
				// get the type of the component
				const std::string& type = componentObject.value["type"].GetString();

				if (!resourceManager.fileExists("resources/component_types/" + type + ".lua")) {
					Error::error("failed to locate component " + type);
				}

				// if the component is not cached already, we need to cache it
				if(Component::components.find(type) == Component::components.end()) {
					// get the component and match the key to it
					Component component = Component(key, type, luaState);

					// cache our component
					Component::components[type] = component;
				}

				// regardless, load it to the actor
				Component component = Component::components[type];

				// make a copy from the component list
				actor.componentsByKey[key] = component;
				// update the key to match from config
				actor.componentsByKey[key].key = key;
				// load properties from the config
				actor.componentsByKey[key].loadProperties(componentObject.value);

				// get the address of the copy we made
				std::shared_ptr ptr = std::make_shared<Component>(actor.componentsByKey[key]);
				// put it in componentsByType
				actor.componentsByType[type].emplace(ptr);

				// if we have OnStart, make sure the actor knows that
				if (!actor.componentsByKey[key].instanceTable["OnStart"].isNil()) {
					actor.componentsWithOnStart[key] = ptr;
				}
			}
			// else, we need to update with new values
			else if (actor.componentsByKey.find(key) != actor.componentsByKey.end()) {
				// just load the properties, nothing else :)
				actor.componentsByKey[key].loadProperties(componentObject.value);
			}
		}
	}
}
