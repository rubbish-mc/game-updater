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
#include <sys/stat.h>
//#include "PicoSHA2/picosha2.h"
//#include <zlib.h>
#include "license.h"
#include "dlwebsite.h"

#ifdef _MSC_VER
#pragma comment(lib,"wininet.lib")
#pragma warning(disable:4390)
#pragma warning(disable:4244)
//#define _CRT_SECURE_NO_WARNINGS
#endif

#undef _UNICODE
#undef UNICODE

using std::string;
//using namespace nlohmann;
using std::ifstream;

extern string base_website;
extern const char * json_file;

const std::regex yes_re("^[yY]$"), no_re("^[nN]$");
const string diff_ = "diff", full_ = "full", complete_ = "complete";

std::vector<std::regex> moddiff;
//std::vector<unsigned char> hash(picosha2::k_digest_size);

string get_json_raw() {
	string json_raw = "", str;
#if ((defined _DEBUG) && (defined PRT_PATH))
	std::cout << boost::filesystem::current_path().string() << std::endl;
#endif
	func_download_file(json_file, "tmp.json");
	ifstream fin("tmp.json");
	if (!fin.is_open()); // some error check
	for (; !fin.eof();) {
		std::getline(fin, str);
		json_raw.append(str).append("\n");
	}
	fin.close();
	fin.clear();
	DeleteFile("tmp.json");
	return json_raw;
	//json j = json::parse(json_raw.c_str());
}

int func_download() {
	json json_remote(get_json_raw());
	string full_path = base_website + json_remote.at("info").at("complete").at("file_name").get<string>();
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

void configure() {
	string downloadstr = base_website;
	std::cout << "Checking unzip... " << (!system("unzip >nul 2>nul") ? "yes" : "no") << std::endl;
	if (system("unzip >nul 2>nul")) {
		std::cout << "Downloading unzip from remote...";
		downloadstr.append("unzip.exe");
		func_download_file(downloadstr.c_str(), "unzip.exe");
		std::cout << "  Ok\n";
		downloadstr = base_website;
	}
	std::cout << "Checking sha256sum... " << (!system("sha256sum <nul >nul 2>nul") ? "yes" : "no") << std::endl;
	if (system("sha256sum <nul >nul 2>nul")) {
		std::cout << "Downloading sha256sum from remote...";
		downloadstr.append("sha256sum.zip");
		func_download_file(downloadstr.c_str(), "sha256sum.zip");
		std::cout << "  Ok\nUnpackage sha256sum... ";
		system("unzip -qqo sha256sum.zip");
		std::cout << "Ok\n";
		downloadstr = base_website;
	}
}

const char getRandc() {
	return 'a' + rand()%26;
}

int func_maix() {
	std::cout << "Download remote json... ";
	json json_remote = json::parse(get_json_raw()), json_local;
	std::cout << "Ok\n";
	string tmpstr;
	if (json_remote.at("base_rewrite")) {
		base_website = json_remote.at("new_site").get<string>();
		std::cout << "Detected URL change, change source to " << base_website << std::endl;
		tmpstr = base_website + "game-update.exe";
		if (func_download_file(tmpstr.c_str(), "game-update.new.exe"))
			std::cout << "New executable file downloaded. Auto-update may applyed after program exited.\n";
		//TODO : auto update
	}
	string download_filename, download_loc = base_website, download_type, execute_command = "unzip -oqq ";
	size_t download_size = 0;
	std::ifstream fin("updater.json");
	configure();
	//std::regex re("^[yY]$");
	if (!json_remote["acquire_complete_update"])
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
			json_local = json::parse(tmpstr);
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
		std::cout << "Remote acquire full update, program will download completed package\n" << std::endl;
		download_type = "complete";
	}
	fin.close();

	bool need_download = true;
	download_filename = json_remote.at("info").at(download_type).at("filename").get<string>();
	download_loc.append(json_remote.at("info").at(download_type).at("filename").get<string>());
	download_size = json_remote.at("info").at(download_type).at("size").get<size_t>();
	fin.open(json_remote.at("info").at(download_type).at("filename").get<string>(), std::ios::binary);
	if (fin.is_open()) {
		fin.close();
		std::cout << "Local file found, calculating sha256sum...\n";
		//picosha2::hash256(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>(), hash.begin(), hash.end());
		tmpstr.append("sha256sum ").append(json_remote.at("info").at(download_type).at("filename").get<string>()).append(" >tmp.sum");
		//std::cout << "Calculating sha256sum... ";
		system(tmpstr.c_str());
		fin.open("tmp.sum");
		assert(fin.is_open());
		fin >> tmpstr;
		fin.close();
		boost::filesystem::remove("tmp.sum");
		//tmpstr = picosha2::bytes_to_hex_string(hash.begin(), hash.end());
		std::cout << tmpstr;
		if (tmpstr == json_remote.at("info").at(download_type).at("checksum").get<string>()) {
			std::cout << "... Ok\nHash value matched, jump over the downlad process.\n";
			need_download = false;
		}
		tmpstr.clear();
	}
	fin.close();
	if (need_download) {
		std::cout << "\nNow downloading " << download_type << "\nFile size: " << download_size << " bytes" << std::endl;
		func_download_file(download_loc, json_remote.at("info").at(download_type).at("filename").get<string>().c_str(), download_type == "complete" ? download_size : 0);
	}

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
						std::cerr << "DeleteFile error (" << GetLastError() << ")\nProgram abort\n";
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
			std::cerr << "SetCurrentDirectory error (" << GetLastError() << ")\nProgram abort.\n";
			abort();
		}
		//struct stat info;
		//if (!stat("./mods_backup", &info))
		boost::filesystem::remove_all("./mods_backup");
		if (!MoveFile("./mods", "./mods_backup")) {
			std::cerr << "MoveFile fail (" << GetLastError() << ")\nProgram abort.\n";
		}
		execute_command.append("../").append(download_filename);
	}
	//else if (download_type == "complete")
	else {
		struct stat info;
		if (!stat("./.minecraft_backup", &info)) {
			std::cout << "\nDo you want to delete old backup? [Y/n]: ";
			std::getline(std::cin, tmpstr);
			if (!std::regex_match(tmpstr, yes_re))
				boost::filesystem::remove_all("./.minecraft_backup");
			else {
				srand(time(NULL));
				tmpstr = "./minecraft_backup_";
				for (int i = 0; i < 10; i++)
					tmpstr.append(1, getRandc());
				if (!MoveFile("./.minecraft_backup", tmpstr.c_str())) {
					std::cerr << "MoveFile() fail (" << GetLastError() << ")\nProgram abort.\n";
				}
			}
		}
		if (!MoveFile("./.minecraft", "./.minecraft_backup")) {
			std::cerr << "MoveFile() fail (" << GetLastError() << ")\nProgram abort.\n";
		}
		execute_command.append(download_filename);
		system(execute_command.c_str());
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
	std::cout << "Update completed.\n";
	return 0;
}


void init() {
	std::cout << "Initializating environment variable... ";
	// Set environment at first.
	char * tmpPoint = new char[2000];
	if (!GetEnvironmentVariable("path", tmpPoint, 2000)) {
		std::cerr << "SetEnvironmentVariable (" << GetLastError() << ")\n";
		abort();
	}
	string str = tmpPoint;
	delete[] tmpPoint;
	//std::cout << system("unzip >nul") << std::endl;
	boost::filesystem::path p = boost::filesystem::current_path();
	str.append(";").append(p.string());
	//char test[2000];
	if (!SetEnvironmentVariable("path", str.c_str())) {
		std::cerr << "SetEnvironmentVariable (" << GetLastError() << ")\n";
		abort();
	}
	std::cout << "Ok\n";
}

void func_update() {
	Sleep(5 * 1000);
	MoveFile("game-updater.exe", "game-updater.backup.exe");
	MoveFile("game-updater.new.exe", "game-updater.exe");
}

int main(int argc, char const * argv[])
{
	//system("unzip >nul");
	//std::cout << getenv("PATH");//_putenv("PATH");
	//std::cout << getenv("path") << std::endl;
	//SetCurrentDirectory("..");
	//std::cout << test << std::endl;
	//std::cout << system("unzip >nul") << std::endl;
	//return 0;
	print_agpl_license();
	std::cout << "\n\n";
	init();
	if (argc == 1)
		return func_maix();
	else
		if (argc == 2 and strncmp(argv[1], "--download", 11))
			func_download();
		else
			if (argc == 2 and strncmp(argv[1], "--updater", 10))
				func_update();
	system("pause");
	return 0;
}

