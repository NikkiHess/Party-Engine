// std library
#include <string>
#include <cstdlib>

// my code
#include "../errors/Error.h"
#include "AudioPlayer.h"

// dependencies
#include "AudioHelper.h"

void AudioPlayer::play(std::string& soundName, int loops) {
	std::string soundPath = "resources/audio/" + soundName; // the audio's path
	// Verify that the intro music exists
	if (resourceManager.fileExists(soundPath + ".wav")) {
		soundPath += ".wav";
	}
	else if (resourceManager.fileExists(soundPath + ".ogg")) {
		soundPath += ".ogg";
	}
	if (soundPath == "resources/audio/" + soundName) {
		Error::error("failed to play audio clip " + soundName);
	}

	// the sound to be played
	Mix_Chunk* sound = nullptr;

	// Is the sound cached? Load it
	auto it = resourceManager.sounds.find(soundName);
	if (it != resourceManager.sounds.end()) {
		sound = it->second;
	}
	// Otherwise, load from path and cache it
	else {
		sound = AudioHelper::Mix_LoadWAV498(soundPath.c_str());
		resourceManager.sounds[soundName] = sound;
	}

	// Play the sound on channel 0, looping indefinitely
	AudioHelper::Mix_PlayChannel498(0, sound, loops);
}