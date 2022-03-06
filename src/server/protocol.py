import socket
import struct
from enum import Enum
import uuid


class RequestCode(Enum):
    register = 1100
    users_list = 1101
    get_pub_key = 1102
    send_message = 1103
    get_messages = 1104


class Request:
    client_id_size = 16  # the size in bytes of client_id

    def __init__(self, client_id, version, code, payload_size=0, payload=""):
        self.client_id = uuid.UUID(bytes_le=client_id)
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
        pack_format = f"<{Request.client_id_size}sBHI"
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

    # TODO check if param or property func
    @property
    def _max_response_size(self):
        # _max_response_size = 4096
        return 4096

    def __init__(self, version, code=-1, payload=""):
        self.version = version
        try:
            self.code = ResponseCode(code)
        except ValueError:
            self.code = None
        # self.payload_size = payload_size
        self.payload_size = len(payload)
        self.payload = payload

    def __str__(self):
        return f"version: {self.version}, code: {self.code.value}, payload: {self.payload}"

    def send(self, conn):
        b = struct.pack("<BHI", self.version, self.code.value, self.payload_size)
        if self.payload_size:
            b += self.payload
        conn.sendall(b)
