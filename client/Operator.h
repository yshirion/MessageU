#pragma once
#include "CryptoHelper.h"
#include "Utilities.h"
#include "Enums.h"
#include "MyUser.h"
#include "Users.h"
#include "ConnectToServerHandler.h"
#include <fstream>
#include <iostream>
#include <string>

#pragma comment(lib,"ws2_32.lib") //Winsock Library
class Operator
{
public:

	Operator();

    //all operations
	bool ReadServerDetails();	
    bool IsRegisterBefore();
	void Register();
    void RequestClientsList();
    void RequestPublicKey();
    void RequestWaitingMessages();
    void SendTextMessage();
    void SendRequestSymmetricKey();
    void SendSymmetricKey();

  
private:
    
    std::shared_ptr<MyUser> my_user;
    std::shared_ptr<Users> usersList;

    static constexpr auto myInfoFilePath = "me.info";

    void UpdateUsersListFormServer(const std::vector<uint8_t>&);
    void SendMessageToServer(MessageType);
    std::string HandleMessagesPayload(std::vector<uint8_t>& payload);


};

