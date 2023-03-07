#pragma once
#include "stringHelper.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

inline std::string getAppPathA()
{
#ifdef _WIN32
	char strModuleFileName[MAX_PATH];
	char strDriver[MAX_PATH];
	char strPath[MAX_PATH];
	GetModuleFileNameA(nullptr, strModuleFileName, MAX_PATH);
	_splitpath_s(strModuleFileName, strDriver, std::size(strDriver), strPath, std::size(strPath), nullptr, 0, nullptr, 0);
	strcat_s(strDriver, std::size(strDriver), strPath);
	return strDriver;
#else
	char* buffer = nullptr;
	if ((buffer = getcwd(nullptr, 0)) == nullptr)
		return std::string();
	std::string path(buffer, strlen(buffer));
	free(buffer);
	return path;
#endif
}

inline std::wstring getAppPathW()
{
	return getWString(getAppPathA());
}
