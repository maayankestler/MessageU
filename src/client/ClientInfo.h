#pragma once
#include "AESWrapper.h"
#include "RSAWrapper.h"
#include <Windows.h>

class ClientInfo
{
    // TOD add getters and setters??????
private:
    std::string _username;
    //UUID client_id;
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

    void setPubKey(RSAPublicWrapper* pubkey)
    {
        //_publicKey = new RSAPublicWrapper(pubkey.getPublicKey());
        delete _publicKey; // TODO check if delete is neccery
        _publicKey = pubkey; // TODO check if pointer setter is OK
    }

    RSAPublicWrapper* getPubKey()
    {
        return _publicKey;
    }

    void setSymKey(unsigned char* key)
    {
        _symKey = new AESWrapper(key, AESWrapper::DEFAULT_KEYLENGTH);
    }

    AESWrapper* getSymKey()
    {
        return _symKey;
    }
};