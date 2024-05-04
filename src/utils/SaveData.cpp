// std library
#include <string>

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

rapidjson::Value& SaveData::getSection(std::string section) {
    // Ensure the document root is an object
    if (!document.IsObject()) document.SetObject();

    auto& allocator = document.GetAllocator();
    rapidjson::Value* sectionPtr = &document; // Start at the root of the document
    
    std::string delimiter = ".";
    size_t pos = 0;
    std::string subsection;
    
    while ((pos = section.find(delimiter)) != std::string::npos) {
        subsection = section.substr(0, pos);
        rapidjson::Value subsectionVal(subsection.c_str(), allocator);

        // Create a new section if it doesn't exist
        if (!sectionPtr->HasMember(subsectionVal)) {
            sectionPtr->AddMember(subsectionVal, rapidjson::Value(rapidjson::kObjectType), allocator);
        }

        // Get the next level in the hierarchy
        sectionPtr = &((*sectionPtr)[subsection.c_str()]);
        section.erase(0, pos + delimiter.length());
    }
    
    // Handle the final subsection
    if (!section.empty()) {
        rapidjson::Value sectionVal(section.c_str(), allocator);
        if (!sectionPtr->HasMember(sectionVal)) {
            sectionPtr->AddMember(sectionVal, rapidjson::Value(rapidjson::kObjectType), allocator);
        }
        sectionPtr = &((*sectionPtr)[section.c_str()]);
    }
    
    return *sectionPtr; // Return the targeted section
}

void SaveData::setString(const std::string& section, const std::string& key, const std::string& value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    // Get the target section
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());
    rapidjson::Value jsonValue(value.c_str(), document.GetAllocator());
    
    rapidjson::Value::MemberIterator itr = targetSection.FindMember(jsonKey);
    if (itr != targetSection.MemberEnd()) {
        // If key exists, update
        itr->value.SetString(value.c_str(), document.GetAllocator());
    } else {
        // If key doesn't exist, add
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator());
    }

    // Optionally save changes to the file
    if (doSave) saveDocument();
}

void SaveData::setInt(const std::string& section, const std::string& key, const int value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    // Get the appropriate section or root
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonValue(value);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    // Add or update the value within the targeted section
    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonValue; // Update existing value
    } else {
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator()); // Add a new key-value pair
    }

    // Save the document if required
    if (doSave) saveDocument();
}

void SaveData::setFloat(const std::string& section, const std::string& key, const float value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    // Get the appropriate section or root
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonValue(value);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    // Add or update the value within the targeted section
    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonValue; // Update existing value
    } else {
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator()); // Add a new key-value pair
    }

    // Save the document if required
    if (doSave) saveDocument();
}

void SaveData::setBool(const std::string& section, const std::string& key, const bool value, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    // Get the appropriate section or root
    rapidjson::Value& targetSection = getSection(section);

    rapidjson::Value jsonValue(value);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    // Add or update the value within the targeted section
    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonValue; // Update existing value
    } else {
        targetSection.AddMember(jsonKey, jsonValue, document.GetAllocator()); // Add a new key-value pair
    }

    // Save the document if required
    if (doSave) saveDocument();
}

void SaveData::setTable(const std::string& section, const std::string& key, const luabridge::LuaRef table, const bool doSave) {
    if (!document.IsObject()) document.SetObject();
    
    // Get the appropriate section or root
    rapidjson::Value& targetSection = getSection(section);
    
    rapidjson::Value jsonTable = saveTable(section, table);
    rapidjson::Value jsonKey(key.c_str(), document.GetAllocator());

    // Add or update the value within the targeted section
    if (targetSection.HasMember(jsonKey)) {
        targetSection[jsonKey] = jsonTable; // Update existing value
    } else {
        targetSection.AddMember(jsonKey, jsonTable, document.GetAllocator()); // Add a new key-value pair
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
            // nested table :(
            tableObject.AddMember(jsonKey, saveTable(section, value), allocator);
        }
        else if (value.isString()) {
            rapidjson::Value stringValue(value.cast<std::string>().c_str(), allocator);
            tableObject.AddMember(jsonKey, stringValue, allocator);
        }
        else if (value.isNumber()) {
            std::string number = value.cast<std::string>();
            
            // this is an int
            if(number.find(".") == std::string::npos) {
                setInt(section, key, stoi(number), false);
            }
            // this is a float
            else {
                setFloat(section, key, stof(number), false);
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
            
            // TODO: Fix for deeper sections
            auto& section = getSection(baseSection);
            
            // no member?
            bool hasNoMember = !section.HasMember(key.c_str());
            // if no member, not object?
            bool isObject = hasNoMember || section[key.c_str()].IsObject();
            
            const std::string fullSectionPath = isObject ? baseSection + "." + key : baseSection;
            
            // Note the use of fullSectionPath here to ensure subsection properties are set correctly
            if (value.isString()) {
                setString(fullSectionPath, key, value.cast<std::string>(), doSave);
            }
            else if (value.isNumber()) {
                std::string number = value.cast<std::string>();
                if (number.find(".") == std::string::npos) {
                    setInt(fullSectionPath, key, stoi(number), doSave);
                } else {
                    setFloat(fullSectionPath, key, stof(number), doSave);
                }
            }
            else if (value.isBool()) {
                setBool(fullSectionPath, key, value.cast<bool>(), doSave);
            }
            else if (value.isTable()) {
                // Recursively call saveAll for nested tables
                saveAll(fullSectionPath, value, doSave);
            }
            // Add other data types if necessary
        }
    }

    // Save the document if needed
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
