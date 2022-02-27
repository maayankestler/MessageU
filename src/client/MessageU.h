#pragma once
#include <iostream>
#include "filesutils.h"
#include "Protocol.h"
#include "RSAWrapper.h"

const std::string WELCOME_MESSAGE = "MessageU client at your service.";
const std::string SERVER_CONFIG_PATH = "server.info";
const std::string USER_INFO_PATH = "my.info";
const uint8_t VERSION = 1;

namespace InputEnum
{
    enum userInput {
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
        registertion,
        clientsList,
        getPubKey,
        getMessages,
        sendMessage,
        requestSymKey,
        sendSymKey,
        exitApp 
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
    RSAPrivateWrapper* privateKey;
    RSAPublicWrapper* publicKey;
    static std::string optionToText(InputEnum::userInput option);
    Response registerUser(std::string userName);
    Response getCLientList();
    Response getPubKey();
    Response getMessages();
    Response sendMessage();
    Response requestSymKey();
    Response sendSymKey();
};