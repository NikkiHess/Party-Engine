// my code
#include "SceneConfig.h"
#include "../../world/Component.h"

void SceneConfig::parse(rapidjson::Document& document, ResourceManager& resourceManager, Scene& scene) {
	if (document.HasMember("actors")) {
		rapidjson::GenericArray docActors = document["actors"].GetArray();

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
	if (actorDocument.HasMember("name") && actorDocument["name"].IsString())
		actor.name = actorDocument["name"].GetString();

	// verify that we have components
	if (actorDocument.HasMember("components") && actorDocument["components"].IsObject()) {
		// loop over component strings
		// match component keys to component types
		for (auto& componentObject : actorDocument["components"].GetObject()) {
			// get the string name/key of the component
			const std::string& key = componentObject.name.GetString();
			if (componentObject.value.HasMember("type") && componentObject.value["type"].IsString()) {
				// get the type of the component
				const std::string& type = componentObject.value["type"].GetString();

				if (!resourceManager.fileExists("resources/component_types/" + type + ".lua")) {
					Error::error("failed to locate component " + type);
				}

				// if the component is not cached already, we need to cache it
				if(Component::components.find(key) == Component::components.end()) {
					// get the component and match the key to it
					Component component = Component(key, type, luaState);

					// cache our component
					Component::components[key] = component;
				}

				// regardless, load it to the actor
				actor.components[key] = &Component::components[key];
				// if we have OnStart, make sure the actor knows that
				if (!Component::components[key].instanceTable["OnStart"].isNil()) {
					actor.componentsWithOnStart[key] = &Component::components[key];
				}
			}
			Component* componentPtr = &Component::components[key];
			componentPtr->loadProperties(componentObject.value);
		}
	}
}
