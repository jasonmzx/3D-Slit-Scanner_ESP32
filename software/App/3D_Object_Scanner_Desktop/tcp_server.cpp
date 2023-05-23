#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstring>
#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <functional>


#pragma comment(lib, "Ws2_32.lib")

class Cleanup {
    std::function<void()> m_func;

public:
    explicit Cleanup(std::function<void()> func) : m_func(func) {} // Regular Constructor for executing void funcs w/ no params
    Cleanup(const Cleanup& other) = delete;             // Deleting Copy Constructor 
    Cleanup operator=(const Cleanup& other) = delete; // Deleting Re-Assignment Operator 

    //Cleanup Destructor to automatically run clean-up function upon 
    ~Cleanup() {
        m_func();
    }
};

//https://stackoverflow.com/questions/56919006/i-can-only-receive-one-word-winsock-c-tcp-ip-server-and-client


void showMat(cv::Mat m) {
    cv::imshow("mat", m);

    char k;

    k = cv::waitKey(0);

    if (k == 'ESC') return;
}


void tcp_server_main()
{

    AllocConsole();
    std::cout << "Server TCP/IP for incoming ESP-32 messages: ON PORT: 8887 " << std::endl;

    WSADATA wsa;

    WSAStartup(MAKEWORD(2, 2), &wsa); //???
    sockaddr_in server, client;

    Cleanup cleanup([] {
        WSACleanup();
        });

    //CREATION OF TCP SOCKET:

    SOCKET s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    // BINDING SOCKET ADDRESS:
    int bindStatus = bind(s, (sockaddr*)&server, sizeof(server));

    if (bindStatus) {
        std::cout << "Exited bindStatus" << std::endl;
        return;
    }
    else {
        std::cout << "Bind OK." << std::endl;
    }

    //Listen
    listen(s, 3);

    //Accept Incoming

    do
    {

        sockaddr_in from; // for the client
        int fromlen = sizeof(from);
        SOCKET clientS = accept(s, (sockaddr*)&from, &fromlen); //ASYNC FUNCTION !

        if (clientS == INVALID_SOCKET)
        {
            std::cout << "error with accept()" << std::endl;
            return;
        }
        std::cout << "User is connected." << std::endl;

        char recvbuf[4096];

        int bufferCount = 0;
        int imgHeight, imgWidth;

        std::vector<cv::Mat> matrixBuffer = {};
        std::vector<unsigned char> imgCharBuffer = {};


        do
        {
            // memset(recvbuf, 0, sizeof(recvbuf)); //Reset recvbuf by setting it all of it to 0
            char recvbuf[4096] = {};

            int iResult = recv(clientS, recvbuf, sizeof(recvbuf), 0); //

            if (iResult > 0)
            {
                send(clientS, recvbuf, iResult, 0);


                //In this else statement I'm loading in the actual IMG in chunks, this else statement is called (4-6) times per iMG load
                for (int z = 0; z < iResult; z++) {
                    imgCharBuffer.push_back((unsigned char)recvbuf[z]);
                }

                std::cout << "Pushed another chunk into the Matrix Buffer" << std::endl;

                //std::cout << "NEW LOOP: " << std::endl;
                //std::cout << recvbuf << std::endl;
                //std::cout << "buffSize: " << imgCharBuffer.size() << std::endl;
            }
            else if (imgCharBuffer.size() != 0) {

                std::cout << "Iterating thru vec<char>" << imgCharBuffer.size() << std::endl;
                for (auto c : imgCharBuffer) {
                    std::cout << c;
                }

                auto casted = cv::imdecode(imgCharBuffer, cv::IMREAD_COLOR); //Potential Errors with this 

                //Now casted is a Mat type (cv::Mat)
                matrixBuffer.push_back(casted);
                std::cout << "Matrix pushed into MBuffer !" << std::endl;
                bufferCount = 0;
                showMat(casted);
                closesocket(clientS);
                break;

            }
            else {
                std::printf("elsing\n");
                closesocket(clientS);
                break;
            }

            bufferCount++;
        } while (true);

    } while (true);

}