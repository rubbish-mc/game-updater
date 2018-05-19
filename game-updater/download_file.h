#pragma once
#include <string>
#include "json.h"
using std::string;

bool func_download_file(const string &, json::value_type, size_t = 0);
bool func_download_file(const char *, const char *, size_t = 0);