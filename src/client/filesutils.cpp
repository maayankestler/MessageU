#include "filesutils.h"

std::string FileUtils::fileToString(const std::string filename)
{
    std::ifstream in(filename);
    if (!in.good()) {
        throw std::exception("File not found");
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool FileUtils::fileExist(const std::string filename)
{
    std::ifstream file(filename);
    return file.good();
}

void FileUtils::stringToFile(const std::string file_content, std::string path)
{
    std::fstream myfile = std::fstream(path, std::ios::out | std::ios::binary);
    myfile.write(file_content.c_str(), file_content.length());
    myfile.close();
}
