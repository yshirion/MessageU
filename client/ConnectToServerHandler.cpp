#include "ConnectToServerHandler.h"

 std::string ConnectToServerHandler::server_ip;
 unsigned int ConnectToServerHandler::port;

ConnectToServerHandler::ConnectToServerHandler()
{
    
}

bool ConnectToServerHandler::ReceiveResponse(const SOCKET& sock, ResponseHeader* response_header, std::vector<uint8_t>* response_payload)
{
    char  response_header_bytes[sizeof(ResponseHeader)] = { 0 };
    int recv_size = 0;
    
    if ((recv_size = recv(sock, response_header_bytes, sizeof(ResponseHeader), 0)) == SOCKET_ERROR)
    {
        std::cout << "Receving Response is Failed.\n";
        return false;
    }
    *response_header = *(reinterpret_cast<ResponseHeader*>(response_header_bytes));

    if (response_header->payload_size)
    {
        char* response_payload_bytes = new char[response_header->payload_size]{ 0 };
        if ((recv_size = recv(sock, response_payload_bytes, response_header->payload_size, 0)) == SOCKET_ERROR)
        {
            std::cout << "Receving Response is Failed.\n";
            return false;
        }

        response_payload->resize(response_header->payload_size);
        std::copy(response_payload_bytes, response_payload_bytes + response_header->payload_size, response_payload->begin());
        delete[] response_payload_bytes;
    }

    return true;
}


bool ConnectToServerHandler::SendRequest(const SOCKET& sock, const RequestHeader& request_header, const std::vector<uint8_t>& request_payload)
{

    std::vector<uint8_t> request(sizeof(RequestHeader) + request_payload.size());
    char* header_bytes = new char[sizeof(RequestHeader)]{ 0 };
    auto tmp = reinterpret_cast<RequestHeader*>(header_bytes); // in order to copy to header_bytes
    *tmp = request_header;
    std::copy(header_bytes, header_bytes + sizeof(RequestHeader), request.begin());
    std::copy(request_payload.cbegin(), request_payload.cend(), request.begin() + sizeof(RequestHeader));


    if (send(sock, reinterpret_cast<char*>(&request[0]), request.size(), 0) != request.size())
    {
        std::cout << "Sending Request is Failed.\n";
        return false;
    }

    return true;
}

bool ConnectToServerHandler::RequestHandle(const RequestHeader& request_header, const std::vector<uint8_t>& request_payload,
                                            ResponseHeader* response_header, std::vector<uint8_t>* response_payload )
{
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        std::cout << "WSAStartup Failed.\n";
        return false;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        std::cout << "Creating socket Failed.\n";
        return false;
    }
    /// TODO: pickup address from file 
    inet_pton(AF_INET, server_ip.c_str(), &server.sin_addr.s_addr);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
    {
        std::cout << "Connection Failed.\n";
        return false;
    }
    bool success = true;

    success &= SendRequest(sock, request_header, request_payload);

    if (success)
    {
        success &= ReceiveResponse(sock, response_header, response_payload);
    }

    closesocket(sock);
    WSACleanup();

    return success;
}

bool ConnectToServerHandler::ReadServerDetails()
{
    std::string line = ReadFromFile(serverFilePath);

    if (line == "")
        return false;
    std::string splitline;
    auto colon_index = line.find(':');
    if (colon_index == std::string::npos)
    {
        return false;
    }
    server_ip = line.substr(0, colon_index);

    try
    {
        port = std::stoul(line.substr(colon_index + 1, line.length()));

    }
    catch (const std::invalid_argument&)
    {
        return false;
    }

    return true;
}

std::string ConnectToServerHandler::ReadFromFile(std::string fileName)
{
    std::string sLine = "";
    try {

        std::ifstream infile(fileName);

        if (infile.is_open())
        {
            getline(infile, sLine);
        }

        infile.close();
    }
    catch (std::exception ex)
    {
        std::cout << "error reading file " << fileName;
        return "";
    }

    return sLine;
}