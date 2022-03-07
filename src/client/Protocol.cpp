#include "protocol.h"

Request::Request(UUID client_id, uint8_t version, requestCode _code, uint32_t _payload_size, char* _payload)
{
    setClientId(client_id);
    setVersion(version);
    setCode(_code);
    setPayloadSize(_payload_size);
    setPayload(_payload);
}

char* Request::getRequestBytes()
{
    char* request_data = new char[MAX_LENGTH];
    _bytes_amount = 0;

    if (request_data)
    {
        std::memcpy(&request_data[_bytes_amount], &_client_id, sizeof(_client_id));
        _bytes_amount += sizeof(_client_id);
        std::memcpy(&request_data[_bytes_amount], &_version, sizeof(_version));
        _bytes_amount += sizeof(_version);
        std::memcpy(&request_data[_bytes_amount], &_code, sizeof(_code));
        _bytes_amount += sizeof(_code);
        std::memcpy(&request_data[_bytes_amount], &_payload_size, sizeof(_payload_size));
        _bytes_amount += sizeof(_payload_size);
        if (_payload_size != NULL)
        {
            std::memcpy(&request_data[_bytes_amount], _payload, _payload_size);
            _bytes_amount += _payload_size;
        }
    }

    return request_data;
}

Response Request::sendRequset(std::string ip, int port)
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        std::cout << "WSAStartup failed with error: " << iResult << std::endl;
        // TODO return null or something
    }
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
    }
    char* request_bytes = getRequestBytes();
    struct sockaddr_in saServer;
    saServer.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &saServer.sin_addr.s_addr);
    saServer.sin_port = htons(port);
    iResult = connect(ConnectSocket, (SOCKADDR*)&saServer, sizeof(saServer));
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    iResult = send(ConnectSocket, request_bytes, _bytes_amount, 0);
    delete request_bytes;
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
    }
    //iResult = shutdown(ConnectSocket, SD_SEND);
    //if (iResult == SOCKET_ERROR) {
    //    printf("shutdown failed with error: %d\n", WSAGetLastError());
    //    closesocket(ConnectSocket);
    //    WSACleanup();
    //}
    char* response_bytes = new char[MAX_LENGTH];
    iResult = recv(ConnectSocket, response_bytes, MAX_LENGTH, 0);
    closesocket(ConnectSocket);
    WSACleanup();
    return Response::processResponse(response_bytes, MAX_LENGTH);
}

Response Response::processResponse(char* serverdata, int length)
{
    //Response resp = Response(); // TODO check {0}
    Response(resp); // TODO check if valid
    resp._bytes_amount = 0;
    std::memcpy(&resp._version, &serverdata[resp._bytes_amount], sizeof(resp._version));
    resp._bytes_amount += sizeof(resp._version);
    std::memcpy(&resp._code, &serverdata[resp._bytes_amount], sizeof(resp._code));
    resp._bytes_amount += sizeof(resp._code);
    if (length > resp._bytes_amount)
    {
        std::memcpy(&resp._payload_size, &serverdata[resp._bytes_amount], sizeof(resp._payload_size));
        resp._bytes_amount += sizeof(resp._payload_size);
        resp._payload = new char[resp._payload_size + 1];
        std::memcpy(resp._payload, &serverdata[resp._bytes_amount], resp._payload_size);
        resp._bytes_amount += resp._payload_size;
    }
    delete serverdata;
    return resp;
}