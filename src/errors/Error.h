#pragma once

// std library
#include <string>
#include <iostream>

// Probably wanna override std::exceptions later
class Error
{
public:
	static void error(const std::string message) {
		std::cout << "error: " + message;
		exit(0);
	}
};

