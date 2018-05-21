#pragma once
#include <string>
#include "nlohmann/json.hpp"
using std::string;

using namespace nlohmann;

bool func_download_file(const string &, json::value_type, size_t = 0);
bool func_download_file(const char *, const char *, size_t = 0);