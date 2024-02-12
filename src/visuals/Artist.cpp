// std library
#include <iostream>

// my code
#include "Artist.h"
#include "../Constants.h"
#include "../gamedata/GameInfo.h"

SDL_Texture* Artist::loadImageTexture(std::string& imageName) {
	SDL_Texture* imageTexture = nullptr;
	// If cached, load the imageTexture
	auto it = configUtils.imageTextures.find(imageName);
	if (it != configUtils.imageTextures.end()) {
		imageTexture = it->second;
	}
	else {
		std::string imagePath = "resources/images/" + imageName + ".png";
		imageTexture = IMG_LoadTexture(sdlRenderer, imagePath.c_str());
		configUtils.imageTextures[imageName] = imageTexture;
	}

	return imageTexture;
}

SDL_Texture* Artist::loadTextTexture(std::string& text, SDL_Color fontColor) {
	SDL_Texture* textTexture = nullptr;
	// If cached, load the textTexture
	auto it = configUtils.textTextures.find(text);
	if (it != configUtils.textTextures.end()) {
		textTexture = it->second;
	}
	else {
		// create a surface to render our text
		SDL_Surface* textSurface = TTF_RenderText_Solid(configUtils.font, text.c_str(), fontColor);

		// create a texture from that surface
		textTexture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

		configUtils.textTextures[text] = textTexture;

		SDL_FreeSurface(textSurface);
	}

	return textTexture;
}

void Artist::drawActor(GameInfo& gameInfo, Actor& actor) {
	glm::ivec2 size(0, 0);
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	glm::dvec2 playerPos = (gameInfo.player ? gameInfo.player->transform.pos : glm::dvec2(0));

	// if the image hasn't been loaded in yet and there is one to be found, do it.
	if (!actor.view.image && actor.view.imageName != "") {
		actor.view.image = loadImageTexture(actor.view.imageName);
	}

	// get the width and height from the actor view
	SDL_QueryTexture(actor.view.image, nullptr, nullptr, &size.x, &size.y);

	// scale size using actor.transform.scale
	glm::ivec2 scaledSize(
		size.x * actor.transform.scale.x,
		size.y * actor.transform.scale.y
	);

	if (scaledSize.x < 0) {
		flip = SDL_RendererFlip(flip | SDL_FLIP_HORIZONTAL);
	}
	if (scaledSize.y < 0) {
		flip = SDL_RendererFlip(flip | SDL_FLIP_VERTICAL);
	}

	// calculate the center of the screen, offset by the camera position
	glm::dvec2 screenCenter(
		configUtils.renderSize.x / 2.0 - configUtils.cameraOffset.x * Constants::PIXELS_PER_UNIT, 
		configUtils.renderSize.y / 2.0 - configUtils.cameraOffset.y * Constants::PIXELS_PER_UNIT
	);

	// x and y either from config or (width or height) * 0.5 * scale
	// NOTE TO SELF: the pivot point should always use size, not scaledSize
	SDL_Point pivot{
		static_cast<int>(std::round(actor.view.pivotOffset.x.value_or(size.x * 0.5) * actor.transform.scale.x)),
		static_cast<int>(std::round(actor.view.pivotOffset.y.value_or(size.y * 0.5) * actor.transform.scale.y))
	};

	// subtract this
	glm::dvec2 playerPosOffset(
		playerPos.x * Constants::PIXELS_PER_UNIT,
		playerPos.y * Constants::PIXELS_PER_UNIT
	);

	// the image position, without the scale offset
	glm::ivec2 imagePos(
		static_cast<int>(std::round(screenCenter.x + actor.transform.pos.x * Constants::PIXELS_PER_UNIT - pivot.x - playerPosOffset.x)),
		static_cast<int>(std::round(screenCenter.y + actor.transform.pos.y * Constants::PIXELS_PER_UNIT - pivot.y - playerPosOffset.y))
	);

	// account for odd flips
	glm::ivec2 rectPos(
		imagePos.x + (scaledSize.x < 0 ? scaledSize.x + (scaledSize.x % 2) : 0),
		imagePos.y + (scaledSize.y < 0 ? scaledSize.y + (scaledSize.y % 2) : 0)
	);

	// center position around the pivot point
	// offset by scaledSize if we flip either one
	SDL_Rect imageRect = {
		rectPos.x,
		rectPos.y,
		abs(scaledSize.x),
		abs(scaledSize.y) };

	SDL_RenderCopyEx(
		sdlRenderer, actor.view.image, nullptr,
		&imageRect, actor.transform.rotationDegrees,
		&pivot, flip
	);
}

void Artist::drawStaticImage(std::string& imageName, glm::ivec2 pos, glm::ivec2 size) {
	SDL_Texture* imageTexture = loadImageTexture(imageName);

	// Set the rendering position and size (center, full size)
	SDL_Rect imageRect = { pos.x, pos.y, size.x, size.y };

	// Copy the texture to the renderer
	SDL_RenderCopy(sdlRenderer, imageTexture, nullptr, &imageRect);
}

void Artist::drawText(std::string& text, SDL_Color fontColor, glm::ivec2 pos) {
	SDL_Texture* textTexture = loadTextTexture(text, fontColor);

	int width = 0, height = 0;

	SDL_QueryTexture(textTexture, nullptr, nullptr, &width, &height);

	// create a rect to render the text in
	SDL_Rect textRect = { pos.x, pos.y, width, height };

	// copy the texture to the renderer
	SDL_RenderCopy(sdlRenderer, textTexture, nullptr, &textRect);
}