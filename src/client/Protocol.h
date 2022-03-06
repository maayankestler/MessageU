#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

const int MAX_LENGTH = 1024 * 1024 * 100;
const int MAX_USERNAME_LENGTH = 255;

enum class responseCode {
    registertion = 2100,
    usersList = 2101,
    getPubKey = 2102,
    sendMessage = 2103,
    getMessages = 2104,
    generalError = 9000
};

enum class requestCode {
    registertion = 1100,
    users_list = 1101,
    getPubKey = 1102,
    sendMessage = 1103,
    getMessages = 1104
};

class Response
{
private:
    int bytes_amount = 0;
    // TOD add getters and setters??????
public:
    uint8_t version{};
    uint16_t code{};
    uint32_t payload_size{};
    char* payload{};

    //char* getResponseBytes();

    static Response processResponse(char clientdata[], int length);

    void setCode(responseCode status)
    {
        this->code = uint16_t(status);
    }
};

class Request
{
    // TOD add getters and setters??????
public:
    UUID client_id{};
    uint8_t version{};
    uint16_t code{};
    uint32_t payload_size{};
    char* payload{};
    char* getRequestBytes();
    Response sendRequset(std::string ip, int port);

private:
    int bytes_amount = 0;
};
