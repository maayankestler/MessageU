import threading
from multiprocessing.pool import ThreadPool
import socket
import logging
from protocol import Response, Request, RequestCode, ResponseCode
from MessageU import MessageU


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
        pubkey = req.payload[self._username_size:]  # TODO transfer to string
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
        pubkey = self.app.get_pub_key(req.client_id)
        return Response(self.version, ResponseCode.get_pub_key, req.client_id.bytes_le + pubkey)

    def handle_send_message_request(self, req):
        msg = self.app.send_message(*req.payload)  # TODO split payload by bytes
        return Response(self.version, ResponseCode.users_list, msg.to_client + msg.message_id)

    def handle_get_messages_request(self, req):
        msgs = self.app.get_messages(req.client_id)
        return Response(self.version, ResponseCode.get_messages, msgs)  # TODO handle msgs list to payload

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
                    logging.error(f"got {e} Exception at {req.code.name} request")
                    resp = Response(self.version, ResponseCode.error)
                finally:
                    logging.info(f"finished handle {req.code.name} request")
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
