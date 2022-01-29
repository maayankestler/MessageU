import threading
from multiprocessing.pool import ThreadPool
import socket
import logging
import time


class Server:
    version = 2

    def __init__(self, host="127.0.0.1", port=80, thread_pool_size=8, socket_timeout=2):
        self.host = host
        self.port = port
        self.thread_pool_size = thread_pool_size
        self.socket_timeout = socket_timeout

    def handle_connection(self, conn, addr):
        try:
            with conn:
                logging.debug(f'Connected by {addr}')
                data = conn.recv(1024)
                text = data.decode("utf-8")
                logging.debug("Received message: " + text)
                time.sleep(1)
                logging.debug("finished", threading.currentThread().name)
                # print(1/0)
        except Exception as e:
            logging.error(f"got {e} Exception at {threading.currentThread().name}")

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
