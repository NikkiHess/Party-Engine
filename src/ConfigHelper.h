#pragma once

// std stuff
#include <string>
#include <iostream>
#include <filesystem>

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class ConfigHelper {
public:
	rapidjson::Document document = nullptr;
	std::string game_start_message;
	std::string game_over_bad_message, game_over_good_message;

	// initializes the config helper by verifying the resources directory as well as the game.config
	// reads the json from the given file and then loads the information into member variables
	ConfigHelper() {
		std::string resources = "resources/";
		std::string game_config = resources + "game.config";

		check_file(resources);
		check_file(game_config);
		read_json_file(resources + "game.config", document);

		initialize_messages();
	}
private:
	// checks that a file exists, and if not prints an error message and exits with code 1
	bool check_file(const std::string& path);

	// initializes the messages loaded in from the resources/game.config file
	void initialize_messages();

	// reads a json file from path and puts it in the out_document
	static void read_json_file(const std::string& path, rapidjson::Document& out_document) {
		FILE* file_pointer = nullptr;
#ifdef _WIN32
		fopen_s(&file_pointer, path.c_str(), "rb");
#else
		file_pointer = fopen(path.c_str(), "rb");
#endif
		char buffer[65536];
		rapidjson::FileReadStream stream(file_pointer, buffer, sizeof(buffer));
		out_document.ParseStream(stream);
		std::fclose(file_pointer);

		if (out_document.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = out_document.GetParseError();
			std::cout << "error parsing json at [" << path << "]" << std::endl;
			exit(0);
		}
	}
};