#pragma once
// the state the engine is in
// normal = nothing of note happened
// win = stop the engine with a win message
// lose = stop the engine with a lose message
enum GameState {
	NORMAL, WIN, LOSE
};