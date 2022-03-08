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
    char* request_data = new char[HEADER_SIZE + _payload_size];
    _bytes_amount = 0;

    if (request_data)
    {
        // copy the vars bytes
        std::memcpy(&request_data[_bytes_amount], &_client_id, sizeof(_client_id));
        _bytes_amount += sizeof(_client_id);
        std::memcpy(&request_data[_bytes_amount], &_version, sizeof(_version));
        _bytes_amount += sizeof(_version);
        std::memcpy(&request_data[_bytes_amount], &_code, sizeof(_code));
        _bytes_amount += sizeof(_code);
        std::memcpy(&request_data[_bytes_amount], &_payload_size, sizeof(_payload_size));
        _bytes_amount += sizeof(_payload_size);
        // validate that there is a payload
        if (_payload_size != 0)
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
        std::string err_msg = "WSAStartup failed with error: " + iResult;
        throw std::exception(err_msg.c_str());
    }
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // create socket
    if (ConnectSocket == INVALID_SOCKET) {
        std::string err_msg = "Error at socket(): " + WSAGetLastError();
        throw std::exception(err_msg.c_str());
    }
    struct sockaddr_in saServer;
    saServer.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &saServer.sin_addr.s_addr);
    saServer.sin_port = htons(port);
    iResult = connect(ConnectSocket, (SOCKADDR*)&saServer, sizeof(saServer)); // connect
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
        std::string err_msg = "failed to connect";
        throw std::exception(err_msg.c_str());
    }
    char* request_bytes = getRequestBytes(); // get the request's bytes
    iResult = send(ConnectSocket, request_bytes, _bytes_amount, 0); // send the request to the server
    delete request_bytes;
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        std::string err_msg = "send failed with error: " + WSAGetLastError();
        throw std::exception(err_msg.c_str());
    }
    iResult = shutdown(ConnectSocket, SD_SEND); // shutdown the connection for sending more data
    if (iResult == SOCKET_ERROR) {
        std::string err_msg = "shutdown failed with error: " + WSAGetLastError();
        closesocket(ConnectSocket);
        WSACleanup();
        throw std::exception(err_msg.c_str());
    }
    Response resp = Response::processResponse(ConnectSocket); // procces the response from the server
    closesocket(ConnectSocket);
    WSACleanup();
    return resp;
}

Response Response::processResponse(SOCKET ConnectSocket)
{
    char* header_bytes = new char[Response::HEADER_SIZE];
    int recived_bytes = recv(ConnectSocket, header_bytes, Response::HEADER_SIZE, 0);
    Response resp = Response::processResponseHeader(header_bytes); // get the header as Response objet
    resp._payload = new char[resp._payload_size](); // create the payload
    // read the data from the server in chunks
    for (uint32_t i = 0;i < resp._payload_size;i += CHUNK_SIZE)
    {
        int read_size = std::min<int>(CHUNK_SIZE, resp._payload_size - i);
        recived_bytes = recv(ConnectSocket, &resp._payload[i], read_size, 0);
    }
    return resp;
}

Response Response::processResponseHeader(char serverdata[HEADER_SIZE])
{
    Response(resp);
    resp._bytes_amount = 0;
    std::memcpy(&resp._version, &serverdata[resp._bytes_amount], sizeof(resp._version));
    resp._bytes_amount += sizeof(resp._version);
    std::memcpy(&resp._code, &serverdata[resp._bytes_amount], sizeof(resp._code));
    resp._bytes_amount += sizeof(resp._code);
    std::memcpy(&resp._payload_size, &serverdata[resp._bytes_amount], sizeof(resp._payload_size));
    resp._bytes_amount += sizeof(resp._payload_size);
    return resp;
}
