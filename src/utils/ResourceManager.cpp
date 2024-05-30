// my code
#include "ResourceManager.h"
#include "../errors/Error.h"

// sdl
#include "SDL2/SDL.h" 
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

bool ResourceManager::fileExists(const std::string& path) {
	// checks whether the cache contains the entry
	if (fileExistsCache.count(path))
		return fileExistsCache[path];
	// if the cache doesn't contain an entry, create it and return it
	bool exists = std::filesystem::exists(path);
	fileExistsCache[path] = exists;
	return exists;
}

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

int ResourceManager::createTextDrawRequest(const std::string& text, TTF_Font* font, glm::ivec2& pos, SDL_Color& fontColor, const std::string horizJust, const std::string vertJust, const bool bold, const bool underline, const bool italic, const bool strikethrough) {
    
    if(bold) TTF_SetFontStyle(font, TTF_GetFontStyle(font) | TTF_STYLE_BOLD);
    if(underline) TTF_SetFontStyle(font, TTF_GetFontStyle(font) | TTF_STYLE_UNDERLINE);
    if(italic) TTF_SetFontStyle(font, TTF_GetFontStyle(font) | TTF_STYLE_ITALIC);
    if(strikethrough) TTF_SetFontStyle(font, TTF_GetFontStyle(font) | TTF_STYLE_STRIKETHROUGH);
    
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

	SDL_FreeSurface(textSurface);

	int width, height;
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);

    if (horizJust == "center" || horizJust == "centered") {
        pos.x -= width / 2;
    }
    else if(horizJust == "right") {
        pos.x -= width;
    }
    // invalid justification
    else if(horizJust != "left") {
        Error::error("Please use one of the following for text horizontal justification: left, center(ed), right");
    }
    
    if (vertJust == "center" || vertJust == "centered") {
        pos.y -= height / 2;
    }
    else if(vertJust == "top") {
        pos.y -= height;
    }
    // invalid justification
    else if(vertJust != "bottom") {
        Error::error("Please use one of the following for text vertical justification: top, center(ed), bottom");
    }

    // create the request
	TextDrawRequest request = TextDrawRequest(text, pos, font, fontColor, texture);
	textDrawRequests.emplace(request);
    
    // clean up
    TTF_SetFontStyle(font, TTF_STYLE_NORMAL);

	return request.id;
}

int ResourceManager::createUIImageDrawRequest(SDL_Texture* imageTexture, const std::string& imageName, glm::ivec2& pos) {
	glm::vec2 scale = { 1, 1 };
	glm::vec2 pivot = { 0, 0 };
	return createUIImageDrawRequestEx(imageTexture, imageName, pos, 0, scale, pivot, {255, 255, 255, 255}, 0);
}

int ResourceManager::createUIImageDrawRequestEx(SDL_Texture* imageTexture, const std::string& imageName, glm::ivec2& pos, float rotationDegrees, 
												glm::vec2& scale, glm::vec2& pivot, SDL_Color color, int sortingOrder) {
	ImageDrawRequest request = ImageDrawRequest(imageTexture, imageName, pos, rotationDegrees, scale, pivot, 
											    color, sortingOrder, uiImageDrawRequests.size(), SCREEN_SPACE);
	uiImageDrawRequests.emplace(request);

	int width, height;
	SDL_QueryTexture(imageTexture, nullptr, nullptr, &width, &height);

	return request.id;
}

// create an image draw request, to be drawn at the end of the frame
int ResourceManager::createImageDrawRequest(SDL_Texture* imageTexture, const std::string& imageName, glm::vec2& pos) {
	glm::vec2 scale = { 1, 1 };
	glm::vec2 pivot = { 0.5f, 0.5f };
	SDL_Color color = { 255, 255, 255, 255 };

	return createImageDrawRequestEx(imageTexture, imageName, pos, 0, scale, pivot, color, 0);
}

// create an image draw request (with more data), to be drawn at the end of the frame
int ResourceManager::createImageDrawRequestEx(SDL_Texture* imageTexture, const std::string& imageName, glm::vec2& pos, float rotationDegrees, 
											   glm::vec2& scale, glm::vec2& pivot, SDL_Color color, int sortingOrder) {
	ImageDrawRequest request = ImageDrawRequest(imageTexture, imageName, pos, rotationDegrees, scale, pivot, color, 
												sortingOrder, imageDrawRequests.size(), SCENE_SPACE);
	imageDrawRequests.emplace(request);

	int width, height;
	SDL_QueryTexture(imageTexture, nullptr, nullptr, &width, &height);

	return request.id;
}

void ResourceManager::createPixelDrawRequest(glm::ivec2& pos, SDL_Color color) {
	PixelDrawRequest request = PixelDrawRequest(pos, color);
	pixelDrawRequests.emplace(request);
}
