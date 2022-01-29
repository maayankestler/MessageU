import socket
import struct
from enum import Enum


class RequestCode(Enum):
    register = 1100
    users_list = 1101
    get_pub_key = 1102
    send_message = 1103
    get_messages = 1104


class Request:
    _client_id_size = 16  # the size in bytes of client_id

    def __init__(self, client_id, version, code, payload_size=0, payload=""):
        self.client_id = client_id
        self.version = version
        self.code = RequestCode(code)
        self.payload_size = payload_size
        self.payload = payload

    def __str__(self):
        return f"client_id: {self.client_id}, version: {self.version}, " \
               f"code: {self.code.value} payload: {self.payload}"

    @staticmethod
    def process_request(data_bytes):
        # read client id, Version, Code and Payload size
        pack_format = f"<{Request._client_id_size}sBHI"
        data = struct.unpack(pack_format, data_bytes[:struct.calcsize(pack_format)])
        i = struct.calcsize(pack_format)
        payload_size = data[-1]

        # if there is a payload
        if payload_size:
            pack_format = f"<{payload_size}s"
            data += struct.unpack(pack_format, data_bytes[i:i + struct.calcsize(pack_format)])
        req = Request(*data)
        return req


class ResponseCode(Enum):
    register = 2100
    users_list = 2101
    get_pub_key = 2102
    send_message = 2103
    get_messages = 2104
    error = 9000


class Response:
    _max_response_size = 4096

    def __init__(self, version, code, payload_size=0, payload=""):
        self.version = version
        self.code = ResponseCode(code)
        self.payload_size = payload_size
        self.payload = payload

    def __str__(self):
        return f"version: {self.version}, code: {self.code.value}, payload: {self.payload}"

    def send(self, server, port):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # pack the request vars as byes using little endian
            b = struct.pack("<BHI", self.version, self.code, self.payload_size)
            if self.payload_size:
                b += struct.pack(f"{self.payload_size}s", self.payload)
            s.connect((server, port))
            s.sendall(b)
            data = s.recv(self._max_response_size)
