#include "SceneConfig.h"

void SceneConfig::parse(rapidjson::Document& document, ResourceManager& resourceManager, Scene& scene, std::string hpImage) {
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
				setActorProps(actor, document);
			}

			// override any template properties redefined by the scene document
			setActorProps(actor, docActors[i]);

			// instantiate the actor in the scene
			scene.instantiateActor(actor);
		}
	}
	else {
		Error::error("\"actors\" is missing from " + scene.name);
	}
}

// initializes an actor from its configuration
void SceneConfig::setActorProps(Actor& actor, rapidjson::Value& actorDocument) {
	// handle name
	if (actorDocument.HasMember("name"))
		actor.name = actorDocument["name"].GetString();
}
