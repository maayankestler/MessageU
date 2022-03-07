#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#pragma comment(lib, "ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

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
    int _bytes_amount = 0;
    uint8_t _version{};
    uint16_t _code{};
    uint32_t _payload_size{};
    char* _payload{};
    static const int HEADER_SIZE = sizeof(_version) + sizeof(_code) + sizeof(_payload_size);
    static const int CHUNK_SIZE = 256;
    static Response processResponseHeader(char serverdata[HEADER_SIZE]);
public:
    static Response processResponse(SOCKET ConnectSocket);

    void setPayload(char* payload) {
        _payload = payload;
    }

    char* getPayload()
    {
        return _payload;
    }

    void setPayloadSize(uint32_t size) {
        _payload_size = size;
    }

    uint32_t getPayloadSize()
    {
        return _payload_size;
    }

    void setVersion(uint8_t version) {
        _version = version;
    }

    uint8_t getVersion()
    {
        return _version;
    }

    void setCode(responseCode status)
    {
        _code = uint16_t(status);
    }

    responseCode getCode() 
    {
        return responseCode(_code);
    }
};

class Request
{

private:
    int _bytes_amount = 0;
    UUID _client_id{};
    uint8_t _version{};
    uint16_t _code{};
    uint32_t _payload_size{};
    char* _payload{};
    const int HEADER_SIZE = sizeof(_client_id) + sizeof(_version) + sizeof(_code) + sizeof(_payload_size);
public:
    Request(UUID client_id, uint8_t version, requestCode _code, uint32_t _payload_size, char* _payload = NULL);
    char* getRequestBytes();
    Response sendRequset(std::string ip, int port);

    void setClientId(UUID id) {
        _client_id = id;
    }

    UUID getClientId()
    {
        return _client_id;
    }

    void setPayload(char* payload) {
        delete _payload;
        if (payload == NULL && _payload_size > 0)
        {
            _payload = new char[_payload_size]();
            
        }
        else
        {
            _payload = payload;
        }
    }

    char* getPayload()
    {
        return _payload;
    }

    void setPayloadSize(uint32_t size) {
        _payload_size = size;
    }

    uint32_t getPayloadSize()
    {
        return _payload_size;
    }

    void setVersion(uint8_t version) {
        _version = version;
    }

    uint8_t getVersion()
    {
        return _version;
    }

    void setCode(requestCode status)
    {
        _code = uint16_t(status);
    }

    requestCode getCode()
    {
        return requestCode(_code);
    }
};
