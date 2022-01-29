import logging
import sqlite3
import uuid
from datetime import datetime


class MessageU:
    def __init__(self, db_file="server.db"):
        self.db_file = db_file
        self.connection = sqlite3.connect(self.db_file)
        self.cursor = self.connection.cursor()
        self.init_tables()

    def init_tables(self):
        try:
            self.cursor.execute('''CREATE TABLE clients (ID CHARACTER(16) PRIMARY KEY, 
                                                         UserName varchar(255), 
                                                         PublicKey CHARACTER(160),
                                                         LastSeen DATETIME)''')
            self.cursor.execute('''CREATE TABLE messages (ID CHARACTER(4) PRIMARY KEY, 
                                                          ToClient CHARACTER(16), 
                                                          FromClient CHARACTER(16), 
                                                          Type TINYINT, 
                                                          Content BLOB)''')
        except sqlite3.OperationalError as e:
            logging.warning("tables already exists")

    def register_user(self, username):
        user = User(username)
        self.cursor.execute('''INSERT INTO clients(ID,UserName,PublicKey,LastSeen)
                               VALUES(?,?,?,?)''',
                            (user.client_id, user.username, user.pubkey, user.last_seen))
        self.connection.commit()

    def get_users_list(self):
        users = list(self.cursor.execute('SELECT * FROM clients'))
        # map(users) TODO use user class
        print(users)
        return users

    def get_pub_key(self):
        pass

    def send_message(self):
        pass

    def get_messages(self):
        pass


class User:
    def __init__(self, username):
        self.client_id = str(uuid.uuid4())
        self.username = username
        self.pubkey = "b"  # * 160  # TODO generate pubkey from private key
        self.last_seen = datetime.now()


class Message:
    def __init__(self, message_id, to_client, from_client, message_type, content):
        self.message_id = message_id
        self.to_client = to_client
        self.from_client = from_client
        self.message_type = message_type
        self.content = content
