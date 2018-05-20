// game-updater.cpp : Defines the entry point for the console application.
//
/*
	Copyright (C) 2018 Too-Naive

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program. If not, see <https://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

#include <iostream>
#include <string>
#include <windows.h>
#include <tchar.h>
#include <fstream>
#include <regex>
#include <boost/filesystem.hpp>
#include <vector>
#include <algorithm>
#include "download_file.h"
//#include <zlib.h>


#ifdef _MSC_VER
#pragma comment(lib,"wininet.lib")
#pragma warning(disable:4390)
//#define _CRT_SECURE_NO_WARNINGS
#endif

#undef _UNICODE
#undef UNICODE

using std::string;
//using namespace nlohmann;
using std::ifstream;

const char * base_website = "https://example.com/mcdownload/";
const char * json_file = "https://example.com/mcdownload/LATEST.json";


const std::regex yes_re("^[yY]$"), no_re("^[nN]$");
const string diff_ = "diff", full_ = "full", complete_ = "complete";

std::vector<std::regex> moddiff;

string get_json_raw() {
	string json_raw = "", str;
	func_download_file(json_file, "tmp.json");
	ifstream fin("tmp.json");
	if (!fin.is_open()); // some error check
	for (; !fin.eof();) {
		std::getline(fin, str);
		json_raw += str;
	}
	fin.close();
	fin.clear();
	DeleteFile("tmp.json");
	return json_raw;
	//json j = json::parse(json_raw.c_str());
}

int func_download() {
	json json_remote(get_json_raw());
	string full_path = base_website + json_remote["info"]["complete"]["file_name"];
	string download_name = json_remote["info"]["complete"]["file_name"];
	func_download_file(full_path.c_str(), download_name.c_str());
	std::cout << json_remote["info"]["full"]["complete"] << " Downloaded." << std::endl;
	return 0;
}

bool get_ans(const std::regex & regex_type, std::istream & is=std::cin) {
	string str;
	std::getline(is, str);
	return (bool)std::regex_match(str, regex_type);
}

int func_maix() {
	json json_remote = json(get_json_raw()), json_local;
	string download_filename, download_loc, download_type, execute_command = "unzip -qqo ", tmpstr;
	size_t download_size = 0;
	std::ifstream fin("updater.json");
	//std::regex re("^[yY]$");
	std::cout << "Checking unzip: " << (!system("unzip >nul") ? "yes" : "no") << std::endl;
	if (!system("unzip >nul 2>nul")) {
		std::cout << "Downloading unzip from remote...";
		download_loc = base_website;
		download_loc.append("unzip.exe");
		func_download_file(download_loc.c_str(), "unzip.exe");
	}
	if (json_remote["acquire_complete_update"] != true)
		if (!fin.is_open()) {
			// configure file not foud, maybe is first start
			std::cout << "Configure file not found, would you like apply mods update? [Y/n]";
			//std::getline(std::cin, str);
			//if (std::regex_match(str, re)) {
			if (get_ans(yes_re)){
				// comfirmed
				std::cout << "Have you already apply pervious mods update? [Y/n]";
				//std::getline(std::cin, str);
				//if (std::regex_match(str, re)) {
				if (get_ans(yes_re))
					// confirmed
					// Pervious mods update applyed, using diff to update mods
					//download_loc = base_website + json_remote["info"]["diff"]["filename"];
					//download_size = json_remote["info"]["diff"]["size"];
					download_type = "diff";
			
				else 
					// canceled
					// get full mod update
					//download_loc = base_website + json_remote["info"]["full"]["filename"];
					//download_size = json_remote["info"]["full"]["size"];
					download_type = "full";
			
				//std::cout << "Download size: " << json_remote[""];
			}
			else 
				//download_loc = base_website + json_remote["info"]["complete"]["filename"];
				//download_size = json_remote["info"]["complete"]["size"];
				download_type = "complete";
		}
		else {
			while (!fin.eof()) {
				string str;
				std::getline(fin, str);
				tmpstr.append(str).append("\n");
			}
			json_local.parse_str(tmpstr);
			tmpstr.clear();
			// file opened
			if (json_remote["version"]["major"] == json_local["version"]["major"])
				if (int(json_remote["version"]["minor"]) - int(json_local["version"]["minor"]) == 1)
					// apply mod diff
					download_type = "diff";
				else
					download_type = "full";
			else
				download_type = "complete";
		}
	else {
		std::cout << "Remote acquire full update, program will download completed package" << std::endl;
		download_type = "complete";
	}
	download_filename = "" + json_remote["info"][download_type]["filename"];
	download_loc = base_website + json_remote["info"][download_type]["filename"];
	download_size = json_remote["info"][download_type]["size"];
	std::cout << "\nNow downloading " << download_type << "\nFile size: " << download_size << " bytes" << std::endl;
	func_download_file(download_loc, json_remote["info"][download_type]["filename"], download_type == "complete" ? download_size : 0);

	if (download_type == "diff") {
		// process diff action
		/*char * tmp_path = new char[MAX_PATH];
		if (!GetCurrentDirectory(MAX_PATH, tmp_path)) {
			std::cerr << "GetCurrentDirectory error (" << GetLastError() << ")\nProgram abort.";
			abort();
		}
		string target_path = tmp_path;
		delete[] tmp_path;
		target_path.append("/.minecraft/mods");*/
		//if (!SetCurrentDirectory(target_path.c_str())) {
		// Finding and apply different
		if (!SetCurrentDirectory("./.minecraft/mods")) {
			std::cerr << "SetCurrentDirectory error (" << GetLastError() << ")\nProgram abort.";
			abort();
		}
		boost::filesystem::path current_dir(".");
		for (string modname : json_remote["diff"])
#ifdef _DEBUG
		{
			std::cout << modname << std::endl;
#endif
			if (modname[0] == '-') {
				modname.erase(modname.begin(), modname.begin() + 1);
				moddiff.push_back(std::regex(modname));
			}
#ifdef _DEBUG
		}
#endif
		for (boost::filesystem::recursive_directory_iterator iter(current_dir), end; iter != end && !moddiff.empty(); iter++) {
			string filename = iter->path().filename().string();
			//for (std::regex r : moddiff)
			for (std::vector<std::regex>::iterator it = moddiff.begin(); it != moddiff.end(); it++) {
				std::regex r = *it;
				if (std::regex_search(filename, r)) {
					if (!DeleteFile(filename.c_str())) {
						std::cerr << "DeleteFile error (" << GetLastError() << ")\nProgram will now abort()";
						abort();
					}
					std::cout << "Deleted " << filename << std::endl;
					// delete regular expression after use
					//moddiff.erase(std::find(moddiff.begin(), moddiff.end(), r));
					moddiff.erase(it);
					break;
				}
			}
		}
		execute_command.append("../../").append(download_filename);
	} // diff action end
	else if (download_type == "full") {
		// process full mode.
		if (!SetCurrentDirectory("./.minecraft")) {
			std::cerr << "SetCurrentDirectory error (" << GetLastError() << ")\nProgram abort.";
			abort();
		}
		execute_command.append("../").append(download_filename);
	}
	//else if (download_type == "complete")
	else {
		if (!MoveFile("./.minecraft", "./.minecraft_backup")) {
			std::cerr << "MoveFile() fail (" << GetLastError() << ")\nProgram abort.";
		}
		execute_command.append(download_filename);
		system(execute_command.c_str());
		if (!MoveFile("./.minecraft", "./.minecraft_backup")) {
			std::cerr << "MoveFile() fail (" << GetLastError() << ")\nProgram abort.";
		}
	}
	if (download_type != "complete")
		system(execute_command.c_str());
	json_local["version"]["major"] = json_remote["version"]["major"];
	json_local["version"]["minor"] = json_remote["version"]["minor"];
	// dump local json finaly
	std::ofstream fout("updater.json");
	fout << json_local.dump();
	//fout.flush();
	fout.close();
	return 0;
}


void init() {
	// Set environment at first.
	char * tmpPoint = new char[2000];
	if (!GetEnvironmentVariable("path", tmpPoint, 2000)) {
		std::cerr << "SetEnvironmentVariable (" << GetLastError() << ")\n";
		abort();
	}
	string str = tmpPoint;
	delete[] tmpPoint;
	//std::cout << system("unzip >nul") << std::endl;
	boost::filesystem::path p = boost::filesystem::current_path();;
	str.append(";").append(p.string());
	//char test[2000];
	if (!SetEnvironmentVariable("path", str.c_str())) {
		std::cerr << "SetEnvironmentVariable (" << GetLastError() << ")\n";
		abort();
	}
}

int main(int argc, char const * argv[])
{
	//system("unzip >nul");
	//std::cout << getenv("PATH");//_putenv("PATH");
	//std::cout << getenv("path") << std::endl;
	//SetCurrentDirectory("..");
	//std::cout << test << std::endl;
	//std::cout << system("unzip >nul") << std::endl;
	return 0;
	init();
	if (argc == 1)
		return func_maix();
	else
		if (argc == 2 and strncmp(argv[1], "--download", 10))
			func_download();
	return 0;
}

