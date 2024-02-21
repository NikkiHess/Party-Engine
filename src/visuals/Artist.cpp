// std library
#include <iostream>

// my code
#include "Artist.h"
#include "Helper.h"
#include "../gamedata/GameInfo.h"

void Artist::drawActor(Actor& actor, Camera& camera) {
	RenderingConfig& renderConfig = configManager.renderingConfig;

	// check if the actor's images need to be loaded
	if (!actor.view.imageFront.image && actor.view.imageFront.name != "") {
		actor.view.imageFront.image = resourceManager.loadImageTexture(actor.view.imageFront.name);
	}
	if (!actor.view.imageBack.image && actor.view.imageBack.name != "") {
		actor.view.imageBack.image = resourceManager.loadImageTexture(actor.view.imageBack.name);
	}

	// get the actor's image front/back size
	glm::ivec2 size(0);

	// load in the images' sizes if they haven't been already
	if (actor.view.imageFront.size == glm::ivec2(0)) {
		SDL_QueryTexture(actor.view.imageFront.image, nullptr, nullptr, &size.x, &size.y);
		actor.view.imageFront.size = size;
	}
	if (actor.view.imageBack.size == glm::ivec2(0)) {
		SDL_QueryTexture(actor.view.imageBack.image, nullptr, nullptr, &size.x, &size.y);
		actor.view.imageBack.size = size;
	}
	
	size = actor.transform.showBack ? actor.view.imageBack.size : actor.view.imageFront.size;
	glm::ivec2 frontSize(actor.view.imageFront.size);

	// get the renderImage AFTER we load it in...
	SDL_Texture* renderImage = actor.transform.showBack ? actor.view.imageBack.image : actor.view.imageFront.image;

	// scale size using actor.transform.scale
	// also flip on x if the actor is flipped at the moment
	glm::vec2 scaledSize(
		size.x * actor.transform.scale.x * (actor.transform.flipped ? -1 : 1),
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
		static_cast<int>(std::round(actor.view.pivot.x.value_or(frontSize.x * 0.5))),
		static_cast<int>(std::round(actor.view.pivot.y.value_or(frontSize.y * 0.5)))
	};

	// camera center in pixel coordinates
	glm::vec2 cameraCenter(
		(renderConfig.renderSize.x / 2 - renderConfig.cameraOffset.x * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor,
		(renderConfig.renderSize.y / 2 - renderConfig.cameraOffset.y * renderConfig.pixelsPerUnit) / renderConfig.zoomFactor
	);

	// actor world position in pixel coordinates
	glm::vec2 actorWorldPos(
		(actor.transform.pos.x * renderConfig.pixelsPerUnit) - pivot.x,
		(actor.transform.pos.y * renderConfig.pixelsPerUnit) - pivot.y
	);

	// actor position relative to the camera
	glm::vec2 actorCameraRelativePos = actorWorldPos - glm::vec2(std::round(camera.pos.x), std::round(camera.pos.y));

	// actor screen position, accounting for rendering at screen center
	glm::vec2 actorScreenPos = cameraCenter + actorCameraRelativePos;

	// bounce :)
	if (actor.movementBounce && actor.transform.bounce) {
		actorScreenPos += glm::vec2(0, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);
	}

	// if actor is not within visible area, skip rendering (cull)
	// include a little buffer so as not to cull too early
	// make sure to divide upper bound by zoom factor, because otherwise stuff gets unrendered at zoomFactor < 1
	if (actorScreenPos.x < -std::abs(scaledSize.x * 1.2f) || actorScreenPos.x > renderConfig.renderSize.x * 1.1f / renderConfig.zoomFactor ||
		actorScreenPos.y < -std::abs(scaledSize.y * 1.2f) || actorScreenPos.y > renderConfig.renderSize.y * 1.1f / renderConfig.zoomFactor) {
		// std::cout << actor.name << " culled\n";
		return;
	}

	// center position around the pivot point
	// offset by scaledSize if we flip either one
	SDL_Rect imageRect = {
		static_cast<int>(std::round(actorScreenPos.x)),
		static_cast<int>(std::round(actorScreenPos.y)),
		static_cast<int>(std::abs(scaledSize.x)),
		static_cast<int>(std::abs(scaledSize.y))
	};

	if (actor.view.imageFront.image) {
		// render the actor image
		SDL_RenderCopyEx(
			sdlRenderer, renderImage, nullptr,
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