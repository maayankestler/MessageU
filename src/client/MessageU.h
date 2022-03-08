#pragma once
#include <iostream>
#include <map>
#include <sstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

#include "filesutils.h"
#include "StringUtils.h"
#include "Protocol.h"
#include "RSAWrapper.h"
#include "Base64Wrapper.h"
#include "AESWrapper.h"
#include "ClientInfo.h"
#include "Message.h"

#pragma comment(lib, "rpcrt4.lib")

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
        sendFile = 153,
        exitApp = 0
    };

    // help to iterate the userInput options
    static const userInput All[] = {
        userInput::registertion,
        userInput::clientsList,
        userInput::getPubKey,
        userInput::getMessages,
        userInput::sendMessage,
        userInput::requestSymKey,
        userInput::sendSymKey,
        userInput::sendFile,
        userInput::exitApp
    };
}

class MessageU
{
public:
    MessageU();
    static void printMenu();
    Response handleInput(InputEnum::userInput choice); // handle input from the user
    inline static const std::string WELCOME_MESSAGE = "MessageU client at your service.";
    inline static const std::string SERVER_CONFIG_PATH = "server.info";
    inline static const std::string USER_INFO_PATH = "my.info";
    const int MAX_USERNAME_LENGTH = 255;
    static const uint8_t VERSION = 2;

private:
    std::string serverIp;
    int serverPort;
    std::string username;
    UUID client_id;
    RSAPrivateWrapper* privateKey;
    RSAPublicWrapper* publicKey;
    static std::string optionToText(InputEnum::userInput option);
    Response registerUser(std::string userName);
    Response getCLientList();
    Response getPubKey(ClientInfo* user_info);
    Response getMessages();
    Response sendMessage(ClientInfo* user_info, messageType type, std::string content);
    std::map<std::string, ClientInfo*> clients; // map usernames to use data (keys, id, etc)
    std::map<std::string, std::string> clientsIdToUsername; // map the ids to usernames (so the client will work with usernames)
    ClientInfo* getClientInput(); // help the get usrname as input and find the user info
};