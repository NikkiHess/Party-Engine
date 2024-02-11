#pragma once

// std stuff
#include <vector>

// my code
#include "gamedata/GameInfo.h"
#include "utils/ConfigUtils.h"

// dependencies
#include "glm/glm.hpp"
#include "SDL2/SDL.h"

class Renderer {
public:
	SDL_Renderer* sdlRenderer; // the sdl renderer we're using
	ConfigUtils& configUtils; // the ConfigUtils the game uses
	glm::ivec2 renderSize; // the size of the rendered view

	Renderer(ConfigUtils& configUtils) : configUtils(configUtils), renderSize(configUtils.renderSize) {}


	SDL_Texture* loadImageTexture(std::string& imageName);

	// draw an unchanging image on the screen
	void drawStaticImage(std::string& imageName, int x, int y, int width, int height);

	// draw an unchanging image on the screen
	void drawActor(Actor& actor);

	// draw text on the screen
	void drawText(std::string& text, int fontSize, SDL_Color fontColor, int x, int y);

	// play a sound "loops" times
	void playSound(std::string& soundName, int loops);

	// render the intro images one by one, proceed when enter has been pressed
	void renderIntro(int& index);

	// render the current view, returns the current GameState
	void render(GameInfo& gameInfo);

	// render the HUD
	void renderHUD(GameInfo& gameInfo);

	// print any dialogue from nearby Actors, returns the current GameState
	// as a result of any dialogue commands executed via executeCommands
	void printDialogue(GameInfo& gameInfo);

	// prompt the player for an input command (n, e, s, w, quit)
	// does nothing and proceeds upon invalid input
	// returns NORMAL if player doesn't quit
	// returns LOSE if player quits
	void promptPlayer(GameInfo& gameInfo);
private:
	// execute the commands contained in the dialogue string with the given
	// Actor as a trigger
	GameState executeCommands(Actor& trigger, const std::string& dialogue, GameInfo& gameInfo);
};