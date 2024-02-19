// std library
#include <iostream>

// my code
#include "Artist.h"
#include "../gamedata/GameInfo.h"

void Artist::drawActor(Actor& actor, Camera& camera) {
	RenderingConfig& renderConfig = configManager.renderingConfig;

	// check if the actor's image needs to be loaded
	if (!actor.view.image && actor.view.imageName != "") {
		actor.view.image = resourceManager.loadImageTexture(actor.view.imageName);
	}

	// get the actor's image size
	glm::ivec2 size(0);
	if (actor.view.imageSize == glm::ivec2(0)) {
		if (actor.view.image) {
			SDL_QueryTexture(actor.view.image, nullptr, nullptr, &size.x, &size.y);
		}
		actor.view.imageSize = size;
	}
	size = actor.view.imageSize;

	// scale size using actor.transform.scale
	glm::ivec2 scaledSize(
		size.x * actor.transform.scale.x,
		size.y * actor.transform.scale.y
	);

	// determine the flip state
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	if (scaledSize.x < 0 || scaledSize.y < 0) {
		flip = SDL_RendererFlip(
			flip | (scaledSize.x < 0 ? SDL_FLIP_HORIZONTAL : 0) |
			(scaledSize.y < 0 ? SDL_FLIP_VERTICAL : 0)
		);
	}

	// x and y either from config or (width or height) * 0.5 * scale
	// NOTE TO SELF: the pivot point should always use size, not scaledSize
	SDL_Point pivot{
		static_cast<int>(std::round(actor.view.pivotOffset.x.value_or(size.x * 0.5))),
		static_cast<int>(std::round(actor.view.pivotOffset.y.value_or(size.y * 0.5)))
	};

	// camera center in pixel coordinates
	glm::ivec2 cameraCenter(
		(renderConfig.renderSize.x / 2 - renderConfig.cameraOffset.x * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor,
		(renderConfig.renderSize.y / 2 - renderConfig.cameraOffset.y * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor
	);

	// actor world position in pixel coordinates
	glm::ivec2 actorWorldPos(
		static_cast<int>(std::round(actor.transform.pos.x * renderConfig.pixelsPerUnit) - pivot.x),
		static_cast<int>(std::round(actor.transform.pos.y * renderConfig.pixelsPerUnit) - pivot.y)
	);

	// actor position relative to the camera
	glm::ivec2 actorCameraRelativePos = actorWorldPos - glm::ivec2(std::round(camera.pos.x), std::round(camera.pos.y));

	// actor screen position, accounting for rendering at screen center
	glm::ivec2 actorScreenPos = cameraCenter + actorCameraRelativePos;

	// if actor is not within visible area, skip rendering (cull)
	// include a little buffer so as not to cull too early
	// make sure to divide upper bound by zoom factor, because otherwise stuff gets unrendered at zoomFactor < 1
	if (actorScreenPos.x < -std::abs(scaledSize.x * 1.2f) || actorScreenPos.x > renderConfig.renderSize.x * 1.1f / renderConfig.zoomFactor ||
		actorScreenPos.y < -std::abs(scaledSize.y * 1.2f) || actorScreenPos.y > renderConfig.renderSize.y * 1.1f / renderConfig.zoomFactor) {
		//std::cout << actor.name << " culled\n";
		return;
	}

	// center position around the pivot point
	// offset by scaledSize if we flip either one
	SDL_Rect imageRect = {
		actorScreenPos.x,
		actorScreenPos.y,
		std::abs(scaledSize.x),
		std::abs(scaledSize.y) 
	};

	if (actor.view.image) {
		// render the actor image
		SDL_RenderCopyEx(
			sdlRenderer, actor.view.image, nullptr,
			&imageRect, actor.transform.rotationDegrees,
			&pivot, flip
		);
	}
}

void Artist::drawUIImage(std::string& imageName, glm::ivec2 pos, glm::ivec2 size) {
	SDL_Texture* imageTexture = resourceManager.loadImageTexture(imageName);

	// Set the rendering position and size (center, full size)
	SDL_Rect imageRect = { pos.x, pos.y, size.x, size.y };

	// UI images should always be unscaled
	SDL_RenderSetScale(sdlRenderer, 1, 1);

	// Copy the texture to the renderer
	SDL_RenderCopy(sdlRenderer, imageTexture, nullptr, &imageRect);
}

void Artist::drawUIText(std::string& text, SDL_Color fontColor, glm::ivec2 pos) {
	SDL_Texture* textTexture = resourceManager.loadTextTexture(text, fontColor);

	int width = 0, height = 0;

	SDL_QueryTexture(textTexture, nullptr, nullptr, &width, &height);

	// create a rect to render the text in
	SDL_Rect textRect = { pos.x, pos.y, width, height };

	// UI text should always be unscaled
	SDL_RenderSetScale(sdlRenderer, 1, 1);

	// copy the texture to the renderer
	SDL_RenderCopy(sdlRenderer, textTexture, nullptr, &textRect);
}