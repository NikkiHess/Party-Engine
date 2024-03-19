#pragma once

// std library
#include <optional>
#include <string>
#include <unordered_set>

// my code
#include "../utils/ResourceManager.h"
#include "../utils/OptionalVec2.h"
#include "../utils/config/RenderingConfig.h"
#include "Component.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL_render.h"

class Transform {
public:
	glm::vec2 pos = { 0.0, 0.0 };
	glm::vec2 scale = { 1.0, 1.0 };
	float rotationDegrees = 0.0;
};

class TextureImage {
public:
	SDL_Texture* image = nullptr;
	std::string name = "";
	glm::ivec2 size = glm::ivec2(0);
};

struct View {
	// the various images used
	TextureImage image;

	// the pivot offset, in pixels
	OptionalVec2 pivot;
};

class Actor {
public:
	std::string name = "";
	int id = 0;

	// the actor's transform (pos, scale, rotation)
	Transform transform;
	// the actor's view (imageFront, imageBack, and pivot)
	View view;
	// the actor's x and y velocity
	glm::vec2 velocity;

	// the actor's location in the render order
	int renderOrder = 0;

	// maps component key to component
	std::map<std::string, Component*> components;
	std::map<std::string, Component*> componentsWithOnStart;
    
    Actor() : velocity(0, 0) {}

	// for lua
	const std::string& getName() const {
		return name;
	}

	// for lua
	int getID() const {
		return id;
	}

	// load relevant view texture
	void loadTextures(ResourceManager& resourceManager);

	glm::vec2 getWorldPos(RenderingConfig& renderConfig, glm::vec2 pos);

	glm::vec2 getScreenPos(RenderingConfig& renderConfig, glm::vec2 cameraPos);
};

class ActorComparator {
public:
	bool operator()(Actor* actor1, Actor* actor2) const;
};

class RenderOrderComparator {
public:
	bool operator()(const Actor* actor1, const Actor* actor2) const;
};
