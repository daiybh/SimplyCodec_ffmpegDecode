#pragma once
#include <cstdint>
#include <string>

inline std::string ipToString(uint32_t _ip)
{
	char ip[std::size("255.255.255.255")];
	snprintf(ip, std::size(ip), "%03u.%03u.%03u.%03u", _ip & 0xff, (_ip >> 8) & 0xff, (_ip >> 16) & 0xff, (_ip >> 24) & 0xff);
	return ip;
}

inline std::wstring ipToWString(uint32_t _ip)
{
	wchar_t ip[std::size("255.255.255.255")];
	swprintf(ip, std::size(ip), L"%03u.%03u.%03u.%03u", _ip & 0xff, (_ip >> 8) & 0xff, (_ip >> 16) & 0xff, (_ip >> 24) & 0xff);
	return ip;
}
