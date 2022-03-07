#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>
#include <algorithm>
#include "protocol.h"


class FileUtils
{
public:
	static std::string fileToString(const std::string filename);
	static void stringToFile(const std::string file_content, std::string path);
	static bool fileExist(const std::string filename);
};
