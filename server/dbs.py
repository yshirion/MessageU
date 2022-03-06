import sqlite3
import os
import threading

#To lock access for database to avoid parallel accessing, 
#lock before every access and release in closed function 
#or in function itself if it doesnt call to 'closed'
sem=threading.Semaphore(10)

class DataBase():
   
   def __init__(self, dbfile):
       self.conn = sqlite3.connect(dbfile)
       self.conn.text_factory = bytes

       #Check if tables are exist, and not recreate them.
       sem.acquire()
       if os.stat(dbfile).st_size == 0:
          print("file_empty")
          self.create_tables()
       self.message_number = self.reset_message_number()
       sem.release()

   def reset_message_number(self):
       cur = self.conn.cursor()
       cur.execute("SELECT MAX(ID) FROM messages")
       max = cur.fetchall()
       if max[0][0]:
           return int.from_bytes(max[0][0],'big')
       cur.close()
       return 0

   def create_tables(self):
       sem.acquire()
       tables = """
       CREATE TABLE clients (
       ID varchar(16) NOT NULL PRIMARY KEY, 
       UserName varchar(255), 
       PublicKey varchar(160), 
       LastSeen text);
       CREATE TABLE messages (
       ID varchar(4) NOT NULL PRIMARY KEY,
       ToClient varchar(16),
       FromClient varchar(16),
       Type varchar(1),
       Content BLOB);
       """
       self.conn.executescript(tables)
       self.conn.commit()

   def update_time(self, uid):
       sem.acquire()
       #Update entry user time
       query = "UPDATE clients set LastSeen = datetime('now', 'localtime') WHERE ID = ?"
       self.conn.execute(query, [uid])
       self.conn.commit()
       sem.release()
       
   def get_bublic_key(self, uid):
       sem.acquire()
       cur = self.conn.cursor()
       querry = "SELECT PublicKey FROM clients WHERE ID = ?"
       cur.execute(querry, [uid])
       answer = cur.fetchall()[0][0] #Because the data return in tuple in list
       self.closed()
       return answer

   def register_in_DB(self, uid, name, publicK):
       sem.acquire()
       self.conn.execute("""
        INSERT INTO clients VALUES 
        (?, ?, ?, datetime('now', 'localtime'))""",
        (uid, name, publicK))
       self.closed()

   def ifexist_user(self, user):
       sem.acquire()
       curr = self.conn.cursor()
       if len(user) == 16: #It mean that user= user id. for send message for other client
           querry = "SELECT ID FROM clients WHERE ID = ?"
       else: # user = name uaer. When we want to check existing for registeration.
           querry = "SELECT ID FROM clients WHERE UserName = ?"

       curr.execute(querry, [user])
       user = curr.fetchall()
       self.conn.commit()
       sem.release()
       if user == []:
           return False
       else: return True

   def get_userlist(self, uid):
        sem.acquire()
        cur = self.conn.cursor()
        querry = "SELECT ID, UserName FROM clients WHERE ID != ?"
        cur.execute(querry, [uid])
        answer =cur.fetchall()
        self.closed()
        return answer
   
   def insert_message(self, message):
       sem.acquire()
       #Serial number for id of message.
       self.message_number += 1
       print(self.message_number)
       number = self.message_number.to_bytes(4, 'big')
       message.insert(0,number)
       querry = "INSERT INTO messages VALUES(?, ?, ?, ?, ?)"
       cur = self.conn.cursor()
       self.conn.execute(querry, (message[0],message[1], message[2],message[3],message[4]))
       self.closed()
       return self.message_number

   def get_messages(self, uid):
       sem.acquire()
       cur = self.conn.cursor()
       querry = "SELECT FromClient, ID, Type, Content FROM messages WHERE ToClient = ?"
       cur.execute(querry, [uid])
       res = cur.fetchall()
       querry1 = "DELETE FROM messages WHERE ToClient = ?"
       self.conn.execute(querry1, [uid])
       self.closed()
       return res

   def closed(self):
       #save all changes and close the file, and release lock on database.
       self.conn.commit()
       self.conn.close()
       sem.release()
