#pragma once

// std library
#include <string>

class StringUtils
{
public:
	// Thank you, EECS 498 Staff :)
	// Returns the word after a phrase, skipping spaces
	static std::string getWordAfterPhrase(const std::string& input, const std::string& phrase) {
		// Find the position of the phrase in the string
		size_t pos = input.find(phrase);

		// If phrase is not found, return an empty string
		if (pos == std::string::npos) return "";

		// Find the starting position of the next word (skip spaces)
		pos += phrase.length();
		while (pos < input.size() && std::isspace(input[pos])) {
			++pos;
		}

		// If we're at the end of the string, return an empty string
		if (pos == input.size()) return "";

		// Find the end pos of the word (until a space or end of string)
		size_t endPos = pos;
		while (endPos < input.size() && !std::isspace(input[endPos])) {
			++endPos;
		}

		// Extract and return the word
		return input.substr(pos, endPos - pos);
	}
};

