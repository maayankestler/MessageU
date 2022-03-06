import threading
from multiprocessing.pool import ThreadPool
import socket
import logging
from protocol import Response, Request, RequestCode, ResponseCode
from MessageU import MessageU
import uuid
import struct


class Server:
    version = 2
    _username_size = 255

    def __init__(self, host="127.0.0.1", port=80, thread_pool_size=8, socket_timeout=2):
        self.host = host
        self.port = port
        self.thread_pool_size = thread_pool_size
        self.socket_timeout = socket_timeout
        self.app = MessageU()

        self.code_to_func = {
            RequestCode.register: self.handle_register_request,
            RequestCode.users_list: self.handle_get_users_request,
            RequestCode.get_pub_key: self.handle_get_pubkey_request,
            RequestCode.send_message: self.handle_send_message_request,
            RequestCode.get_messages: self.handle_get_messages_request
        }

    def handle_register_request(self, req):
        username = req.payload[:self._username_size].decode("utf8").strip("\0")
        pubkey = req.payload[self._username_size:]
        # pubkey = b64encode(req.payload[self._username_size:])
        user = self.app.register_user(username, pubkey)
        # return Response(self.version, ResponseCode.register, user.client_id.replace("-", ""))
        return Response(self.version, ResponseCode.register, user.client_id.bytes_le)

    def handle_get_users_request(self, req):
        users = self.app.get_users_list(req.client_id)
        users_bytes = bytes()
        for user in users:
            users_bytes += bytes(user)
        return Response(self.version, ResponseCode.users_list, users_bytes)  # TODO handle users list to payload

    def handle_get_pubkey_request(self, req):
        client_id = uuid.UUID(bytes_le=req.payload)
        pubkey = self.app.get_pub_key(client_id)
        return Response(self.version, ResponseCode.get_pub_key, client_id.bytes_le + pubkey)

    def handle_send_message_request(self, req):
        # read client id, type, and content size
        pack_format = f"<{Request.client_id_size}sBI"
        data = struct.unpack(pack_format, req.payload[:struct.calcsize(pack_format)])
        i = struct.calcsize(pack_format)
        to_client_byes, message_type, content_size = data
        to_client = uuid.UUID(bytes_le=to_client_byes)
        if content_size:
            pack_format = f"<{content_size}s"
            data += struct.unpack(pack_format, req.payload[i:i + struct.calcsize(pack_format)])
            msg = self.app.send_message(to_client, req.client_id, message_type, data[-1])
        else:
            msg = self.app.send_message(to_client, req.client_id, message_type)
        msg_bytes = msg.to_client.bytes_le + struct.pack("<I", msg.message_id)
        return Response(self.version, ResponseCode.send_message, msg_bytes)

    def handle_get_messages_request(self, req):
        msgs = self.app.get_messages(req.client_id)
        msgs_bytes = bytes()
        for msg in msgs:
            msgs_bytes += bytes(msg)
        return Response(self.version, ResponseCode.get_messages, msgs_bytes)

    def handle_connection(self, conn, addr, buff_size=1024):
        try:
            with conn:
                logging.info(f'Connected by {addr}')
                data = conn.recv(buff_size)
                req = Request.process_request(data)
                try:
                    resp = self.code_to_func[req.code](req)
                except KeyError:
                    logging.error(f"can't recognize RequestCode {req.code}")
                    resp = Response(self.version, ResponseCode.error)
                except Exception as e:
                    logging.error(f"got '{e}' Exception at {req.code.name} request")
                    resp = Response(self.version, ResponseCode.error)
                finally:
                    logging.info(f"finished handle {req.code.name} request "
                                 f"from {req.client_id} with code {resp.code.name}")
                    resp.send(conn)
        except Exception as e:
            logging.error(f"got {e} Exception at {threading.currentThread().name}")
            # TODO handle Exception

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
            sock.settimeout(self.socket_timeout)
            sock.bind((self.host, self.port))
            sock.listen()
            with ThreadPool(self.thread_pool_size) as pool:
                try:
                    while True:
                        try:
                            conn, addr = sock.accept()
                            pool.apply_async(self.handle_connection, (conn, addr))
                        except socket.timeout:
                            logging.debug("refresh socket")
                            pass  # enable to stop the program with KeyboardInterrupt
                        except Exception:
                            raise
                except KeyboardInterrupt:
                    logging.info("shutting down")
