#pragma once

#include <nlohmann/json.hpp>
#include <iostream>
#include "global.hpp"

/* https://github.com/nlohmann/json */

using json = nlohmann::json;

class Json {
public:

	json static string_to_document(std::string string) {

		json j = string.c_str();
		return j;
	}

	json static pathfile_to_document(std::string path) {
		return string_to_document(ReadFile(path.c_str()).c_str());
	}
};


