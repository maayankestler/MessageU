import logging
import sqlite3
import uuid
from datetime import datetime
from contextlib import closing
import struct


class MessageU:
    max_username_size = 255

    def __init__(self, db_file="server.db"):
        self.db_file = db_file
        self.init_tables()

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
            logging.warning(e)  # warning if the tables already exist

    def register_user(self, username, pubkey):
        user = User(username=username, pubkey=pubkey)
        try:
            with sqlite3.connect(self.db_file) as connection:
                with closing(connection.cursor()) as cursor:
                    cursor.execute('''INSERT INTO clients(UserName,ID,PublicKey,LastSeen) VALUES(?,?,?,?)''',
                                   (user.username, user.client_id.bytes_le, user.pubkey, user.last_seen))
                connection.commit()  # commit the changes
            return user
        except sqlite3.IntegrityError as e:
            logging.error(f"user {username} already exist")
            raise e

    def get_users_list(self, client_id):
        with sqlite3.connect(self.db_file) as connection:
            with closing(connection.cursor()) as cursor:
                users = list(cursor.execute('SELECT * FROM clients WHERE ID!=:id', {"id": client_id.bytes_le}))
        users = list(map(lambda r: User(*r), users))  # process rows to User objects
        return users

    def get_pub_key(self, client_id):
        with sqlite3.connect(self.db_file) as connection:
            with closing(connection.cursor()) as cursor:
                user_row_list = list(cursor.execute('SELECT * FROM clients WHERE ID=:id', {"id": client_id.bytes_le}))
        assert len(user_row_list) > 0, f"client {client_id} not found"
        user = User(*user_row_list[0])  # process row to User object
        return user.pubkey

    def send_message(self, to_client, from_client, message_type, content=b""):
        msg = Message(to_client, from_client, message_type, content)
        try:
            with sqlite3.connect(self.db_file) as connection:
                with closing(connection.cursor()) as cursor:
                    cursor.execute('''INSERT INTO messages(ToClient,FromClient,Type,Content) VALUES(?,?,?,?)''',
                                   (msg.to_client.bytes_le, msg.from_client.bytes_le, msg.message_type, msg.content))
                    if not msg.message_id:
                        msg.message_id = cursor.lastrowid  # update the message id
                connection.commit()
            return msg
        except sqlite3.IntegrityError as e:
            logging.warning(f"message with id {msg.message_id} already exist")
            raise e

    def get_messages(self, client_id):
        with sqlite3.connect(self.db_file) as connection:
            with closing(connection.cursor()) as cursor:
                msgs_rows = list(cursor.execute('SELECT * FROM messages WHERE ToClient=:id', {"id": client_id.bytes_le}))
        msgs = []
        for r in msgs_rows:
            # process messages rows
            to_client, from_client, message_type, content, message_id = r
            msg = Message(uuid.UUID(bytes_le=to_client), uuid.UUID(bytes_le=from_client),
                          message_type, content, message_id)
            msgs.append(msg)
        return msgs

    def delete_messages(self, msgs):
        with sqlite3.connect(self.db_file) as connection:
            with closing(connection.cursor()) as cursor:
                for msg in msgs:
                    cursor.execute('DELETE FROM messages WHERE ID=:id', {"id": msg.message_id})


class User:
    def __init__(self, username, client_id=None,  pubkey=None, last_seen=None):
        self.client_id = uuid.UUID(bytes_le=client_id) if client_id else uuid.uuid4()
        self.username = username
        self.pubkey = pubkey
        self.last_seen = last_seen if last_seen else datetime.now()

    def __str__(self):
        return f"username: {self.username}, id: {self.client_id} last_seen: {self.last_seen}"

    def __bytes__(self):
        b = bytes()
        b += self.client_id.bytes_le
        b += bytes(self.username, "utf-8") + struct.pack("<B", 0) * (MessageU.max_username_size - len(self.username))
        return b


class Message:
    def __init__(self, to_client, from_client, message_type, content, message_id=None):
        self.to_client = to_client
        self.from_client = from_client
        self.message_type = message_type
        self.content = content if content else bytes()
        self.message_id = message_id  # created by the db AUTOINCREMENT

    def __str__(self):
        return f"message with id '{self.message_id}'  from '{self.from_client}' to '{self.to_client}'"

    def __bytes__(self):
        b = self.from_client.bytes_le + struct.pack("<IBI", self.message_id, self.message_type, len(self.content))
        b += self.content
        return b
