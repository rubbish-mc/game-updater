#include "stdafx.h"
#include "json.h"

json::json(const std::string & str) {
	*this = this->parse_str(str);
}

json & json::parse_str(const std::string & json_raw) {
	*this = nlohmann::json::parse(json_raw);
	return *this;
}