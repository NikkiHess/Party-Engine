#pragma once

// depedencies
#include "rapidjson/document.h"
#include "glm/glm.hpp"

class RenderingConfig {
public:
	// The render size, as defined by rendering.config
	glm::ivec2 renderSize = glm::ivec2(640, 360);
	// the camera offset, as defined by rendering.config
	glm::vec2 cameraOffset = glm::vec2(0);
	glm::ivec3 clearColor = glm::ivec3(255, 255, 255);
	// the zoom factor of the camera
	float zoomFactor = 1;
	// the ease factor of the camera, for smoooooth movement
	float easeFactor = 1;

	// not technically configurable yet
	float pixelsPerUnit = 100;

	// parses information from rendering.config
	void parse(rapidjson::Document& document);
};

