#pragma once

// std library
#include <string>
#include <filesystem>
#include <fstream>
#include <iostream>

// rapidjson
#include <rapidjson/document.h>
#include <rapidjson/ostreamwrapper.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/error/en.h>

class SaveData {
public:
	std::string name;

	SaveData(const std::string& name) : name(name) {
		const std::string folderPath = "resources/config/";
		const std::string extension = ".save";
		filePath = folderPath + name + extension;

		// if the file exists, try to parse it
		if (std::filesystem::exists(filePath)) {
			std::ifstream ifStream(filePath);

			if (!ifStream) {
				throw std::runtime_error("Could not open file for reading: " + filePath);
			}

			std::string content((std::istreambuf_iterator<char>(ifStream)), std::istreambuf_iterator<char>());
			ifStream.close();

			document.Parse(content.c_str());

			if (document.HasParseError()) {
				throw std::runtime_error("Parse error in JSON file: " + filePath);
			}
		}
		// the file doesn't exist
		else {
			document.SetObject();

			// create the dir if needed
			std::filesystem::create_directories(folderPath);

			saveDocument();
		}
	}

	void setString(const std::string& key, const std::string& value, bool doSave) {
		// if the file doesn't exist, create it
		if (!document.IsObject()) {
			document.SetObject();
		}

		rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());
		rapidjson::Value jsonValue;

		rapidjson::Value::MemberIterator memberItr = document.FindMember(key.c_str());

		if (memberItr != document.MemberEnd()) {
			memberItr->value.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.length()), document.GetAllocator());
		} else {
			rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());
			rapidjson::Value jsonValue(value.c_str(), document.GetAllocator());
			document.AddMember(jsonKey, jsonValue, document.GetAllocator());
		}

		document.RemoveMember(jsonKey);
		document.AddMember(jsonKey, jsonValue, document.GetAllocator());

		// optionally save changes to the file (could be slow)
		if (doSave) {
			saveDocument();
		}
	}

	void setInt(const std::string& key, int value, bool doSave) {
		if (!document.IsObject()) {
			document.SetObject();
		}

		rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

		rapidjson::Value::MemberIterator memberItr = document.FindMember(jsonKey);
		if (memberItr != document.MemberEnd()) {
			memberItr->value.SetInt(value);
		} else {
			rapidjson::Value jsonValue;
			jsonValue.SetInt(value);
			document.AddMember(jsonKey, jsonValue, document.GetAllocator());
		}

		if (doSave) {
			saveDocument();
		}
	}

	void setFloat(const std::string& key, float value, bool doSave) {
		if (!document.IsObject()) {
			document.SetObject();
		}

		rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

		rapidjson::Value::MemberIterator memberItr = document.FindMember(jsonKey);
		if (memberItr != document.MemberEnd()) {
			memberItr->value.SetFloat(value);
		} else {
			rapidjson::Value jsonValue;
			jsonValue.SetFloat(value);
			document.AddMember(jsonKey, jsonValue, document.GetAllocator());
		}

		if (doSave) {
			saveDocument();
		}
	}

	void setBool(const std::string& key, bool value, bool doSave) {
		if (!document.IsObject()) {
			document.SetObject();
		}

		rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

		rapidjson::Value::MemberIterator memberItr = document.FindMember(jsonKey);
		if (memberItr != document.MemberEnd()) {
			memberItr->value.SetBool(value);
		} else {
			rapidjson::Value jsonValue;
			jsonValue.SetBool(value);
			document.AddMember(jsonKey, jsonValue, document.GetAllocator());
		}

		if (doSave) {
			saveDocument();
		}
	}

	std::string getString(const std::string& key) {
		if (!document.HasMember(key.c_str())) {
			throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
		}

		const rapidjson::Value& value = document[key.c_str()];

		if (!value.IsString()) {
			throw std::runtime_error("Value for key " + key + " is not a string.");
		}
		return std::string(value.GetString());
	}

	int getInt(const std::string& key) {
		if (!document.HasMember(key.c_str())) {
			throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
		}

		const rapidjson::Value& value = document[key.c_str()];

		if (!value.IsInt()) {
			throw std::runtime_error("Value for key " + key + " is not a int.");
		}
		return value.GetInt();
	}

	float getFloat(const std::string& key) {
		if (!document.HasMember(key.c_str())) {
			throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
		}

		const rapidjson::Value& value = document[key.c_str()];

		if (!value.IsFloat()) {
			throw std::runtime_error("Value for key " + key + " is not a float.");
		}
		return value.GetFloat();
	}

	bool getBool(const std::string& key) {
		if (!document.HasMember(key.c_str())) {
			throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
		}

		const rapidjson::Value& value = document[key.c_str()];

		if (!value.IsBool()) {
			throw std::runtime_error("Value for key " + key + " is not a bool.");
		}
		return value.GetBool();
		throw std::runtime_error("Value must be of type: string, float, int, or bool");
	}

	bool hasMember(const std::string& key) {
		return document.HasMember(key.c_str());
	}

	// saves the document to the file path
	void saveDocument() {
		std::ofstream ofStream(filePath);

		if (!ofStream) {
			throw std::runtime_error("Could not open file for writing: " + filePath);
		}

		rapidjson::OStreamWrapper osWrapper(ofStream);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper> prettyWriter(osWrapper);

		document.Accept(prettyWriter);
		ofStream.close();
	}

	// deletes the document locally and the save file
	void clear() {
		// delete the document
		if (document.IsObject()) {
			document = rapidjson::Document();
		}
		// delete the save file
		if (std::filesystem::exists(filePath)) {
			std::filesystem::remove(filePath);
		}
	}

private:
	rapidjson::Document document;
	std::string filePath;

	template <typename T>
	void setJsonValue(rapidjson::Value& v, const T& value, rapidjson::Document::AllocatorType& allocator) {
		// Handling for std::string
		if constexpr (std::is_same<T, std::string>::value) {
			v.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.length()), allocator);
		}
		// Handling for int
		else if constexpr (std::is_same<T, int>::value) {
			v.SetInt(value);
		}
		// Handling for bool
		else if constexpr (std::is_same<T, bool>::value) {
			v.SetBool(value);
		}
		// Handling for double
		else if constexpr (std::is_same<T, double>::value) {
			v.SetDouble(value);
		}
		// Handling for float (note: there is no SetFloat, using SetDouble instead)
		else if constexpr (std::is_same<T, float>::value) {
			v.SetDouble(static_cast<double>(value));
		}
		// Add more type handling as necessary.
		throw std::runtime_error("Unsupported type for setJsonValue");
	}
};