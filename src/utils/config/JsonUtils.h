#pragma once

// std library
#include <string>

// depedencies
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"

class JsonUtils {
public:
	// reads a json file from path and puts it in the out_document
	static void readJsonFile(const std::string& path, rapidjson::Document& outDocument) {
		FILE* filePointer = nullptr;
#ifdef _WIN32
		fopen_s(&filePointer, path.c_str(), "rb");
#else
		filePointer = fopen(path.c_str(), "rb");
#endif'
		// close the use and close the filePointer if it's null
		if (filePointer != nullptr) {
			char buffer[65536];
			rapidjson::FileReadStream stream(filePointer, buffer, sizeof(buffer));
			outDocument.ParseStream(stream);
			std::fclose(filePointer);
		}
		else {
			std::cerr << "JSON file pointer is null.\n";
		}

		if (outDocument.HasParseError()) {
			rapidjson::ParseErrorCode errorCode = outDocument.GetParseError();
			std::cout << "error parsing json at [" << path << "]" << std::endl;
			exit(0);
		}
	}
};