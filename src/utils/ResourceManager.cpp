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

TextObject& ResourceManager::loadTextTexture(const std::string& text, TTF_Font* font, glm::ivec2 pos, SDL_Color& fontColor) {
	//// If cached, load the textTexture
	//auto fontIt = textTextures.find(font);
	//if (fontIt == textTextures.end()) {
	//	textTextures[font] = {};
	//}
	//auto colorIt = textTextures[font].find(fontColor);
	//if (colorIt == textTextures[font].end()) {
	//	textTextures[font][fontColor] = {};
	//}
	//auto textureIt = textTextures[font][fontColor].find(text);
	//if (textureIt == textTextures[font][fontColor].end()) {
	//	// create a surface to render our text
	//	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);

	//	textTextures[font][fontColor][text] = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

	//	SDL_FreeSurface(textSurface);
	//}





	//auto textIt = textById.find(id - 1);
	//if (textIt == textById.end()) {
	//	// create a surface to render our text
	//	
	//}

	//TextObject& object = textById[id];

	//if (object.text != text) {
	//	object.text = text;
	//}
	//if (object.font != font) {
	//	object.font = font;
	//}
	//if (object.pos != pos) {
	//	object.pos = pos;
	//}
	//if (object.color.r != fontColor.r || object.color.g != fontColor.g || object.color.b != fontColor.b || object.color.a != fontColor.a) {
	//	object.color = fontColor;
	//}

	////return textTextures[font][fontColor][text];
	//return object;

	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), fontColor);

	SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, textSurface);

	SDL_FreeSurface(textSurface);

	TextObject object = TextObject(text, pos, font, fontColor, texture);
	textToDraw.emplace(object);

	return object;
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