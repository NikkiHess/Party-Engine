#pragma once

// std library
#include <string>

// Forward declaration for Actor
class Actor;

// DON'T FORGET PARITY TO LuaUtils.cpp
class CppComponent {
public:
	std::string key = "";
	bool enabled = true;
	Actor* actor = nullptr;

	virtual ~CppComponent() = default;
};