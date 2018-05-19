#pragma once
#include "nlohmann/json.hpp"
#include <string>

class json : public nlohmann::json {
public:
	json() = default;
	json(const std::string &);
	json & parse_str(const std::string &);
};
