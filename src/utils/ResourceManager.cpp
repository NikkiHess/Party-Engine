// my code
#include "ResourceManager.h"

// sdl
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

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

TextDrawRequest& ResourceManager::createTextDrawRequest(const std::string& text, TTF_Font* font, glm::ivec2 pos, SDL_Color& fontColor) {
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

	SDL_FreeSurface(textSurface);

	TextDrawRequest request = TextDrawRequest(text, pos, font, fontColor, texture);
	textDrawRequests.emplace(request);

	return request;
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