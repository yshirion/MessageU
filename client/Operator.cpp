#include "Operator.h"


Operator::Operator()
{
    //init private objects 
    usersList = std::make_shared<Users>();
    my_user = nullptr;

    //form the beginning to check if the user is register 
    my_user = MyUser::ReadMyUserFromFile(myInfoFilePath);
}

bool Operator::ReadServerDetails()
{
    return ConnectToServerHandler::ReadServerDetails();
}

bool Operator::IsRegisterBefore()
{
    //if there isnt file details
    if (my_user != nullptr)
    {
        std::cout << "\n\nHello "<< my_user->GetName()<<"! Wellcome back!!!";
        return true;
    }

    return false;
}

void Operator::Register()
{
    //get name from client
    std::string fullName;
    std::cout << "Please enter your full name for register : ";
    std::cin.ignore();
    std::getline (std::cin, fullName);

    //declare keys
    std::shared_ptr<CryptoPP::RSA::PrivateKey> private_key = CryptoHelper::PrivateKeyCreation();
    std::shared_ptr<CryptoPP::RSA::PublicKey> public_key = CryptoHelper::PublicKeyCreation(private_key);
   
    //init keys
    CryptoPP::byte private_key_value[CryptoHelper::Unsymmetric_Key_Length] = { 0 },
        public_key_value[CryptoHelper::Unsymmetric_Key_Length] = { 0 };

    CryptoHelper::ConvertUnsymmetricKeyToBytes(private_key, private_key_value);
    CryptoHelper::ConvertUnsymmetricKeyToBytes(public_key, public_key_value);

    std::vector<uint8_t> request_payload(NameSize + CryptoHelper::Unsymmetric_Key_Length, 0);
    std::copy(fullName.cbegin(), fullName.cend(), request_payload.begin());
    std::copy(public_key_value, public_key_value + CryptoHelper::Unsymmetric_Key_Length, request_payload.begin() + NameSize);

    //init request
    RequestHeader request_header;
    request_header.code = RequestCode::Register;
    request_header.payload_size = request_payload.size();

    //init response
    ResponseHeader response_header;
    std::vector<uint8_t> response_payload(0);

    if (!ConnectToServerHandler::RequestHandle(request_header, request_payload, &response_header, &response_payload))
    {
        std::cout << "error in connected to the server";
        return;
    }

    //check if the respone is contain what we excpected 
    if (response_header.code != ResponseCode::Registeration_Ok ||
        response_payload.size() != UidSize)
    {
        std::cout << "server responded with an error";
        return;
    }

    std::array<uint8_t, UidSize> uid = { 0 };
    std::copy(response_payload.cbegin(), response_payload.cend(), uid.begin());
    my_user = std::make_shared<MyUser>(fullName, uid, private_key);

    try
    {
        //write to file the response details
        MyUser::SaveMyUserToFile(my_user, myInfoFilePath);
    }
    catch (const std::exception&)
    {
        std::cerr << "Error: cannot save user information";
    }
}

void Operator::RequestClientsList()
{
    RequestHeader request_header(my_user->GetUid(), RequestCode::Clients_List, 0);
    std::vector<uint8_t> request_payload(0); // for empty payload

    ResponseHeader response_header;
    std::vector<uint8_t> response_payload(0);//init empty

    if (!ConnectToServerHandler::RequestHandle (request_header, request_payload, &response_header, &response_payload))
    {
        std::cout << "error in connected to the server";
        return;
    }

    if (response_header.code != ResponseCode::Request_Users_List)
    {
        std::cout << "server responded with an error";
        return;
    }

    //tritment of the response for usersList
    UpdateUsersListFormServer(response_payload);

    //print all list of user by operator <<
    std::cout << *usersList;
}

void Operator::RequestPublicKey()
{
    std::string userForAskKey;
    std::cout << "Enter name of user for know his public key: ";
    std::cin.ignore();
    std::getline(std::cin, userForAskKey);
    auto user = usersList->GetUserByName(userForAskKey);

    if (!user)//if this name doesnt exist
    {
        std::cout << "This user does not exists yet";
        return;
    }

    //init repuest
    std::vector<uint8_t> request_payload(user->GetUid().cbegin(), user->GetUid().cend());
    RequestHeader request_header(my_user->GetUid(), RequestCode::Public_Key, request_payload.size());
    
    //init response
    ResponseHeader response_header;
    std::vector<uint8_t> response_payload(0);
    
    //send request
    if (!ConnectToServerHandler::RequestHandle(request_header, request_payload, &response_header, &response_payload))
    { 
        std::cout << "error in connected to the server";
        return;
    }
    //tritment on response
    if (response_header.code != ResponseCode::Request_Public_Key)
    {
        std::cout << "server responded with an error";
        return;
    }
    //casting key
    auto public_key_response = reinterpret_cast<PublicKeyResponse*>(&response_payload[0]);
    
    //set key property
    user->SetPublicKey(CryptoHelper::PublicKeyCreation(public_key_response->key_value.data()));

    std::cout << "Your request send succesfully!\n";
}

void Operator::RequestWaitingMessages()
{
    //init request
    RequestHeader request_header(my_user->GetUid(), RequestCode::Waiting_Messages, 0);
    std::vector<uint8_t> request_payload(0); // empty payload

    //init response
    ResponseHeader response_header;
    std::vector<uint8_t> response_payload(0);

    //connect to server
    if (!ConnectToServerHandler::RequestHandle(request_header, request_payload, &response_header, &response_payload))
    {
        std::cout << "error in connected to the server";
        return;
    }

    //check response
    if (response_header.code != ResponseCode::Requested_Messages)
    {
        std::cout << "server responded with an error";
        return;
    }

    if (response_payload.size() > 0)
    {
        std::cout << HandleMessagesPayload(response_payload);
        return;
    }
    std::cout << "No messages is Watting for you. Sorry!!";
}

void Operator::SendRequestSymmetricKey()
{
    SendMessageToServer(MessageType::Key_Requst);
}

void Operator::SendTextMessage()
{
    SendMessageToServer(MessageType::Text);
}

void Operator::SendSymmetricKey()
{
    SendMessageToServer(MessageType::Key_Sending);
}

void Operator::UpdateUsersListFormServer(const std::vector<uint8_t>& payload)
{
    auto index = payload.cbegin();
    while (index < payload.cend())//cross over all paylod and to usersList 
    {
        std::array<uint8_t, UidSize> next_uid;
        std::copy(index, index + UidSize, next_uid.begin());
        std::string next_name(index + UidSize, index + (NameSize + UidSize));
        next_name.resize(std::strlen(next_name.c_str()));

        if (!usersList->GetUserByName(next_name))//if user not in the list, add to the list
        {
            usersList->AddUser(std::make_shared<OtherUser>(next_name, next_uid));
        }
        index += (NameSize + UidSize);
    }
}

void Operator::SendMessageToServer(MessageType mType)
{
    bool resultMessType = false;
    std::string userName;
    std::vector<uint8_t> content(0);
    std::cout << "To which user you want to send a message? Enter his name: ";
    std::cin.ignore();
    std::getline(std::cin, userName);
    //find user from list
    auto user = usersList->GetUserByName(userName);
    if (!user)// if user not found
    {
        std::cout << "User does not exists";
        return;
    }

    switch (mType)
    {
    case MessageType::Key_Requst:
        (& content)->clear();
        resultMessType = true;
        break;
    case MessageType::Key_Sending:
    {
        if (!user->GetPublicKey())
        {
            std::cout << "You must ask for public key before!\n";
            resultMessType = false;
            break;
        }
        CryptoPP::byte newKey[CryptoHelper::Symmetric_Key_Length];
        CryptoHelper::SymmetricKeyCreation(newKey);
        user->SetSymmetricKey(newKey);
        std::vector<CryptoPP::byte> clear_key(newKey, newKey + CryptoHelper::Symmetric_Key_Length);

        if (!CryptoHelper::EncryptUnsymmetricKey(user->GetPublicKey(), clear_key, &content))
        {
            std::cout << "Encryption Error";
            resultMessType = false;
            break;
        }

        resultMessType = true;
    }
        break;
    case MessageType::Text:
    {
        if (!user->GetSymmetricKey())
        {
            std::cout << "You must ask for public key before!\n";
            resultMessType = false;
            break;
        }
        std::string text, chiper_text;
        std::cout << "What is your message? Enter it: ";
        std::cin.ignore();
        std::getline(std::cin, text);
        if (!CryptoHelper::EncryptSymmetricKey(user->GetSymmetricKey(), text, &chiper_text))
        {
            std::cout << "Encryption Error";
            resultMessType = false;
            break;
        }
        (&content)->resize(chiper_text.length());
        std::copy(chiper_text.cbegin(), chiper_text.cend(), (&content)->begin());

        resultMessType = true;
    }
        break;
    default:
        break;
    }

    if (!resultMessType)
    {
        std::cout << "Your message faild in sending =(\n";
        return;
    }

    std::vector<uint8_t> request_payload(sizeof(MessageRequestHeader) + content.size());

    MessageRequestHeader sub_header;
    sub_header.uid = user->GetUid();
    sub_header.type = mType;
    sub_header.size = content.size();

    char sub_header_bytes[sizeof(MessageRequestHeader)] = { 0 };
    auto tmp = reinterpret_cast<MessageRequestHeader*>(sub_header_bytes); // in order to copy to header_bytes
    *tmp = sub_header;
    std::copy(sub_header_bytes, sub_header_bytes + sizeof(MessageRequestHeader), request_payload.begin());
    std::copy(content.cbegin(), content.cend(), request_payload.begin() + sizeof(MessageRequestHeader));

    RequestHeader request_header(my_user->GetUid(), RequestCode::Send_Text_Message, request_payload.size());

    ResponseHeader response_header;
    std::vector<uint8_t> response_payload(0);

    if (!ConnectToServerHandler:: RequestHandle(request_header, request_payload, &response_header, &response_payload))
    {
        std::cout << "error in connected to the server";
        return;
    }


    if (response_header.code != ResponseCode::Sending_Ok)
    {
        std::cout << "server responded with an error";
        return;
    }

    std::cout << "Your request send succesfully!\n";
}

std::string Operator::HandleMessagesPayload(std::vector<uint8_t>& payload)
{
    std::ostringstream output;
    size_t index = 0;
  
    while (index < payload.size())
    {   
        //check size payload
        if (payload.size() < (index + sizeof(MessageResponseHeader)))
        {
            return "Error in the payload data";
        }


        MessageResponseHeader* header = reinterpret_cast<MessageResponseHeader*>(&payload[index]);
        
        index += sizeof(MessageResponseHeader);
        auto user = usersList->GetUserById(header->uid);
        
        //when user not found
        if (!user)
        {
            output << "<3 <3 This user is unrecognaized. Maybe you can update your users list.\n\n";
            continue;
        }
        output << "FROM: " << user->GetName() << "\nContent:\n";

        switch (header->type)
        {
        case MessageType::Key_Sending:
        {
            //check size payload
            if (payload.size() < (index + header->size))
            {
                return "Error in the payload data";
            }

            std::vector<CryptoPP::byte> encryped_key(&payload[index], &payload[index] + header->size),
                decrypted_key;
            index += header->size;
            if (CryptoHelper::DecryptUnsymmetricKey(my_user->GetPrivateKey(), encryped_key, &decrypted_key))
            {
                user->SetSymmetricKey(&decrypted_key[0]);
                output << "symmetric key received\n";
            }
            else
            {
                output << "Can't decrypt message\n";

            }
            break;
        }
        case MessageType::Key_Requst:
        {
            output << "Request for for symmetric key\n";
            break;
        }
        case MessageType::Text:
        {
            if (payload.size() < (index + header->size))
            {
                return "Error in the payload data";
            }

            std::string content(&payload[index], &payload[index] + header->size);
            index += header->size;
            std::string decrypted_content;
            if (!user->GetSymmetricKey() || !CryptoHelper::DecryptSymmetricKey(user->GetSymmetricKey(), content, &decrypted_content))
            {
                decrypted_content = "Can't decrypt message";
            }
            output << decrypted_content << "\n";
            break;
        }
        default:
        {
            output << "message type is unrecognaize\n";
            break;
        }
        }
        output << "****************************************************\n\n";
    }
    return output.str();
}

