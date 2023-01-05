#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstring>
#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>


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

    int bufferCount = 0;

    int imgHeight, imgWidth;

    std::vector<unsigned char> imgCharBuffer = {};

    do
    {
        memset(recvbuf, 0, sizeof(recvbuf)); //Reset recvbuf
        int iResult = recv(clientS, recvbuf, sizeof(recvbuf), 0); //

        if (iResult > 0)
        {
            send(clientS, recvbuf, iResult, 0);

            if (bufferCount == 0) { //Width
                imgWidth = ((int*)recvbuf)[0];
            }else if (bufferCount == 1) { //Height
                imgHeight = ((int*)recvbuf)[0];
            }
            else {


                int bufferLength = sizeof(recvbuf) / sizeof(recvbuf[0]);

                for (int z = 0; z < bufferLength; z++) {
                    imgCharBuffer.push_back((unsigned char)recvbuf[z]);
                }

                //Loop over character array & stuff

      /*          for (int i = 0; i < sizeof(&recvbuf) / sizeof(const char*); i++) {
                    std::cout << "a" << std::endl;
                    imgCharBuffer.push_back(recvbuf[i]);
                }*/



                std::cout << "NEW LOOP: " << std::endl;
                std::cout << recvbuf << std::endl;
                std::cout << "buffSize: " << imgCharBuffer.size() << std::endl;
            }
        }
        else {

            std::cout << "Iterating thru vec<char>" << std::endl;
            for (int j = 0; j < imgCharBuffer.size(); j++) {
                std::cout << imgCharBuffer[j];
            }

            std::cout << "Cleaning and exitting?" << std::endl;
            cv::Mat casted = cv::Mat(imgCharBuffer);
            cv::imshow("Raw Difference (No Pre-processing)", casted);

            cv::waitKey(0);
            WSACleanup();
            return;
        }
        
        bufferCount++;
    } while (true);


}