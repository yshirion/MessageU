
from enum import Enum


class Finals(Enum):
    REGITER_SUCCESS = 2100
    USER_LIST = 2101
    PUBLIC_KEY = 2102
    SENT = 2103
    MESSAGES = 2104
    ERROR = 9000
    REGSIZE = 16
    ULSIZE = 271
    KEYSIZE = 176
    SENTSIZE = 20
    MESSAGESIZE = 25
    VERSION = 2


class Response():

    def __init__(self):
        self.version = 0
        self.code = 0
        self.payload_size = 0
        self.payload = b''
        
    #Set values to responses Respectively
    def set_register(self, uid):
        self.code = Finals.REGITER_SUCCESS.value
        self.payload_size = Finals.REGSIZE.value
        self.payload = uid

    def set_userlist(self, list):
        self.code = Finals.USER_LIST.value
        self.payload_size = len(list) * Finals.ULSIZE.value

        #To concatenate all users (they are return as tuples in list)
        for user in list: 
            self.payload += user[0] + user[1]#self.text_to_bytes(user[1])

    def set_publicK(self, uid, publicK):
        self.code = Finals.PUBLIC_KEY.value
        self.payload_size = Finals.KEYSIZE.value
        self.payload += uid + publicK

    #Care in request to send message
    def set_sent(self, uid, Mid):
        self.code = Finals.SENT.value
        self.payload_size = Finals.SENTSIZE.value
        bytes_Mid = Mid.to_bytes(4, 'little')
        self.payload += uid + bytes_Mid

    #Care in request to get all messages from server
    def set_messages(self, list):
        self.code = Finals.MESSAGES.value
        number_of_messages = len(list)
        size_of_messages = 0
        
        #To concatenate all messages (they are return as tuples in list)
        for message in list:
            message_size = len(message[3])
            size_of_messages += message_size
            self.payload += \
                message[0] + \
                message[1] + \
                message[2] + \
                message_size.to_bytes(4, 'little') + \
                message[3]
        self.payload_size = (number_of_messages * Finals.MESSAGESIZE.value) + size_of_messages

    def error(self):
        self.code = Finals.ERROR.value
        self.payload_size = 0
        self._payload= b''

    def text_to_bytes(self, text):
        data = bytearray(text, 'ascii')
        newdata = bytearray(255)
        for i in range(min(len(data), len(newdata) - 1)):
            newdata[i] = data[i]
        return newdata

    #Concatenate all parts of response to send it.
    def build_response(self):
        self.version = Finals.VERSION.value
        response = self.version.to_bytes(1, 'little') + \
                   self.code.to_bytes(2, 'little') + \
                   self.payload_size.to_bytes(4, 'little') + \
                   self.payload
        #print(response)
        return response
