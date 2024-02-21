// std library
#include <iostream>

// my code
#include "Artist.h"
#include "Helper.h"
#include "../gamedata/Actor.h"
#include "../gamedata/GameInfo.h"

#define COLLIDER_DEBUG 0

void Artist::drawActor(Actor& actor, Camera& camera) {
	RenderingConfig& renderConfig = configManager.renderingConfig;

	// if the actor's textures haven't been loaded, do so
	actor.loadTextures(resourceManager);

	// get the actor's image front/back size
	glm::ivec2 size = actor.transform.showBack ? actor.view.imageBack.size : actor.view.imageFront.size;
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
	glm::vec2 pivot{
		static_cast<int>(std::round(actor.view.pivot.x.value_or(actor.view.imageFront.size.x * 0.5))),
		static_cast<int>(std::round(actor.view.pivot.y.value_or(actor.view.imageFront.size.y * 0.5)))
	};

	glm::vec2 actorScreenPos = actor.getScreenPos(renderConfig, camera.pos);
	glm::vec2 bouncePos = actorScreenPos;

	// bounce :)
	if (actor.movementBounce && actor.transform.bounce) {
		bouncePos += glm::vec2(0, -glm::abs(glm::sin(Helper::GetFrameNumber() * 0.15f)) * 10.0f);
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
		static_cast<int>(std::round(bouncePos.x)),
		static_cast<int>(std::round(bouncePos.y)),
		static_cast<int>(std::abs(scaledSize.x)),
		static_cast<int>(std::abs(scaledSize.y))
	};

	if (actor.view.imageFront.image) {
		SDL_Point pivotPoint = { static_cast<int>(pivot.x), static_cast<int>(pivot.y) };
		// render the actor image
		Helper::SDL_RenderCopyEx498(actor.id, actor.name,
			sdlRenderer, renderImage, nullptr,
			&imageRect, actor.transform.rotationDegrees,
			&pivotPoint, flip
		);
	}

#if defined(COLLIDER_DEBUG) && COLLIDER_DEBUG == 1
	drawBoxCollider(actor, actorScreenPos, pivot);
	drawBoxTrigger(actor, actorScreenPos, pivot);
#endif
}

void Artist::drawBoxCollider(Actor& actor, glm::vec2& actorScreenPos, glm::vec2& pivot) {
	if (actor.boxCollider.has_value()) {
		RenderingConfig& renderConfig = configManager.renderingConfig;

		glm::vec2 center{
			static_cast<int>(std::round(actor.view.pivot.x.value_or(actor.view.imageFront.size.x * 0.5))),
			static_cast<int>(std::round(actor.view.pivot.y.value_or(actor.view.imageFront.size.y * 0.5)))
		};

		actor.calculateBoxCollider(renderConfig, actorScreenPos, center);

		SDL_Rect iRect = {
			static_cast<int>(actor.boxCollider->x),
			static_cast<int>(actor.boxCollider->y),
			static_cast<int>(actor.boxCollider->w),
			static_cast<int>(actor.boxCollider->h),
		};

		SDL_SetRenderDrawColor(sdlRenderer, 255, 0, 0, 255); // red
		SDL_RenderDrawRect(sdlRenderer, &iRect);
	}
}

void Artist::drawBoxTrigger(Actor& actor, glm::vec2& actorScreenPos, glm::vec2& pivot) {
	if (actor.boxTrigger.has_value()) {
		RenderingConfig& renderConfig = configManager.renderingConfig;

		glm::vec2 center{
			static_cast<int>(std::round(actor.view.pivot.x.value_or(actor.view.imageFront.size.x * 0.5))),
			static_cast<int>(std::round(actor.view.pivot.y.value_or(actor.view.imageFront.size.y * 0.5)))
		};

		actor.calculateBoxTrigger(renderConfig, actorScreenPos, center);

		SDL_Rect iRect = {
			static_cast<int>(actor.boxTrigger->x),
			static_cast<int>(actor.boxTrigger->y),
			static_cast<int>(actor.boxTrigger->w),
			static_cast<int>(actor.boxTrigger->h),
		};

		SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 255, 255); // blue
		SDL_RenderDrawRect(sdlRenderer, &iRect);
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