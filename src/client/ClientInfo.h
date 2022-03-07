#pragma once
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include <Windows.h>

class ClientInfo
{
private:
    std::string _username;
    UUID _id;
    RSAPublicWrapper* _publicKey;
    AESWrapper* _symKey;
public:
    void setUserName(std::string username)
    {
        _username = username;
    }

    std::string getUserName()
    {
        return _username;
    }

    void setId(UUID id)
    {
        _id = id;
    }

    UUID getId()
    {
        return _id;
    }

    void setPubKey(RSAPublicWrapper* pubkey)
    {
        delete _publicKey;
        _publicKey = pubkey;
    }

    RSAPublicWrapper* getPubKey()
    {
        return _publicKey;
    }

    void setSymKey(unsigned char* key)
    {
        delete _symKey;
        _symKey = new AESWrapper(key, AESWrapper::DEFAULT_KEYLENGTH);
    }

    AESWrapper* getSymKey()
    {
        return _symKey;
    }
};