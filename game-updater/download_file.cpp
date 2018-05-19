#include "stdafx.h"
#include "download_file.h"
#include <windows.h>
#include <WinInet.h>
#include <iostream>

bool func_download_file(const string & str, json::value_type jv, size_t filesz) {
	string tmp = jv;
	return func_download_file(str.c_str(), tmp.c_str(), filesz);
}

bool func_download_file(const char * url, const char * file, size_t filesz) {
	const size_t dwBuffer = 2048;		//buffer size
	HINTERNET hWeb, hRequest;		//Internet request handle
	DWORD dwReadByte = 0, dwReserved;		//read byte count
	char * szBuffer = new char[dwBuffer];		//read buff
	memset(szBuffer, 0, dwBuffer * sizeof(char));
	HANDLE hdFile = INVALID_HANDLE_VALUE;	//file handle
	if (!(hWeb = InternetOpen("Mozilla/5.0 (Windows NT 10.0; Win64; x64)",
		INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0))) {
		delete[] szBuffer;
		std::cerr << "func_download file failed(" << GetLastError() << ") in InternetOpen(), url: " << url << ", filename: " << file << std::endl;
		return false;
	}
	if (!(hRequest = InternetOpenUrl(hWeb, url, nullptr, 0, INTERNET_FLAG_DONT_CACHE, 0))) {
		InternetCloseHandle(hWeb);
		delete[] szBuffer;
		std::cerr << "func_download file failed(" << GetLastError() << ") in InternetOpenUrl(), url: " << url << ", filename: " << file << std::endl;
		return false;
	}
	if ((hdFile = CreateFile(file, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr)) == INVALID_HANDLE_VALUE) {
		InternetCloseHandle(hWeb);
		InternetCloseHandle(hRequest);
		delete[] szBuffer;
		std::cerr << "func_download file failed(" << GetLastError() << ") in CreateFile(), url: " << url << ", filename: " << file << std::endl;
		return false;
	}
	while (InternetReadFile(hRequest, (PVOID)szBuffer, dwBuffer, &dwReadByte) && dwReadByte)
		WriteFile(hdFile, szBuffer, dwReadByte, &dwReserved, nullptr);
	CloseHandle(hdFile);
	InternetCloseHandle(hRequest);
	InternetCloseHandle(hWeb);
	delete[] szBuffer;
	return true;
}
