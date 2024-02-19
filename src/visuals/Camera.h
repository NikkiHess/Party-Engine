#pragma once

// my code
#include "../utils/config/ConfigManager.h"
#include "../gamedata/Actor.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL.h"

class Camera {
public:
	glm::vec2 pos = { 0, 0 };
	const RenderingConfig& renderConfig;

	Camera(const ConfigManager& configManager) : renderConfig(configManager.renderingConfig) {}

	glm::vec2 getPlayerPos(Actor* player) {
		// calculate the center of the screen
		// with camera offset and zoom correction
		// we divide by the zoom factor here because that will center things correctly
		int w, h;
		SDL_QueryTexture(player->view.imageFront.image, nullptr, nullptr, &w, &h);

		// the player's position
		return glm::vec2(
			player->transform.pos.x * renderConfig.pixelsPerUnit,
			player->transform.pos.y * renderConfig.pixelsPerUnit
		);
	}

	void jump(Actor* player) {
		pos = getPlayerPos(player);
	}

	void update(Actor* player, float easeFactor) {
		pos = glm::mix(pos, getPlayerPos(player), easeFactor);
	}
};
