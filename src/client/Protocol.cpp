#include "protocol.h"

char* Request::getRequestBytes()
{
    char* request_data = (char*)malloc(MAX_LENGTH);

    //request_data[0] = '\0';
    this->bytes_amount = 0;
    std::memcpy(&request_data[this->bytes_amount], &this->version, sizeof(this->version));
    this->bytes_amount += sizeof(this->version);
    std::memcpy(&request_data[this->bytes_amount], &this->code, sizeof(this->code));
    this->bytes_amount += sizeof(this->code);
    if (this->payload_size != NULL)
    {
        std::memcpy(&request_data[this->bytes_amount], &this->payload_size, sizeof(this->payload_size));
        this->bytes_amount += sizeof(this->payload_size);
        //std::memcpy(&responsedata[this->bytes_amount], &this->payload, this->size);
        strncpy_s(&request_data[this->bytes_amount], int(this->payload_size) + 1, this->payload, int(this->payload_size));
        this->bytes_amount += this->payload_size;
    }

    return request_data;
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