// my code
#include "ResourceManager.h"

// sdl
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

SDL_Texture* ResourceManager::loadImageTexture(const std::string& imageName) {
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

void ResourceManager::createTextDrawRequest(const std::string& text, TTF_Font* font, glm::ivec2& pos, SDL_Color& fontColor) {
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

	SDL_FreeSurface(textSurface);

	TextDrawRequest request = TextDrawRequest(text, pos, font, fontColor, texture);
	textDrawRequests.emplace_back(request);
}

void ResourceManager::createUIImageDrawRequest(const std::string& imageName, glm::ivec2& pos, SDL_Texture* imageTexture) {
	createUIImageDrawRequestEx(imageName, pos, imageTexture, {255, 255, 255, 255}, 0);
}

void ResourceManager::createUIImageDrawRequestEx(const std::string& imageName, glm::ivec2& pos, SDL_Texture* imageTexture, SDL_Color color, int sortingOrder) {
	int width = 0, height = 0;

	SDL_QueryTexture(imageTexture, nullptr, nullptr, &width, &height);

	// set the rendering position and size (center, full size)
	SDL_Rect imageRect = { pos.x, pos.y, width, height };

	ImageDrawRequest request = ImageDrawRequest(imageName, pos, imageTexture, 0, glm::vec2(1, 1), glm::vec2(0.5f, 0.5f), color, sortingOrder, uiImageDrawRequests.size(), SCREEN_SPACE);
	uiImageDrawRequests.emplace_back(request);
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