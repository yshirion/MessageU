#pragma once
#include "Utilities.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <fstream>
#include <iostream>
#include <string>

class ConnectToServerHandler
{

public:
	ConnectToServerHandler();

	bool static ReadServerDetails();

	//handeling requests and response
	bool static ReceiveResponse(const SOCKET& sock, ResponseHeader* response_header, std::vector<uint8_t>* response_payload);
	bool static SendRequest(const SOCKET& sock, const RequestHeader& request_header, const std::vector<uint8_t>& request_payload);
	bool static RequestHandle(const RequestHeader& request_header, const std::vector<uint8_t>& request_payload, ResponseHeader* response_header, std::vector<uint8_t>* response_payload);

private:

	static std::string server_ip;
	static unsigned int port;

	static std::string ReadFromFile(std::string fileName);


	static constexpr auto serverFilePath = "server.info";

};

