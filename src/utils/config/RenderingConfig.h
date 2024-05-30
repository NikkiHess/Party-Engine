#pragma once

// depedencies
#include "glm/glm.hpp"
#include "rapidjson/document.h"

class RenderingConfig {
public:
	// The render size, as defined by rendering.config
	glm::ivec2 renderSize = glm::ivec2(640, 360);
	// the camera offset, as defined by rendering.config
	static inline glm::ivec3 clearColor = glm::ivec3(255, 255, 255);
	// the zoom factor of the camera
	float zoomFactor = 1;
	// the ease factor of the camera, for smoooooth movement
	float easeFactor = 1;

	// actor rendering
	bool actorFlipping = false; // decides whether the actor flips when moving west, false by default

	// not configurable, messy if not 100
	float pixelsPerUnit = 100;

	// parses information from rendering.config
	void parse(rapidjson::Document& document);
};

