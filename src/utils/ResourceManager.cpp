#include "ResourceManager.h"

SDL_Texture* ResourceManager::loadImageTexture(std::string& imageName) {
	SDL_Texture* imageTexture = nullptr;
	// If cached, load the imageTexture
	auto it = imageTextures.find(imageName);
	if (it != imageTextures.end()) {
		imageTexture = it->second;
	}
	else {
		std::string imagePath = "resources/images/" + imageName + ".png";
		imageTexture = IMG_LoadTexture(sdlRenderer, imagePath.c_str());
		imageTextures[imageName] = imageTexture;
	}

	return imageTexture;
}

SDL_Texture* ResourceManager::loadTextTexture(std::string& text, SDL_Color fontColor) {
	SDL_Texture* textTexture = nullptr;
	// If cached, load the textTexture
	auto it = textTextures.find(text);
	if (it != textTextures.end()) {
		textTexture = it->second;
	}
	else {
		// create a surface to render our text
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);

		// create a texture from that surface
		textTexture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

		textTextures[text] = textTexture;

		SDL_FreeSurface(textSurface);
	}

	return textTexture;
}

bool ResourceManager::fileExists(const std::string& path) {
	// checks whether the cache contains the entry
	if (fileExistsCache.count(path))
		return fileExistsCache[path];
	// if the cache doesn't contain an entry, create it and return it
	bool exists = std::filesystem::exists(path);
	fileExistsCache[path] = exists;
	return exists;
}