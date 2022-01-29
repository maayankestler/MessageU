from server import Server
import logging


def read_config(path="myport.info"):
    with open(path, "r") as file:
        port = file.read()
    return port


def init_logging():
    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)


def main():
    init_logging()
    port = int(read_config("myport.info"))
    server = Server(port=port)
    server.run()


if __name__ == '__main__':
    main()
