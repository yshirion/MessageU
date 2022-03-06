// Main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include "Operator.h"

enum Choices
{
    Register = 110,
    Clients_List = 120,
    Public_Key = 130,
    Waiting_Messages = 140,
    Send_Text_Message = 150,
    Send_Request_Symmetric_Key = 151,
    Send_Symmetric_Key = 152,
    Exit = 0
};


int main()
{
    bool isRegBefore = false;
    //declare on operator pointer
    std::shared_ptr<Operator> op(new Operator);

    //read servers detiles
    if (!op->ReadServerDetails())
    {
        std::cout << "MessageU system could not find the server info,please try again later.";
        exit(EXIT_FAILURE);
    }
    
    std::string instractions = "\n\nMessageU client at your service.\n";
    instractions += "110) Register\n";
    instractions += "120) Request for clients list\n";
    instractions += "130) Request for public key\n";
    instractions += "140) Request for waiting messages\n";
    instractions += "150) Send a text message\n";
    instractions += "151) Send a request for symmetric key\n";
    instractions += "152) Send your symmetric key\n";
    instractions += "0) Exit client\n\n";
    std::string error_reg = "You need to register, and after select it again.\n";
    std::string choice;
    int choice_num = 0;

    do {
        
        //for checking before each choice
        isRegBefore = op->IsRegisterBefore();

            std::cout << instractions << "Please enter your choice: ";
            std::cin >> choice;
        try{
            choice_num = static_cast<Choices>(std::stoi(choice.c_str()));
        }
        catch (std::exception ex)
        {
            choice_num = -1; //for error input
        }
        try {
            
            switch (choice_num)
            {
            case Register:
                if (!isRegBefore)
                    op->Register();
                else
                    std::cout << "You alredy registered before.\n";
                break;
            case Clients_List:
                if (isRegBefore)
                    op->RequestClientsList();
                else
                    std::cout << error_reg;
                break;
            case Public_Key:
                if (isRegBefore)
                    op->RequestPublicKey();
                else
                    std::cout << error_reg;
                break;

            case Waiting_Messages:
                if (isRegBefore)
                    op->RequestWaitingMessages();
                else
                    std::cout << error_reg;
                break;

            case Send_Text_Message:
                if (isRegBefore)
                    op->SendTextMessage();
                else
                    std::cout << error_reg;
                break;

            case Send_Request_Symmetric_Key:
                if (isRegBefore)
                    op->SendRequestSymmetricKey();
                else
                    std::cout << error_reg;
                break;

            case Send_Symmetric_Key:
                if (isRegBefore)
                    op->SendSymmetricKey();
                else
                    std::cout << error_reg;
                break;

            case Exit:
                return EXIT_SUCCESS;

            default:
                std::cout << "Invalid option\n\n";
                break;

            }
        }
        catch (std::exception ex)
        {
            std::cout << "server responded with an error\n";
        }

    } while (true);


    return 0;
}

