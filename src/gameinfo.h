#pragma once

#include "MapHelper.h"

// TODO: Can this be used in GameEngine.h to keep things simpler?
// TODO: GameState vs GameInfo, can we make things less confusing?

struct GameInfo {
	Actor& player;
	int& player_health;
	int& player_score;
	std::map<Actor*, bool>& triggered_score_up;
};