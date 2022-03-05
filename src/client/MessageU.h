#pragma once
#include <iostream>
#include "filesutils.h"
#include "Protocol.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"

#pragma comment(lib, "rpcrt4.lib")

const std::string WELCOME_MESSAGE = "MessageU client at your service.";
const std::string SERVER_CONFIG_PATH = "server.info";
const std::string USER_INFO_PATH = "my.info";
const uint8_t VERSION = 1;

namespace InputEnum
{
    enum class userInput {
        registertion = 110,
        clientsList = 120,
        getPubKey = 130,
        getMessages = 140,
        sendMessage = 150,
        requestSymKey = 151,
        sendSymKey = 152,
        exitApp = 0
    };

    static const userInput All[] = { 
        userInput::registertion,
        userInput::clientsList,
        userInput::getPubKey,
        userInput::getMessages,
        userInput::sendMessage,
        userInput::requestSymKey,
        userInput::sendSymKey,
        userInput::exitApp
    };
}

class MessageU
{
public:
    MessageU();
    static void printMenu();
    Response handleInput(InputEnum::userInput choice);

private:
    std::string serverIp;
    int serverPort;
    std::string username;
    UUID client_id;
    RSAPrivateWrapper* privateKey;
    RSAPublicWrapper* publicKey;
    static std::string optionToText(InputEnum::userInput option);
    static std::string hexStr(unsigned char* data, int len);
    static std::string UuidStr(UUID uuid);
    Response registerUser(std::string userName);
    Response getCLientList();
    Response getPubKey();
    Response getMessages();
    Response sendMessage();
    Response requestSymKey();
    Response sendSymKey();
};