#include "Scene.h"

// std stuff
#include <string>
#include <unordered_map>

void Scene::create_actor(const std::string& name, const char& view,
						 const int& x, const int& y, const int& vel_x, const int& vel_y,
					 	 const bool& blocking, const std::string& nearby_dialogue,
						 const std::string& contact_dialogue) {
	glm::ivec2 actor_pos(x, y);
	Actor actor = Actor(name, view, actor_pos, glm::ivec2(vel_x, vel_y), blocking, nearby_dialogue, contact_dialogue);

	// insert the actor into the list of actors
	actors.push_back(actor);

	// insert the (location, actors) pair into the unordered map
	auto it = loc_to_actors.find(actor_pos);
	if (it != loc_to_actors.end())
		it->second.push_back(&actor);
	else
		loc_to_actors.insert({actor_pos, {&actor}});
}

void Scene::delete_actors() {
	loc_to_actors.clear();
}