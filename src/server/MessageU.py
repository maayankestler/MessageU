import logging
import sqlite3
import uuid
from datetime import datetime
from contextlib import closing


class MessageU:
    def __init__(self, db_file="server.db"):
        self.db_file = db_file
        # self.connection = sqlite3.connect(self.db_file, check_same_thread=False)  # TODO think about multi threads
        # self.cursor = self.connection.cursor()
        self.init_tables()

    # def __del__(self):
    #     self.cursor.close()
    #     self.connection.close()

    def init_tables(self):
        try:
            with sqlite3.connect(self.db_file) as connection:
                with closing(connection.cursor()) as cursor:
                    cursor.execute('''CREATE TABLE clients (UserName varchar(255) UNIQUE,
                                                            ID BLOB(16) PRIMARY KEY,  
                                                            PublicKey CHARACTER(160),
                                                            LastSeen DATETIME)''')
                    cursor.execute('''CREATE TABLE messages (ToClient BLOB(16), 
                                                             FromClient BLOB(16), 
                                                             Type TINYINT, 
                                                             Content BLOB,
                                                             ID INTEGER PRIMARY KEY AUTOINCREMENT)''')
        except sqlite3.OperationalError as e:
            logging.warning(e)

    def register_user(self, username, pubkey="b"*160):
        user = User(username=username, pubkey=pubkey)
        try:
            with sqlite3.connect(self.db_file) as connection:
                with closing(connection.cursor()) as cursor:
                    cursor.execute('''INSERT INTO clients(UserName,ID,PublicKey,LastSeen)
                                   VALUES(?,?,?,?)''', (user.username, user.client_id.bytes_le, user.pubkey, user.last_seen))
                connection.commit()
            return user
        except sqlite3.IntegrityError:
            logging.warning(f"user {username} already exist")
            # TODO return error to client

    def get_users_list(self, client_id):
        with sqlite3.connect(self.db_file) as connection:
            with closing(connection.cursor()) as cursor:
                users = list(cursor.execute('SELECT * FROM clients WHERE ID!=:id', {"id": client_id}))
        users = list(map(lambda r: User(*r), users))
        return users

    def get_pub_key(self, client_id):
        with sqlite3.connect(self.db_file) as connection:
            with closing(connection.cursor()) as cursor:
                user_row = list(cursor.execute('SELECT * FROM clients WHERE ID=:id', {"id": client_id}))[0]
        user = User(*user_row)
        return user.pubkey

    def send_message(self, to_client, from_client, message_type, content):
        msg = Message(to_client, from_client, message_type, content)
        try:
            with sqlite3.connect(self.db_file) as connection:
                with closing(connection.cursor()) as cursor:
                    cursor.execute('''INSERT INTO messages(ToClient,FromClient,Type,Content)
                                   VALUES(?,?,?,?)''', (msg.to_client, msg.from_client, msg.message_type, msg.content))
                connection.commit()
            return msg
        except sqlite3.IntegrityError:
            logging.warning(f"message with id {msg.message_id} already exist")
            # TODO return error to client

    def get_messages(self, client_id):
        with sqlite3.connect(self.db_file) as connection:
            with closing(connection.cursor()) as cursor:
                msgs = list(cursor.execute('SELECT * FROM messages WHERE ID=:id', {"id": client_id}))
        msgs = list(map(lambda r: Message(*r), msgs))
        return msgs


class User:
    def __init__(self, username, client_id=None,  pubkey=None, last_seen=None):
        self.client_id = client_id if client_id else uuid.uuid4()
        self.username = username
        self.pubkey = pubkey if pubkey else "b"  # * 160  # TODO generate pubkey from private key
        self.last_seen = last_seen if last_seen else datetime.now()

    def __str__(self):
        return f"username: {self.username}, id: {self.client_id} last_seen: {self.last_seen}"


class Message:
    def __init__(self, to_client, from_client, message_type, content, message_id=None):
        self.to_client = to_client
        self.from_client = from_client
        self.message_type = message_type
        self.content = content
        self.message_id = message_id  # if message_id else "b" * 4  # TODO create message ID

    def __str__(self):
        return f"message with id '{self.message_id}'  from '{self.from_client}' to '{self.to_client}'"
