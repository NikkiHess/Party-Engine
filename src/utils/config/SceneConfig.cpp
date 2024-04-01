// std library
#include <memory>
#include <optional>

// my code
#include "SceneConfig.h"
#include "../../world/Component.h"
#include "../LuaStateSaver.h"

// dependencies
#include "rapidjson/rapidjson.h"

void SceneConfig::parse(rapidjson::Document& document, ResourceManager& resourceManager, Scene& scene, const std::string& sceneName) {
	if (document.HasMember("actors")) {
		rapidjson::GenericArray docActors = document["actors"].GetArray();

		scene.name = sceneName;

		scene.actors.reserve(docActors.Size());
		for (unsigned int i = 0; i < docActors.Size(); ++i) {
			Actor actor;

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
			scene.instantiateActor(actor, true);
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

				std::optional<rapidjson::Value*> obj = std::make_optional<rapidjson::Value*>(&componentObject.value);
				actor.addComponent(type, key, obj);
			}
			// else, we need to update with new values
			else if (actor.componentsByKey.find(key) != actor.componentsByKey.end()) {
				// just load the properties, nothing else :)
				actor.componentsByKey[key].loadProperties(componentObject.value);
			}
		}
	}
}
