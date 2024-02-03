#include "ConfigHelper.h"

// std stuff
#include <string>
#include <iostream>
#include <filesystem>

// my code
#include "Scene.h"

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

bool ConfigHelper::check_file(const std::string& path) {
	if (!std::filesystem::exists(path)) {
		std::cout << "error: " + path + " missing";
		exit(0);
	}
}

void ConfigHelper::initialize_messages(rapidjson::Document& document) {
	if (document.HasMember("game_start_message"))
		game_start_message = document["game_start_message"].GetString();
	if (document.HasMember("game_over_bad_message"))
		game_over_bad_message = document["game_over_bad_message"].GetString();
	if (document.HasMember("game_over_good_message"))
		game_over_good_message = document["game_over_good_message"].GetString();
}

void ConfigHelper::initialize_rendering(rapidjson::Document& document) {
	if (document.HasMember("x_resolution"))
		render_size.x = document["x_resolution"].GetInt();
	if (document.HasMember("y_resolution"))
		render_size.y = document["y_resolution"].GetInt();

	// we've been told in the spec we can assume it will always be odd, but...
	if (render_size.x % 2 == 0 || render_size.y % 2 == 0) {
		std::cout << "error: render size must be odd.";
		exit(0); // should this be a different exit code?
	}
}

void ConfigHelper::initialize_scene(std::string& resources, rapidjson::Document& document) {
	// get the name of the initial scene
	if (!document.HasMember("initial_scene")) {
		std::cout << "error: initial_scene unspecified";
		exit(0);
	}
	initial_scene.name = document["initial_scene"].GetString();

	// get the path of the initial scene and read its file
	std::string initial_scene_path = resources + "scenes/" + initial_scene.name + ".scene";
	if (!std::filesystem::exists(initial_scene_path)) {
		std::cout << "error: scene " + initial_scene.name + " is missing";
		exit(0);
	}
	read_json_file(initial_scene_path, document);

	// TODO: Make this not do if else, do like above
	if (document.HasMember("actors")) {
		rapidjson::GenericArray doc_actors = document["actors"].GetArray();

		for (unsigned int i = 0; i < doc_actors.Size(); ++i) {
			std::string name = "";
			char view = '?';
			int x = 0, y = 0, vel_x = 0, vel_y = 0;
			bool blocking = false;
			std::string nearby_dialogue = "", contact_dialogue = "";

			if (doc_actors[i].HasMember("name"))
				name = doc_actors[i]["name"].GetString();
			if (doc_actors[i].HasMember("view"))
				view = doc_actors[i]["view"].GetString()[0];
			if (doc_actors[i].HasMember("x"))
				x = doc_actors[i]["x"].GetInt();
			if (doc_actors[i].HasMember("y"))
				y = doc_actors[i]["y"].GetInt();
			if (doc_actors[i].HasMember("vel_x"))
				vel_x = doc_actors[i]["vel_x"].GetInt();
			if (doc_actors[i].HasMember("vel_y"))
				vel_y = doc_actors[i]["vel_y"].GetInt();
			if (doc_actors[i].HasMember("blocking"))
				blocking = doc_actors[i]["blocking"].GetBool();
			if (doc_actors[i].HasMember("nearby_dialogue"))
				nearby_dialogue = doc_actors[i]["nearby_dialogue"].GetString();
			if (doc_actors[i].HasMember("contact_dialogue"))
				contact_dialogue = doc_actors[i]["contact_dialogue"].GetString();

			initial_scene.create_actor(name, view, x, y, vel_x, vel_y, blocking, nearby_dialogue, contact_dialogue);
		}
	}
	else {
		std::cout << "error: \"actors\" is missing from " + initial_scene.name;
		exit(0);
	}
}