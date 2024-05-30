#pragma once

// my code
#include "../utils/config/RenderingConfig.h"

// glm
#include "glm/glm.hpp"

class Camera {
public:
	static inline glm::vec2 pos = { 0, 0 };
	static inline RenderingConfig* renderConfig;

	// gets the camera's zoom factor
	static float getZoom();

	// sets the camera's zoom factor
	static void setZoom(const float zoomFactor);

	// gets the position x of the camera
	static float getPositionX();

	// gets the position y of the camera
	static float getPositionY();

	// sets the camera's (x, y) position
	static void setPosition(const float x, const float y);
	
	// gets the camera's (renderer's) width from renderConfig
	static int getWidth();

	// gets the camera's (renderer's) height from renderConfig
	static int getHeight();
};
