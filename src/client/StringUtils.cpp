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
	// add "-" to seperte the hex groups and match the UuidFromString convention
	int bytes_to_hex_ratio = CHAR_BIT / 4; // one bye have CHAR_BIT bits and each hex char is 4 bits
	int first_group_len = sizeof(uuid.Data1) * bytes_to_hex_ratio;
	int midlle_groups_len = sizeof(uuid.Data2) * bytes_to_hex_ratio;
	for (int i = first_group_len;i < first_group_len + 4 * midlle_groups_len;i += midlle_groups_len + 1)
		uuid_str.insert(i, "-");
	UuidFromStringA((RPC_CSTR)uuid_str.c_str(), &uuid);
	return uuid;
}
