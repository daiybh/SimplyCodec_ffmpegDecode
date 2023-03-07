#pragma once
#include <algorithm>
#include <cstdarg>
#include <iterator>
#include <string>
#include <cstring>
#include <string_view>
#include <vector>
#include <sstream>

inline std::wstring getWString(const char* fmt, ...)
{
	if (fmt == nullptr)
		return {};

	va_list marker;
	char    szBuffer[5 * 1024] = { 0 };

	va_start(marker, fmt);
	vsnprintf(szBuffer, std::size(szBuffer), fmt, marker);
	va_end(marker);

	wchar_t wBuffer[5 * 1024] = { 0 };
#ifdef  _WIN32
	size_t size = 0;
	mbstowcs_s(&size, wBuffer, std::size(wBuffer), szBuffer, strlen(szBuffer));
	return wBuffer;
#else
	mbstowcs(wBuffer, szBuffer, strlen(szBuffer));
	return wBuffer;
#endif
}

inline std::wstring getWString(const wchar_t* fmt, ...)
{
	if (fmt == nullptr)
		return {};

	va_list marker;
	wchar_t szBuffer[5 * 1024] = { 0 };
	va_start(marker, fmt);
	vswprintf(szBuffer, std::size(szBuffer), fmt, marker);
	va_end(marker);
	return szBuffer;
}

inline std::wstring getWString(const std::string& fmt)
{
	wchar_t wBuffer[5 * 1024] = { 0 };
#ifdef  _WIN32
	size_t size = 0;
	mbstowcs_s(&size, wBuffer, std::size(wBuffer), fmt.c_str(), fmt.size());
	return wBuffer;
#else
	mbstowcs(wBuffer, fmt.c_str(), fmt.size());
	return wBuffer;
#endif
}

inline std::string getAString(const wchar_t* fmt, ...)
{
	if (fmt == nullptr)
		return {};

	va_list marker;
	wchar_t szBuffer[5 * 1024] = { 0 };

	va_start(marker, fmt);
	vswprintf(szBuffer, std::size(szBuffer), fmt, marker);
	va_end(marker);

	char buffer[5 * 1024] = { 0 };
#ifdef  _WIN32
	size_t size = 0;
	wcstombs_s(&size, buffer, std::size(buffer), szBuffer, wcslen(szBuffer));
	return buffer;
#else
	wcstombs(buffer, szBuffer, wcslen(szBuffer));
	return buffer;
#endif
}

inline std::string getAString(const char* fmt, ...)
{
	if (fmt == nullptr)
		return {};

	va_list marker;
	char    szBuffer[5 * 1024] = { 0 };
	va_start(marker, fmt);
	vsnprintf(szBuffer, std::size(szBuffer), fmt, marker);
	va_end(marker);
	return szBuffer;
}

inline std::string getAString(const std::wstring& fmt)
{
	char buffer[5 * 1024] = { 0 };
#ifdef  _WIN32
	size_t size = 0;
	wcstombs_s(&size, buffer, std::size(buffer), fmt.c_str(), fmt.size());
	return buffer;
#else
	wcstombs(buffer, fmt.c_str(), fmt.size());
	return buffer;
#endif
}

inline std::string lowerString(const std::string& str)
{
	std::string result;
	std::transform(str.begin(), str.end(), std::back_inserter(result), [](auto& item) { return tolower(item); });
	return result;
}

inline std::wstring lowerString(const std::wstring& str)
{
	std::wstring result;
	std::transform(str.begin(), str.end(), std::back_inserter(result), [](auto& item) { return towlower(item); });
	return result;
}

inline std::string upperString(const std::string& str)
{
	std::string result;
	std::transform(str.begin(), str.end(), std::back_inserter(result), [](auto& item) { return toupper(item); });
	return result;
}

inline std::wstring upperString(const std::wstring& str)
{
	std::wstring result;
	std::transform(str.begin(), str.end(), std::back_inserter(result), [](auto& item) { return towupper(item); });
	return result;
}

inline std::vector<std::string> splitString(const std::string& s, char delim)
{
	if (s.empty())
		return {};

	std::vector<std::string> elems;
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim))
		elems.push_back(item);
	return elems;
}