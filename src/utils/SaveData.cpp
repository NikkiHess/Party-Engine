// std library
#include <string>
#include <cmath>

// my code
#include "SaveData.h"
#include "../utils/LuaStateSaver.h"
#include "../world/Actor.h"

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

SaveData::~SaveData() {
    // clear document, minimizing and recreating allocator
    // https://github.com/Tencent/rapidjson/issues/368#issuecomment-117805277
    rapidjson::Document().Swap(document).SetObject();
    rapidjson::Document(rapidjson::kObjectType).Swap(document);
}

rapidjson::Value& SaveData::getSection(std::string section) {
    // ensure the document root is an object
    if (!document.IsObject()) document.SetObject();

    auto& allocator = document.GetAllocator();
    rapidjson::Value* sectionPtr = &document; // start at the root of the document
    
    std::string delimiter = ".";
    size_t pos = 0;
    std::string subsection;
    
    while ((pos = section.find(delimiter)) != std::string::npos) {
        subsection = section.substr(0, pos);
        rapidjson::Value subsectionVal(subsection.c_str(), allocator);

        // create a new section if it doesn't exist
        if (!sectionPtr->HasMember(subsectionVal)) {
            sectionPtr->AddMember(subsectionVal, rapidjson::Value(rapidjson::kObjectType), allocator);
        }

        // get the next level in the hierarchy
        sectionPtr = &((*sectionPtr)[subsection.c_str()]);
        section.erase(0, pos + delimiter.length());
    }
    
    // handle the final subsection
    if (!section.empty()) {
        rapidjson::Value sectionVal(section.c_str(), allocator);
        if (!sectionPtr->HasMember(sectionVal)) {
            sectionPtr->AddMember(sectionVal, rapidjson::Value(rapidjson::kObjectType), allocator);
        }
        sectionPtr = &((*sectionPtr)[section.c_str()]);
    }
    
    return *sectionPtr; // return the targeted section
}

void SaveData::setString(const std::string& section, const std::string& key, const std::string& value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    // get the target section
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());
    rapidjson::Value jsonValue(value.c_str(), document.GetAllocator());
    
    rapidjson::Value::MemberIterator itr = targetSection.FindMember(jsonKey);
    if (itr != targetSection.MemberEnd()) {
        itr->value.SetString(value.c_str(), document.GetAllocator());
    } else {
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator());
    }

    if (doSave) saveDocument();
}

void SaveData::setInt(const std::string& section, const std::string& key, const int value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonValue(value);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonValue;
    } else {
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator());
    }

    if (doSave) saveDocument();
}

void SaveData::setFloat(const std::string& section, const std::string& key, const float value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonValue(value);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonValue;
    } else {
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator());
    }

    if (doSave) saveDocument();
}

void SaveData::setBool(const std::string& section, const std::string& key, const bool value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonValue(value);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonValue;
    } else {
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator());
    }

    if (doSave) saveDocument();
}

void SaveData::setTable(const std::string& section, const std::string& key, const luabridge::LuaRef table, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    rapidjson::Value& targetSection = getSection(section);
    
    rapidjson::Value jsonTable = saveTable(section, table);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonTable;
    } else {
        targetSection.AddMember(jsonKey, jsonTable, document.GetAllocator());
    }

    // Save the document if required
    if (doSave) saveDocument();
}

void SaveData::createSection(const std::string& parentSection, const std::string& key, const bool doSave) {
    if(!document.IsObject()) document.SetObject();
    
    rapidjson::Value& targetSection = getSection(parentSection);
    
    rapidjson::Value::MemberIterator iterator = targetSection.FindMember(key.c_str());
    
    // if the section doesn't exist, create it
    if(iterator != document.MemberEnd()) {
        // this creates an empty object
        rapidjson::Value section(rapidjson::kObjectType);
        rapidjson::Value sectionKey;
        
        sectionKey.SetString(key.c_str(), document.GetAllocator());
        
        // put the section in the document
        targetSection.AddMember(sectionKey, section, document.GetAllocator());
    }
    
    if(doSave) saveDocument();
}

// deep copy the table
rapidjson::Value SaveData::saveTable(const std::string& section, const luabridge::LuaRef table) {
    auto& allocator = document.GetAllocator();
    
    rapidjson::Value tableObject(rapidjson::kObjectType);
    
    for (luabridge::Iterator it(table); !it.isNil(); ++it) {
        std::string key = it.key().cast<std::string>();
        luabridge::LuaRef value = it.value();
        rapidjson::Value jsonKey(key.c_str(), allocator);

        if (value.isTable()) {
            // nested table, recurse
            tableObject.AddMember(jsonKey, saveTable(section + "." + key, value), allocator);
        }
        else if (value.isString()) {
            rapidjson::Value stringValue(value.cast<std::string>().c_str(), allocator);
            tableObject.AddMember(jsonKey, stringValue, allocator);
        }
        else if (value.isNumber()) {
            std::string numberStr = value.cast<std::string>();
            
            float number = std::stof(numberStr);
            // this is an int
            if(numberStr.find('.') == std::string::npos) {
                tableObject.AddMember(jsonKey, rapidjson::Value(static_cast<int>(number)), allocator);
            }
            // this is a float
            else {
                tableObject.AddMember(jsonKey, rapidjson::Value(static_cast<float>(number)), allocator);
            }
        }
        else if (value.isBool()) {
            tableObject.AddMember(jsonKey, rapidjson::Value().SetBool(value.cast<bool>()), allocator);
        }
    }
    return tableObject;
}

void SaveData::saveAll(const std::string& baseSection, const luabridge::LuaRef componentTable, const bool doSave) {
    if (!document.IsObject()) {
        document.SetObject();
    }

    if (componentTable.isTable()) {
        for (luabridge::Iterator iterator(componentTable); !iterator.isNil(); ++iterator) {
            const std::string& key = iterator.key();
            const luabridge::LuaRef value = iterator.value();
            
            if (value.isFunction() || // skip functions
                (value.isTable() && !value["key"].isNil()) || // skip tables with key
                value.isUserdata() || // skip Userdatas
                value.isInstance<Actor>() || // skip Actors
                key == "key" || key == "enabled" || key == "actor") { // skip keywords
                continue;
            }
            
            const std::string fullSectionPath = value.isTable() ? baseSection + "." + key : baseSection;
            
            if (value.isString()) {
                setString(fullSectionPath, key, value.cast<std::string>(), doSave);
            }
            else if (value.isNumber()) {
                std::string numberStr = value.cast<std::string>();
                
                float number = std::stof(numberStr);
                // this is an int
                if(numberStr.find('.') == std::string::npos) {
                    setInt(fullSectionPath, key, static_cast<int>(number), doSave);
                } else {
                    setFloat(fullSectionPath, key, number, doSave);
                }
            }
            else if (value.isBool()) {
                setBool(fullSectionPath, key, value.cast<bool>(), doSave);
            }
            else if (value.isTable()) {
                saveAll(fullSectionPath, value, doSave);
            }
        }
    }

    if (doSave) {
        saveDocument();
    }
}

std::string SaveData::getString(const std::string& section, const std::string& key) {
    auto& innermostSection = getSection(section);
    
    if (!innermostSection.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = innermostSection[key.c_str()];

    if (!value.IsString()) {
        throw std::runtime_error("Value for key " + key + " is not a string.");
    }
    return std::string(value.GetString());
}

int SaveData::getInt(const std::string& section, const std::string& key) {
    auto& innermostSection = getSection(section);
    
    if (!innermostSection.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = innermostSection[key.c_str()];

    if (!value.IsInt()) {
        throw std::runtime_error("Value for key " + key + " is not an int.");
    }
    return value.GetInt();
}

float SaveData::getFloat(const std::string& section, const std::string& key) {
    auto& innermostSection = getSection(section);
    
    if (!innermostSection.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = innermostSection[key.c_str()];

    if (!value.IsFloat()) {
        throw std::runtime_error("Value for key " + key + " is not a float.");
    }
    return value.GetFloat();
}

bool SaveData::getBool(const std::string& section, const std::string& key) {
    auto& innermostSection = getSection(section);
    
    if (!innermostSection.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = innermostSection[key.c_str()];

    if (!value.IsBool()) {
        throw std::runtime_error("Value for key " + key + " is not a bool.");
    }
    return value.GetBool();
}

luabridge::LuaRef SaveData::getTable(const std::string& section, const std::string& key) {
    auto& innermostSection = getSection(section);
    
    if (!innermostSection.HasMember(key.c_str())) {
        throw std::runtime_error("Key " + key + " does not exist in JSON file: " + filePath);
    }

    const rapidjson::Value& value = innermostSection[key.c_str()];
    
    if(!value.IsObject()) {
        throw std::runtime_error("Value for key " + key + " is not a table.");
    }
    
    luabridge::LuaRef ref = luabridge::LuaRef(LuaStateSaver::luaState);
    // loop over table
    for (auto& data : value.GetObject()) {
        const std::string& name = data.name.GetString();

        // make sure we don't add "type" to our properties
        // we already used it to get the Lua file we needed
        if (name != "type") {
            if (data.value.IsString())
                ref[name] = data.value.GetString();
            if (data.value.IsFloat())
                ref[name] = data.value.GetFloat();
            if (data.value.IsInt())
                ref[name] = data.value.GetInt();
            if (data.value.IsBool())
                ref[name] = data.value.GetBool();
            // TODO: Does this work?
            if(data.value.IsObject())
                ref[name] = getTable(section + ".key", key);
        }
    }
    
    return ref;
}

bool SaveData::hasMember(const std::string& section, const std::string& key) {
    auto& innermostSection = getSection(section);
    
    return innermostSection.HasMember(key.c_str());
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
