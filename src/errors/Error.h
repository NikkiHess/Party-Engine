#pragma once

// std library
#include <iostream>
#include <string>

// Probably wanna override std::exceptions later
class Error
{
public:
	static void error(const std::string message) {
		std::cout << "error: " + message;
		exit(0);
	}
};
