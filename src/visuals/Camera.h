#pragma once

// my code
#include "../utils/config/ConfigManager.h"
#include "../world/Actor.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL.h"

class Camera {
public:
	glm::vec2 pos = { 0, 0 };
	const RenderingConfig& renderConfig;

	Camera(const ConfigManager& configManager) : renderConfig(configManager.renderingConfig) {}

	
};
