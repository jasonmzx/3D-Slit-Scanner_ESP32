#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstring>

#pragma comment(lib, "Ws2_32.lib")

//https://stackoverflow.com/questions/56919006/i-can-only-receive-one-word-winsock-c-tcp-ip-server-and-client

void tcp_server_main()
{
    AllocConsole();
    std::cout << "Server tcp/ip test V.14.0 XXXX" << std::endl;

    WSADATA wsa;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    sockaddr_in server, client;

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);
    // bind
    int bindStatus = bind(s, (sockaddr*)&server, sizeof(server));
    if (bindStatus) {
        std::cout << "Exited bindStatus" << std::endl;
        return;
    }
    else {
        std::cout << "OK Bind" << std::endl;
    }

    //listen 
    listen(s, 3);
    
    //Accept

    sockaddr_in from; // for the client
    int fromlen = sizeof(from);
    
    SOCKET clientS = accept(s, (sockaddr*)&from, &fromlen); //ASYNC FUNCTION !

    if (clientS == INVALID_SOCKET)
    {
        std::cout << "error with accept()" << std::endl;
        WSACleanup();
        return;
    }

    std::cout << "User is connected." << std::endl;

    char recvbuf[512];

    do
    {
        memset(recvbuf, 0, sizeof(recvbuf)); //Reset recvbuf
        int iResult = recv(clientS, recvbuf, 512, 0);

        if (iResult > 0)
        {
            send(clientS, recvbuf, iResult, 0);

            std::cout << recvbuf << std::endl;
        }
        else {
            WSACleanup();
            return;
        }

    } while (true);

}