#include "StringUtils.h"

std::string StringUtils::hexStr(unsigned char* data, int len)
{
	std::stringstream ss;
	ss << std::hex;
	for (int i = 0; i < len; ++i)
		ss << std::setw(2) << std::setfill('0') << (int)data[i];
	return ss.str();
}


std::string StringUtils::UuidToStr(UUID uuid)
{
	std::stringstream ss;
	ss << std::hex << std::setw(sizeof(uuid.Data1) * 2) << std::setfill('0') << uuid.Data1;
	ss << std::hex << std::setw(sizeof(uuid.Data2) * 2) << std::setfill('0') << uuid.Data2;
	ss << std::hex << std::setw(sizeof(uuid.Data3) * 2) << std::setfill('0') << uuid.Data3;
	ss << hexStr(uuid.Data4, sizeof(uuid.Data4));
	return ss.str();
}


UUID StringUtils::StrToUuid(std::string uuid_str)
{
	UUID uuid;
	// add "-" to match the UuidFromString convention
	for (int i = 8;i < 24;i += 5) // TODO handle magic numbers
		uuid_str.insert(i, "-");
	UuidFromStringA((RPC_CSTR)uuid_str.c_str(), &uuid);
	return uuid;
}