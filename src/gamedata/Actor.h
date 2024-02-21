#pragma once

// std library
#include <optional>
#include <string>

// my code
#include "../utils/ResourceManager.h"
#include "../utils/OptionalVec2.h"
#include "../utils/config/RenderingConfig.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_render.h"

class Transform {
public:
	glm::vec2 pos = { 0.0, 0.0 };
	glm::vec2 scale = { 1.0, 1.0 };
	float rotationDegrees = 0.0;

	// whether the actor is currently flipped (attempting move west)
	bool flipped = false;
	bool showBack = false;
	bool bounce = false;
};

class TextureImage {
public:
	SDL_Texture* image = nullptr;
	std::string name = "";
	glm::ivec2 size = glm::ivec2(0);
};

struct View {
	TextureImage imageFront;
	TextureImage imageBack;
	// the pivot offset, in pixels
	OptionalVec2 pivot;
};

class Actor {
public:
	// the actor's name
	std::string name = "";
	// the actor's transform
	Transform transform;
	// the actor's view (imageFront, imageBack, and pivot)
	View view;
	// the actor's x and y velocity
	glm::vec2 velocity;
	bool movementBounce = false;

	// the actor's dialogue
	std::string nearbyDialogue = "", contactDialogue = "";

	// the actor's location in the render order
	int renderOrder = 0;

	// the actor's collider properties
	//BoxCollider boxCollider;

	std::optional<SDL_FRect> boxCollider;
	bool boxColliderCalc = false;

	// PLAYER ONLY PROPERTIES!!!
	// should be unused otherwise
	int health = 3, score = 0;
	int lastHealthDownFrame = -180;
	int healthDownCooldown = 180;
	float speed = 0.02f;

	int id = 0;
	bool triggeredScoreUp = false;
    
    Actor() : velocity(0, 0) {}

	void handleFlipping(bool flipping);
	void handleVerticalFacing();

	void loadTextures(ResourceManager& resourceManager);

	glm::vec2 getWorldPos(RenderingConfig& renderConfig, glm::vec2 pos);

	glm::vec2 getScreenPos(RenderingConfig& renderConfig, glm::vec2 cameraPos) {
		glm::vec2 worldPos = getWorldPos(renderConfig, transform.pos);
		// camera center in pixel coordinates
		glm::vec2 cameraCenter(
			(renderConfig.renderSize.x / 2 - renderConfig.cameraOffset.x * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor,
			(renderConfig.renderSize.y / 2 - renderConfig.cameraOffset.y * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor
		);

		// actor position relative to the camera
		glm::vec2 actorCameraRelativePos = worldPos - glm::vec2(std::round(cameraPos.x), std::round(cameraPos.y));

		// actor screen position, accounting for rendering at screen center
		return cameraCenter + actorCameraRelativePos;
	}

	void calculateBoxCollider(RenderingConfig& renderConfig, glm::vec2 screenPos, glm::vec2 pivot) {
		if (!boxColliderCalc) {
			// pixel w and h
			boxCollider->w *= renderConfig.pixelsPerUnit;
			boxCollider->h *= renderConfig.pixelsPerUnit;
		}

		boxCollider->x = screenPos.x - boxCollider->w / 2.0f + pivot.x;
		boxCollider->y = screenPos.y - boxCollider->h / 2.0f + pivot.y;

		boxColliderCalc = true;
	}
};

class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) const;
};

class RenderOrderComparator {
public:
	bool operator()(const Actor* actor1, const Actor* actor2) const;
};
