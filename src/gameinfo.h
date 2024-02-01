#pragma once

#include "MapHelper.h"

struct GameInfo {
	Actor& player;
	int& player_health;
	int& player_score;
	std::map<Actor*, bool>& triggered_score_up;
};