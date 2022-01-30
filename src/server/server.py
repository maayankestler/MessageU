import threading
from multiprocessing.pool import ThreadPool
import socket
import logging
from protocol import Response, Request, RequestCode, ResponseCode
from MessageU import MessageU


class Server:
    version = 2

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
        args_list = req.payload.split("\0")
        username = args_list[0]
        pubkey = args_list[1]
        user = self.app.register_user(username, pubkey)
        return Response(self.version, ResponseCode.register, user.client_id)

    def handle_get_users_request(self, req):
        users = self.app.get_users_list(req.client_id)
        return Response(self.version, ResponseCode.users_list, users)  # TODO handle users list to payload

    def handle_get_pubkey_request(self, req):
        pubkey = self.app.get_pub_key(req.client_id)
        return Response(self.version, ResponseCode.get_pub_key, pubkey)

    def handle_send_message_request(self, req):
        msg = self.app.send_message(*req.payload)  # TODO split payload by bytes
        return Response(self.version, ResponseCode.users_list, msg.to_client + msg.message_id)

    def handle_get_messages_request(self, req):
        msgs = self.app.get_messages(req.client_id)
        return Response(self.version, ResponseCode.get_messages, msgs)  # TODO handle msgs list to payload

    def handle_connection(self, conn, addr, buff_size=1024):
        try:
            with conn:
                logging.debug(f'Connected by {addr}')
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
                    logging.debug(f"finished handle {req.code} request")
                    resp.send(*addr)
        except Exception as e:
            logging.error(f"got {e} Exception at {threading.currentThread().name}")
            # TODO handle Exception

        # resp = Response(self.version)
        # if req.code == RequestCode.register:
        #     args_list = req.payload.split("\0")
        #     username = args_list[0]
        #     pubkey = args_list[1]
        #     self.app.register_user(username, pubkey)
        #     resp.code = ResponseCode.register
        # elif req.code == RequestCode.users_list:
        #     users = self.app.get_users_list(req.client_id)
        #     resp.code = ResponseCode.users_list
        # elif req.code == RequestCode.get_pub_key:
        #     pubkey = self.app.get_pub_key(req.client_id)
        #     resp.code = ResponseCode.get_pub_key
        # elif req.code == RequestCode.send_message:
        #     self.app.send_message(*req.payload)  # TODO split payload by bytes
        #     resp.code = ResponseCode.send_message
        # elif req.code == RequestCode.get_messages:
        #     self.app.get_messages()
        #     resp.code = ResponseCode.get_messages
        # else:
        #     logging.error(f"can't recognize RequestCode {req.code}")
        #     resp.code = ResponseCode.error

    def run(self):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(self.socket_timeout)
            s.bind((self.host, self.port))
            s.listen()
            with ThreadPool(self.thread_pool_size) as pool:
                try:
                    while True:
                        try:
                            conn, addr = s.accept()
                            pool.apply_async(self.handle_connection, (conn, addr))
                        except socket.timeout:
                            logging.debug("refresh socket")
                            pass  # enable to stop the program with KeyboardInterrupt
                        except Exception:
                            raise
                except KeyboardInterrupt:
                    logging.info("shutting down")
                finally:
                    self.app.connection.close()
