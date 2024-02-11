#pragma once

// std library
#include <string>

// my code
#include "../utils/ConfigUtils.h"

class AudioPlayer
{
public:
	ConfigUtils& configUtils; // the ConfigUtils passed in

	AudioPlayer(ConfigUtils& configUtils) : configUtils(configUtils) {}

	// play a sound "loops" times
	void play(std::string& soundName, int loops);
};
