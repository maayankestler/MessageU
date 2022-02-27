#pragma once
#include <iostream>
#include <sstream>
#include <fstream>

const int MAX_LENGTH = 1024 * 1024 * 100;
const int MAX_USERNAME_LENGTH = 255;
const int PUBLIC_KEY_SIZE = 160;

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

class Request
{
public:
    uint16_t client_id;
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;
    char* payload;

    char* getRequestBytes();

private:
    int bytes_amount = 0;
};

class Response
{
private:
    int bytes_amount = 0;

public:
    uint8_t version;
    uint16_t code;
    uint32_t payload_size;
    char* payload;

    //char* getResponseBytes();

    static Response processResponse(char clientdata[], int length);

    void setCode(responseCode status)
    {
        this->code = uint16_t(status);
    }
};
