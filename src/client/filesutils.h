#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <string>
#include <algorithm>
#include "protocol.h"

std::string fileToString(const std::string filename);
bool fileExist(const std::string filename);

//Response save_file(Request req);
//
//Response delete_file(Request req);
//
//Response return_file(Request req);
//
//Response list_files(Request req);
//
//bool validate_path_var(std::string);
//
//std::string random_string(size_t length);