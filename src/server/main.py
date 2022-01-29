from server import Server
import logging
from MessageU import MessageU


def read_config(path="myport.info"):
    with open(path, "r") as file:
        port = file.read()
    return port


def init_logging():
    logging.basicConfig(format='%(levelname)s: %(message)s', level=logging.DEBUG)


def main():
    init_logging()
    port = int(read_config("myport.info"))
    # server = Server(port=port)
    # server.run()
    app = MessageU()
    app.get_users_list()
    app.register_user("maayan")
    app.register_user("tomer")
    app.register_user("ido")
    app.get_users_list()  # TODO check why db delete itself every run
    app.connection.close()


if __name__ == '__main__':
    main()
