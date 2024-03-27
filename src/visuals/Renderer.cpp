// std library
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>

// include my code
#include "../world/Actor.h"
#include "../GameInfo.h"
#include "../GameEngine.h"
#include "../utils/StringUtils.h"
#include "Renderer.h"

// dependencies
#include "AudioHelper.h"
#include "glm/glm.hpp"
#include "Helper.h"
#include "rapidjson/document.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_ttf.h"

void Renderer::render(GameInfo& gameInfo) {
	RenderingConfig& renderConfig = configManager.renderingConfig;

	// Clear the frame buffer at the beginning of a frame
	SDL_SetRenderDrawColor(
		sdlRenderer, 
		renderConfig.clearColor.r,
		renderConfig.clearColor.g,
		renderConfig.clearColor.b,
		1
	);
	SDL_RenderClear(sdlRenderer);

	// set the render scale according to the configured zoom factor
	SDL_RenderSetScale(sdlRenderer, renderConfig.zoomFactor, renderConfig.zoomFactor);

	// draw all actors in order of transform_position_y
	for (std::shared_ptr<Actor> actor : gameInfo.scene.actorsByRenderOrder) {
		Artist::drawActor(*actor, gameInfo.camera);
	}

	for (const TextDrawRequest& textObject : resourceManager.textDrawRequests) {
		Artist::drawUIText(textObject);
	}
	resourceManager.textDrawRequests.clear();
}