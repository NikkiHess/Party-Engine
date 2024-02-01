#pragma once

// std stuff
#include <string>
#include <iostream>
#include <filesystem>

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class JsonHelper {
public:
	rapidjson::Document document = nullptr;
	std::string game_start_message;
	std::string game_over_bad_message, game_over_good_message;

	JsonHelper() {
		std::string resources = "resources/";
		std::string game_config = resources + "game.config";

		check_file(resources);
		check_file(game_config);
		readJsonFile(resources + "game.config", document);

		if (document != nullptr) {
			game_start_message = document["game_start_message"].GetString();
			game_over_bad_message = document["game_over_bad_message"].GetString();
			game_over_good_message = document["game_over_good_message"].GetString();
		}
	}
private:
	static void readJsonFile(const std::string& path, rapidjson::Document& out_document) {
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

	bool check_file(const std::string& path) {
		if (!std::filesystem::exists(path)) {
			std::cout << "error: " + path + " missing";
			exit(0);
		}
	}
};