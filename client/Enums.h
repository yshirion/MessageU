#pragma once

#include <array>

enum class RequestCode :uint16_t
{
    Register = 1100,
    Clients_List = 1101,
    Public_Key = 1102,
    Send_Text_Message = 1103,
    Waiting_Messages = 1104
};

enum class ResponseCode :uint16_t
{
    Registeration_Ok = 2100,
    Request_Users_List = 2101,
    Request_Public_Key = 2102,
    Sending_Ok = 2103,
    Requested_Messages = 2104,
    Server_Error = 9000
};

enum class MessageType :uint8_t
{
    Key_Requst = 1,
    Key_Sending = 2,
    Text = 3
};
