#pragma once
#include <iostream>
#include <sstream>
#include <iomanip>
#include <Windows.h>

#pragma comment(lib, "rpcrt4.lib")

class StringUtils
{
public:
	static std::string hexStr(unsigned char* data, int len);
	static std::string UuidToStr(UUID uuid);
	static UUID StrToUuid(std::string uuid_str);
};