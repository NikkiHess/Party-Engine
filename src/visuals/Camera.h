#pragma once

// my code
#include "../utils/config/ConfigManager.h"
#include "../world/Actor.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL.h"

class Camera {
public:
	static inline glm::vec2 pos = { 0, 0 };
	static inline RenderingConfig* renderConfig;

	static void setPosition(const float x, const float y);

	static void setZoom(const float zoomFactor);

	static float getPositionX();
	static float getPositionY();
	static float getZoom();
};
