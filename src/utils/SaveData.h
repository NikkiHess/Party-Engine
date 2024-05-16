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

// lua
#include <lua/lua.hpp>
#include <LuaBridge/LuaBridge.h>

class SaveData {
public:
	std::string name;

    SaveData(const std::string& name);

    ~SaveData();

    void setString(const std::string& section, const std::string& key, const std::string& value, const bool doSave);
    void setInt(const std::string& section, const std::string& key, const int value, const bool doSave);
    void setFloat(const std::string& section, const std::string& key, const float value, const bool doSave);
    void setBool(const std::string& section, const std::string& key, const bool value, const bool doSave);
    void setTable(const std::string& section, const std::string& key, const luabridge::LuaRef table, const bool doSave);
    void createSection(const std::string& parentSection, const std::string& key, const bool doSave);
    void saveAll(const std::string& section, const luabridge::LuaRef componentTable, const bool doSave);

    std::string getString(const std::string& section, const std::string& key);
    int getInt(const std::string& section, const std::string& key);
    float getFloat(const std::string& section, const std::string& key);
    bool getBool(const std::string& section, const std::string& key);
    luabridge::LuaRef getTable(const std::string& section, const std::string& key);

    bool hasMember(const std::string& section, const std::string& key);

	// saves the document to the file path
    void saveDocument();

	// deletes the document locally and the save file
    void clear();

private:
	rapidjson::Document document;
	std::string filePath;
    
    rapidjson::Value saveTable(const std::string& section, const luabridge::LuaRef table);
    rapidjson::Value& getSection(std::string section);
};
