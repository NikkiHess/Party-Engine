#include "ResourceManager.h"

SDL_Texture* ResourceManager::loadImageTexture(std::string& imageName) {
	SDL_Texture* imageTexture = nullptr;
	// If cached, load the imageTexture
	auto it = configManager.imageTextures.find(imageName);
	if (it != configManager.imageTextures.end()) {
		imageTexture = it->second;
	}
	else {
		std::string imagePath = "resources/images/" + imageName + ".png";
		imageTexture = IMG_LoadTexture(sdlRenderer, imagePath.c_str());
		configManager.imageTextures[imageName] = imageTexture;
	}

	return imageTexture;
}

SDL_Texture* ResourceManager::loadTextTexture(std::string& text, SDL_Color fontColor) {
	SDL_Texture* textTexture = nullptr;
	// If cached, load the textTexture
	auto it = configManager.textTextures.find(text);
	if (it != configManager.textTextures.end()) {
		textTexture = it->second;
	}
	else {
		// create a surface to render our text
		SDL_Surface* textSurface = TTF_RenderText_Solid(configManager.font, text.c_str(), fontColor);

		// create a texture from that surface
		textTexture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

		configManager.textTextures[text] = textTexture;

		SDL_FreeSurface(textSurface);
	}

	return textTexture;
}