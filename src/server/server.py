import threading
from multiprocessing.pool import ThreadPool
import socket
import logging
from protocol import Response, Request, RequestCode, ResponseCode
from MessageU import MessageU, User, Message


class Server:
    version = 2

    def __init__(self, host="127.0.0.1", port=80, thread_pool_size=8, socket_timeout=2):
        self.host = host
        self.port = port
        self.thread_pool_size = thread_pool_size
        self.socket_timeout = socket_timeout
        self.app = MessageU()

    def handle_connection(self, conn, addr, buff_size=1024):
        try:
            with conn:
                logging.debug(f'Connected by {addr}')
                data = conn.recv(buff_size)
                # text = data.decode("utf-8")
                # logging.debug("Received message: " + text)
                req = Request.process_request(data)
                if req.code == RequestCode.register:
                    username = "maayank"
                    self.app.register_user(username)
                elif req.code == RequestCode.users_list:
                    self.app.get_users_list()
                elif req.code == RequestCode.get_pub_key:
                    self.app.get_pub_key()
                elif req.code == RequestCode.send_message:
                    self.app.send_message()
                elif req.code == RequestCode.get_messages:
                    self.app.get_messages()
                logging.debug("finished", threading.currentThread().name)
                # print(1/0)
        except Exception as e:
            logging.error(f"got {e} Exception at {threading.currentThread().name}")
            # TODO handle Exception

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
