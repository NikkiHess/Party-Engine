#include "ConfigHelper.h"

// std stuff
#include <string>
#include <iostream>
#include <filesystem>

// dependencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

bool ConfigHelper::check_file(const std::string& path) {
	if (!std::filesystem::exists(path)) {
		std::cout << "error: " + path + " missing";
		exit(0);
	}
}