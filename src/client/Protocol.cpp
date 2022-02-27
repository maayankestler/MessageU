#include "protocol.h"

char* Request::getRequestBytes()
{
    char* request_data = (char*)malloc(MAX_LENGTH);



    //request_data[0] = '\0';
    this->bytes_amount = 0;
    std::memcpy(&request_data[this->bytes_amount], &this->client_id, sizeof(this->client_id));
    this->bytes_amount += sizeof(this->client_id);
    std::memcpy(&request_data[this->bytes_amount], &this->version, sizeof(this->version));
    this->bytes_amount += sizeof(this->version);
    std::memcpy(&request_data[this->bytes_amount], &this->code, sizeof(this->code));
    this->bytes_amount += sizeof(this->code);
    if (this->payload_size != NULL)
    {
        std::memcpy(&request_data[this->bytes_amount], &this->payload_size, sizeof(this->payload_size));
        this->bytes_amount += sizeof(this->payload_size);
        std::memcpy(&request_data[this->bytes_amount], &this->payload, this->payload_size); // TODO check why addin bytes?????????
        //strncpy_s(&request_data[this->bytes_amount], int(this->payload_size) + 1, this->payload, int(this->payload_size));
        this->bytes_amount += this->payload_size;
    }

    return request_data;
}

Response Request::sendRequset(std::string ip, int port)
{
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    SOCKET ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
    }
    char* request_bytes = this->getRequestBytes();
    struct sockaddr_in saServer;
    saServer.sin_family = AF_INET;
    inet_pton(AF_INET, ip.c_str(), &saServer.sin_addr.s_addr);
    saServer.sin_port = htons(port);
    iResult = connect(ConnectSocket, (SOCKADDR*)&saServer, sizeof(saServer));
    if (iResult == SOCKET_ERROR) {
        closesocket(ConnectSocket);
        ConnectSocket = INVALID_SOCKET;
    }
    iResult = send(ConnectSocket, request_bytes, this->bytes_amount, 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
    }
    printf("Bytes Sent: %ld\n", iResult);
    //iResult = shutdown(ConnectSocket, SD_SEND);
    //if (iResult == SOCKET_ERROR) {
    //    printf("shutdown failed with error: %d\n", WSAGetLastError());
    //    closesocket(ConnectSocket);
    //    WSACleanup();
    //}
    //char response_bytes[MAX_LENGTH];
    //iResult = recv(ConnectSocket, response_bytes, MAX_LENGTH, 0);
    //return Response::processResponse(response_bytes, MAX_LENGTH);
    closesocket(ConnectSocket);
    WSACleanup();
    return Response();
}

Response Response::processResponse(char serverdata[], int length)
{
    //Response resp = Response(); // TODO check {0}
    Response(resp); // TODO check if valid
    int i = 0;
    std::memcpy(&resp.version, &serverdata[i], sizeof(resp.version));
    i += sizeof(resp.version);
    std::memcpy(&resp.code, &serverdata[i], sizeof(resp.code));
    i += sizeof(resp.code);
    if (length > i)
    {
        std::memcpy(&resp.payload_size, &serverdata[i], sizeof(resp.payload_size));
        i += sizeof(resp.payload_size);
        resp.payload = new char[resp.payload_size + 1];
        std::memcpy(resp.payload, &serverdata[i], resp.payload_size);
        //strncpy_s(resp.payload, resp.size + 1, &clientdata[i], resp.size);
        i += resp.payload_size;
    }
    return resp;
}