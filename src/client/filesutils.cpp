#include "filesutils.h"

std::string fileToString(const std::string filename)
{
    std::ifstream in(filename);
    if (!in.good()) {
        throw std::exception("File not found");
    }
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

bool fileExist(const std::string filename)
{
    std::ifstream file(filename);
    return file.good();
}

//Response save_file(Request req)
//{
//    Response resp;
//    resp.filename = req.filename;
//    resp.name_len = req.name_len;
//
//    std::string path = PATH;
//    try {
//        if (!validate_path_var(std::to_string(req.user_id)))
//        {
//            std::cout << "got forbidden user_id " << std::to_string(req.user_id) << std::endl;
//            resp.setStatus(responseStatus::generalError);
//            return resp;
//        }
//        path += std::to_string(req.user_id) + "\\";
//        std::filesystem::create_directory(path);
//        if (!validate_path_var(std::string(req.filename)))
//        {
//            std::cout << "got forbidden filename " << std::string(req.filename) << std::endl;
//            resp.setStatus(responseStatus::generalError);
//            return resp;
//        }
//        path += req.filename;
//
//        auto myfile = std::fstream(path, std::ios::out | std::ios::binary);
//        myfile.write((char*)&req.payload[0], req.size);
//        myfile.close();
//
//        resp.setStatus(responseStatus::fileActionDone);
//    }
//    catch (...) {
//        resp.setStatus(responseStatus::generalError);
//    }
//
//    return resp;
//}
//
//Response delete_file(Request req)
//{
//    Response resp;
//    resp.filename = req.filename;
//    resp.name_len = req.name_len;
//    std::string path = PATH;
//    try {
//        if (!validate_path_var(std::to_string(req.user_id)))
//        {
//            std::cout << "got forbidden user_id " << std::to_string(req.user_id) << std::endl;
//            resp.setStatus(responseStatus::generalError);
//            return resp;
//        }
//        path += std::to_string(req.user_id) + "\\";
//        if (!std::filesystem::exists(path))
//        {
//            resp.setStatus(responseStatus::noFiles);
//        }
//        else
//        {
//            if (!validate_path_var(std::string(req.filename)))
//            {
//                std::cout << "got forbidden filename " << std::string(req.filename) << std::endl;
//                resp.setStatus(responseStatus::generalError);
//                return resp;
//            }
//            path += req.filename;
//            if (!std::filesystem::exists(path))
//            {
//                resp.setStatus(responseStatus::fileNotFound);
//            }
//            else
//            {
//                std::remove(path.c_str());
//                resp.setStatus(responseStatus::fileActionDone);
//            }
//        }
//    }
//    catch (...) {
//        resp.setStatus(responseStatus::generalError);
//    }
//
//    return resp;
//}
//
//Response return_file(Request req)
//{
//    Response resp;
//    resp.filename = req.filename;
//    resp.name_len = req.name_len;
//
//    try {
//        std::string path = PATH;
//        if (!validate_path_var(std::to_string(req.user_id)))
//        {
//            std::cout << "got forbidden user_id " << std::to_string(req.user_id) << std::endl;
//            resp.setStatus(responseStatus::generalError);
//            return resp;
//        }
//        path += std::to_string(req.user_id) + "\\";
//        if (!std::filesystem::exists(path))
//        {
//            resp.setStatus(responseStatus::noFiles);
//        }
//        else
//        {
//            if (!validate_path_var(std::string(req.filename)))
//            {
//                std::cout << "got forbidden filename " << std::string(req.filename) << std::endl;
//                resp.setStatus(responseStatus::generalError);
//                return resp;
//            }
//            path += req.filename;
//            if (!std::filesystem::exists(path))
//            {
//                resp.setStatus(responseStatus::fileNotFound);
//            }
//            else
//            {
//                //open file
//                std::ifstream infile(path);
//
//                //get length of file
//                infile.seekg(0, std::ios::end);
//                resp.size = infile.tellg();
//                infile.seekg(0, std::ios::beg);
//
//                resp.payload = new char[resp.size + 1];
//                //read file
//                infile.read(resp.payload, resp.size);
//                infile.close();
//
//                resp.setStatus(responseStatus::fileFound);
//            }
//        }
//    }
//    catch (...) {
//        resp.setStatus(responseStatus::generalError);
//    }
//
//    return resp;
//}
//
//Response list_files(Request req)
//{
//    Response resp;
//    std::string path = PATH;
//    if (!validate_path_var(std::to_string(req.user_id)))
//    {
//        std::cout << "got forbidden user_id " << std::to_string(req.user_id) << std::endl;
//        resp.setStatus(responseStatus::generalError);
//        return resp;
//    }
//    path += std::to_string(req.user_id);
//    try {
//        if (!std::filesystem::exists(path))
//        {
//            resp.setStatus(responseStatus::noFiles);
//        }
//        else
//        {
//            std::vector<std::string> files;
//            for (const auto& entry : std::filesystem::directory_iterator(path))
//            {
//                files.push_back(entry.path().string());
//            }
//
//            if (files.size() == 0)
//            {
//                resp.setStatus(responseStatus::noFiles);
//            }
//            else
//            {
//                resp.setStatus(responseStatus::filesFound);
//
//                std::string name = random_string(32);
//                resp.name_len = name.length();
//                resp.filename = new char[resp.name_len + 1];
//                strncpy_s(resp.filename, resp.name_len + 1, name.c_str(), resp.name_len);
//
//                std::string s;
//                for (std::string file_name : files) s += file_name + "\n";
//                s.pop_back(); // removing last \n
//
//                resp.size = s.length();
//                resp.payload = new char[resp.size + 1];
//                strncpy_s(resp.payload, resp.size + 1, s.c_str(), resp.size);
//            }
//        }
//    }
//    catch (...) {
//        resp.setStatus(responseStatus::generalError);
//    }
//
//    return resp;
//}

//bool validate_path_var(std::string path_var)
//{
//    std::string blacklist[2] = { "..", "%2e" };
//    for (std::string bs : blacklist)
//    {
//        if (path_var.find(bs) != std::string::npos)
//        {
//            return false;
//        }
//    }
//    return true;
//}

//std::string random_string(size_t length)
//{
//    auto randchar = []() -> char
//    {
//        const char charset[] =
//            "0123456789"
//            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//            "abcdefghijklmnopqrstuvwxyz";
//        const size_t max_index = (sizeof(charset) - 1);
//        return charset[rand() % max_index];
//    };
//    std::string str(length, 0);
//    std::generate_n(str.begin(), length, randchar);
//    return str;
//}