// std library
#include <string>

// my code
#include "SaveData.h"

SaveData::SaveData(const std::string& name) : name(name) {
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

void SaveData::setString(const std::string& key, const std::string& value, const bool doSave) {
    // if the file doesn't exist, create it
    if (!document.IsObject()) document.SetObject();

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
    if (doSave) saveDocument();
}

void SaveData::setInt(const std::string& key, const int value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();

    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    rapidjson::Value::MemberIterator memberItr = document.FindMember(jsonKey);
    if (memberItr != document.MemberEnd()) {
        memberItr->value.SetInt(value);
    } else {
        rapidjson::Value jsonValue;
        jsonValue.SetInt(value);
        document.AddMember(jsonKey, jsonValue, document.GetAllocator());
    }

    if (doSave) saveDocument();
}

void SaveData::setFloat(const std::string& key, const float value, const bool doSave) {
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

void SaveData::setBool(const std::string& key, const bool value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();

    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    rapidjson::Value::MemberIterator memberItr = document.FindMember(jsonKey);
    if (memberItr != document.MemberEnd()) {
        memberItr->value.SetBool(value);
    } else {
        rapidjson::Value jsonValue;
        jsonValue.SetBool(value);
        document.AddMember(jsonKey, jsonValue, document.GetAllocator());
    }

    if (doSave) saveDocument();
}

void SaveData::setTable(const std::string& key, const luabridge::LuaRef table, const bool doSave) {
    if(!document.IsObject()) document.SetObject();
    
    rapidjson::Value jsonTable = saveTable(table);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());
    document.AddMember(jsonKey, jsonTable, document.GetAllocator());
    
    if(doSave) saveDocument();
}

void SaveData::createSection(const std::string& key, const bool doSave) {
    if(!document.IsObject()) document.SetObject();
    
    rapidjson::Value::MemberIterator iterator = document.FindMember(key.c_str());
    
    // if the section doesn't exist, create it
    if(iterator != document.MemberEnd()) {
        // this creates an empty object
        rapidjson::Value section(rapidjson::kObjectType);
        rapidjson::Value sectionKey;
        
        sectionKey.SetString(key.c_str(), document.GetAllocator());
        
        // put the section in the document
        document.AddMember(sectionKey, section, document.GetAllocator());
    }
    
    if(doSave) saveDocument();
}

// deep copy the table
rapidjson::Value SaveData::saveTable(const luabridge::LuaRef table) {
    auto& allocator = document.GetAllocator();
    
    rapidjson::Value tableObject(rapidjson::kObjectType);
    
    for (luabridge::Iterator it(table); !it.isNil(); ++it) {
        std::string key = it.key().cast<std::string>();
        luabridge::LuaRef value = it.value();
        rapidjson::Value jsonKey(key.c_str(), allocator);

        if (value.isTable()) {
            // nested table :(
            tableObject.AddMember(jsonKey, saveTable(value), allocator);
        }
        else if (value.isString()) {
            rapidjson::Value stringValue(value.cast<std::string>().c_str(), allocator);
            tableObject.AddMember(jsonKey, stringValue, allocator);
        }
        else if (value.isNumber()) {
            int intValue = value.cast<int>();
            float floatValue = value.cast<float>();
            
            // this is an int
            if(floatValue == intValue) {
                tableObject.AddMember(jsonKey, intValue, allocator);
            }
            // this is a float
            else {
                tableObject.AddMember(jsonKey, floatValue, allocator);
            }
        } 
        else if (value.isBool()) {
            tableObject.AddMember(jsonKey, rapidjson::Value().SetBool(value.cast<bool>()), allocator);
        }
    }
    return tableObject;
}

void SaveData::saveAll(const luabridge::LuaRef componentTable, const bool doSave) {
    if (!document.IsObject()) {
        document.SetObject();
    }
    
    if(componentTable.isTable()) {
        // iterate over the component table and find all
        // elements, saving them along the way
        for (luabridge::Iterator iterator(componentTable); !iterator.isNil(); ++iterator) {
            const std::string& key = iterator.key();
            const luabridge::LuaRef value = iterator.value();
            
            // skip internal keywords
            if(key == "key" || key == "enabled" || key == "actor" || value.isFunction() || (value.isTable() && !value["key"].isNil())) {
                continue;
            }
            
            // ignore functions
            if(value.isString()) {
                setString(key, value.cast<std::string>(), doSave);
            }
            else if(value.isNumber()) {
                double number = value.cast<double>();
                
                // this is an int
                if(number == static_cast<double>(static_cast<int>(number))) {
                    setInt(key, static_cast<int>(number), doSave);
                }
                // this is a float
                else {
                    setFloat(key, static_cast<float>(number), doSave);
                }
            }
            else if(value.isBool()) {
                setBool(key, value.cast<bool>(), doSave);
            }
            // need to make sure we don't have a key, otherwise this is a component
            else if(value.isTable()) {
                setTable(key, value, doSave);
            }
        }
    }
}

std::string SaveData::getString(const std::string& key) {
    if (!document.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = document[key.c_str()];

    if (!value.IsString()) {
        throw std::runtime_error("Value for key " + key + " is not a string.");
    }
    return std::string(value.GetString());
}

int SaveData::getInt(const std::string& key) {
    if (!document.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = document[key.c_str()];

    if (!value.IsInt()) {
        throw std::runtime_error("Value for key " + key + " is not an int.");
    }
    return value.GetInt();
}

float SaveData::getFloat(const std::string& key) {
    if (!document.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = document[key.c_str()];

    if (!value.IsFloat()) {
        throw std::runtime_error("Value for key " + key + " is not a float.");
    }
    return value.GetFloat();
}

bool SaveData::getBool(const std::string& key) {
    if (!document.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = document[key.c_str()];

    if (!value.IsBool()) {
        throw std::runtime_error("Value for key " + key + " is not a bool.");
    }
    return value.GetBool();
}

bool SaveData::hasMember(const std::string& key) {
    return document.HasMember(key.c_str());
}

// saves the document to the file path
void SaveData::saveDocument() {
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
void SaveData::clear() {
    // delete the document
    if (document.IsObject()) {
        document = rapidjson::Document();
    }
    // delete the save file
    if (std::filesystem::exists(filePath)) {
        std::filesystem::remove(filePath);
    }
}
