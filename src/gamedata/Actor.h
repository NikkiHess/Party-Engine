#pragma once

// std library
#include <optional>
#include <string>
#include <unordered_set>

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
	bool bounce = false;
};

class TextureImage {
public:
	SDL_Texture* image = nullptr;
	std::string name = "";
	glm::ivec2 size = glm::ivec2(0);
};

struct View {
	// the various images used
	TextureImage imageFront;
	TextureImage imageBack;
	TextureImage imageDamage;
	TextureImage imageAttack;

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
	bool showBack = false;

	// the actor's dialogue
	std::string nearbyDialogue = "", contactDialogue = "";

	// the actor's location in the render order
	int renderOrder = 0;

	std::optional<SDL_FRect> boxCollider;
	bool boxColliderCalc = false;
	std::unordered_set<Actor*> collidingActorsThisFrame;

	std::optional<SDL_FRect> boxTrigger;
	bool boxTriggerCalc = false;
	std::unordered_set<Actor*> triggeringActorsThisFrame;

	int lastAttackFrame = -30;

	int id = 0;
	bool triggeredScoreUp = false;

	// PLAYER ONLY PROPERTIES!!!
	// should be unused otherwise
	int health = 3, score = 0;
	int lastHealthDownFrame = -180;
	int healthDownCooldown = 180;
	float speed = 0.02f;
	bool showDamage = false;
	bool showAttack = false;
    
    Actor() : velocity(0, 0) {}

	void handleFlipping(bool flipping);
	void handleVerticalFacing();

	void loadTextures(ResourceManager& resourceManager);

	glm::vec2 getWorldPos(RenderingConfig& renderConfig, glm::vec2 pos);

	glm::vec2 getScreenPos(RenderingConfig& renderConfig, glm::vec2 cameraPos);

	void calculateBoxCollider(RenderingConfig& renderConfig, glm::vec2 screenPos, glm::vec2 pivot);

	void calculateBoxTrigger(RenderingConfig& renderConfig, glm::vec2 screenPos, glm::vec2 pivot);
};

class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) const;
};

class RenderOrderComparator {
public:
	bool operator()(const Actor* actor1, const Actor* actor2) const;
};
