import socket
from response import Response
import uuid
from dbs import DataBase


headsize = 23
maxsize = 1024
register_size = 415
userlist_size = 0
getmessage_size = 0
getpublic_size = 16
sendmessage_minsize = 21
type_getsimetric = 1

type_send = 3

class Request():

    def __init__(self, conn, db):
        self.conn = conn
        self.response = Response()
        self.DB = DataBase(db)
        self.data = b''
        self.payload = b''
        self.id: bytes = b''
        self.version = 0
        self.codereq = 0
        self.payload_size = 0
        
     #threading function   
    def run(self):
        try:
            #recev the request and order it
            self.recev_head()
            self.read_message()
            
            #switch for handling request and return response to send
            if self.codereq == 1100:
               message = self.register()
            elif self.codereq == 1101:
                 message = self.get_userlist()
            elif self.codereq == 1102:
                 message = self.get_public()
            elif self.codereq == 1103:
                 message = self.send_message()
            elif self.codereq == 1104:
                 message = self.get_messages()
            else:
                 message = self.error()           

            self.conn.sendall(message)

        except socket.error:
            print("socket problem")
        

    def register(self):
        #check size request
        if len(self.payload) < register_size or len(self.payload) > register_size:
            print("Payload size of registeration is invalid.")
            return self.error()

        try:
            #Check if user already exist
            name = self.payload[:255]
            public_key = self.payload[255:]
            if self.DB.ifexist_user(name):
                print("The name user already exist.")
                return self.error()
            
            #Create uuid for client and convert it to bytes
            uid = uuid.uuid4().bytes
            self.DB.register_in_DB(uid, name, public_key)

            #Create message for responsed to client 
            self.response.set_register(uid)
            return (self.response.build_response())
                   
        except Exception as e:#For problem probably in data base or locking.
            print("There is problem in register fuction.")
            print(e)
            return self.error()
                         
    def get_userlist(self):
        if len(self.payload) > userlist_size:
            print("Payload size of userlist is invalid.")
            return self.error()

        try: 
            self.DB.update_time(self.id) #update insert time of client
            userlist = self.DB.get_userlist(self.id) #pull out the data from database

            self.response.set_userlist(userlist)
            return (self.response.build_response())

        except Exception as e:
            print("There is problem in get_userlist function.")
            print(e)
            return self.error()

    def get_public(self):
        if len(self.payload) != getpublic_size:
            print("Payload size of get public key is invalid.")
            return self.error()

        try:
            self.DB.update_time(self.id)

            clientid = self.payload
            #Check if the client already exist in the system
            if not self.DB.ifexist_user(clientid):
                print("The client doesn't exist.")
                return self.error()

            publicK = self.DB.get_bublic_key(clientid)

            self.response.set_publicK(clientid, publicK)
            return (self.response.build_response())

        except Exception as e:
            print("There is problem in get_public function.")
            print (e)
            return self.error()

    def send_message(self):
        if len(self.payload) < sendmessage_minsize:
            print("Payload size of send message is invalid.")
            return self.error()

        destid = self.payload[:16]
        type_message = self.payload[16:17]
        message_size = int.from_bytes(self.payload[17:21],'little')
        content = self.payload[21:]
        int_type = int.from_bytes(type_message,'little')

        #Check validation of type values and the size of content respectively
        if int_type < type_getsimetric or int_type > type_send:
            print("The message type is invalid.")
            return self.error()
        if int_type == type_getsimetric and message_size > getmessage_size:
            print("The message size is invalid.")
            return self.error()

        #Organize message to update in database
        message = []
        message.append(destid)
        message.append(self.id)
        message.append(type_message)
        message.append(content)
        
        
        try:
            self.DB.update_time(self.id)
            if not self.DB.ifexist_user(destid): #Check if the destination client exist in the system
                print("The client doesn't exist.")
                return self.error()
            number = self.DB.insert_message(message) # id of message in database

            self.response.set_sent(destid, number)
            return (self.response.build_response())
        except Exception as e:
            print("There is problem in send message function.")
            print(e)
            return self.error()
        
    def get_messages(self):
        if len(self.payload) > getmessage_size:
            print("Payload size of get messages is invalid.")
            return self.error()

        try:
            self.DB.update_time(self.id)
            messages = self.DB.get_messages(self.id)
            
            self.response.set_messages(messages)
            return (self.response.build_response())

        except Exception as e:
            print("There is problem in get_message function.")
            print(e)
            return self.error()

    def error(self): #Build error message when the problem occured
        self.response.error()
        return (self.response.build_response())
        
    #get head message from client 
    def recev_head(self):
        self.data = self.conn.recv(headsize)
        #print(self.data) #test
        if len(self.data) < headsize:
            print("The size of message's head is incorrect.")
            return self.error()
    #handling for head message and get to message body                   
    def read_message(self):
        self.id = self.data[0:16]
        self.version = int.from_bytes(self.data[16:17],'little')#convert int to little bytes for sending
        self.codereq = int.from_bytes(self.data[17:19],'little')
        self.payload_size = int.from_bytes(self.data[19:23],'little')
        self.payload = self.conn.recv(self.payload_size)

        



        

