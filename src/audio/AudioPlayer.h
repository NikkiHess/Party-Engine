#pragma once

// std library
#include <string>

// my code
#include "../utils/config/ConfigManager.h"

class AudioPlayer
{
public:
	ConfigManager& configManager; // the ConfigManager passed in

	AudioPlayer(ConfigManager& configManager) : configManager(configManager) {}

	// play a sound "loops" times
	void play(std::string& soundName, int loops);
};
