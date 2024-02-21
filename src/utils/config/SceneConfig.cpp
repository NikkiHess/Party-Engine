#include "SceneConfig.h"

void SceneConfig::parse(rapidjson::Document& document, ResourceManager& resourceManager, Scene& scene, std::string hpImage) {
	if (document.HasMember("actors")) {
		rapidjson::GenericArray docActors = document["actors"].GetArray();

		scene.actors.reserve(docActors.Size());
		scene.locToActors.reserve(docActors.Size());
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

			// if the player is defined
			if (actor.name == "player") {
				// if hpImage doesn't exist, error out
				if (hpImage == "") {
					Error::error("player actor requires an hp_image be defined");
				}
			}
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

	// handle view
	if (actorDocument.HasMember("view_image"))
		actor.view.imageFront.name = actorDocument["view_image"].GetString();
	if (actorDocument.HasMember("view_image_back"))
		actor.view.imageBack.name = actorDocument["view_image_back"].GetString();
	if (actorDocument.HasMember("view_pivot_offset_x")) {
		if (!actor.view.pivot.x.has_value())
			actor.view.pivot.x = std::make_optional<float>();

		actor.view.pivot.x = actorDocument["view_pivot_offset_x"].GetFloat();
	}
	if (actorDocument.HasMember("view_pivot_offset_y")) {
		if (!actor.view.pivot.y.has_value())
			actor.view.pivot.y = std::make_optional<float>();

		actor.view.pivot.y = actorDocument["view_pivot_offset_y"].GetFloat();
	}

	// handle collisions
	// BOTH of these must be defined for collisions to happen
	if (actorDocument.HasMember("box_collider_width") && actorDocument.HasMember("box_collider_height")) {
		actor.boxCollider.size.x = actorDocument["box_collider_width"].GetFloat();
		actor.boxCollider.size.y = actorDocument["box_collider_height"].GetFloat();

		actor.boxCollider.canCollide = true;
	}

	// handle transform
	if (actorDocument.HasMember("transform_position_x"))
		actor.transform.pos.x = actorDocument["transform_position_x"].GetFloat();
	if (actorDocument.HasMember("transform_position_y"))
		actor.transform.pos.y = actorDocument["transform_position_y"].GetFloat();
	if (actorDocument.HasMember("transform_scale_x"))
		actor.transform.scale.x = actorDocument["transform_scale_x"].GetFloat();
	if (actorDocument.HasMember("transform_scale_y"))
		actor.transform.scale.y = actorDocument["transform_scale_y"].GetFloat();
	if (actorDocument.HasMember("transform_rotation_degrees"))
		actor.transform.rotationDegrees = actorDocument["transform_rotation_degrees"].GetFloat();

	// handle motion
	if (actorDocument.HasMember("vel_x"))
		actor.velocity.x = actorDocument["vel_x"].GetFloat();
	if (actorDocument.HasMember("vel_y"))
		actor.velocity.y = actorDocument["vel_y"].GetFloat();
	if (actorDocument.HasMember("movement_bounce_enabled")) {
		actor.movementBounce = actorDocument["movement_bounce_enabled"].GetBool();
	}

	// handle dialogue
	if (actorDocument.HasMember("nearby_dialogue"))
		actor.nearbyDialogue = actorDocument["nearby_dialogue"].GetString();
	if (actorDocument.HasMember("contact_dialogue"))
		actor.contactDialogue = actorDocument["contact_dialogue"].GetString();

	// handle rendering
	if (actorDocument.HasMember("render_order"))
		actor.renderOrder = actorDocument["render_order"].GetInt();
}
