#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <cstring>
#include <vector>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <functional>

#include <random>
#include <ctime>
#include <sstream>
#include <iomanip>

#include <Windows.h>
#include <fstream>

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


std::string generateRandomFilename() {
    const std::string charset = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    const int length = 10;

    std::mt19937 rng(static_cast<unsigned int>(time(nullptr)));
    std::uniform_int_distribution<int> distribution(0, charset.size() - 1);

    std::stringstream ss;
    for (int i = 0; i < length; ++i) {
        ss << charset[distribution(rng)];
    }

    return ss.str();
}

bool folderExists(const std::string& folderPath)
{
    DWORD fileAttributes = GetFileAttributesA(folderPath.c_str());
    return (fileAttributes != INVALID_FILE_ATTRIBUTES && (fileAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool createFolder(const std::string& folderPath)
{
    if (!CreateDirectoryA(folderPath.c_str(), NULL))
    {
        std::cout << "Failed to create the folder: " << folderPath << std::endl;
        return false;
    }

    return true;
}


void showMat(cv::Mat m) {
    cv::imshow("mat", m);

    char k;

    k = cv::waitKey(0);

    if (k == 'ESC') return;
}

bool saveMat(cv::Mat m, std::string folderPath, int iter, std::string angleStr) {
    if (m.empty()) {
        std::cout << "Error decoding the image." << std::endl;
        return 1;
    }
    else {
        std::string filename = folderPath + '/' + std::to_string(iter) + '_' + angleStr + ".jpg";
        bool saveResult = cv::imwrite(filename, m);

        if (!saveResult)
        {
            std::cout << "Error saving the image as " << filename << std::endl;
            return 1;
        }
        else
        {
            std::cout << "Image saved as " << filename << std::endl;
            return 0;
        }
    }
}



void tcp_server_main()
{
    //& Initialize Data Folder

    std::string instanceFolder = generateRandomFilename();
    std::string folderPath = "../../../data/" + instanceFolder;

    if (folderExists(folderPath))
    {
        std::cout << "The folder already exists: " << folderPath << std::endl;
    }
    else
    {
        if (createFolder(folderPath))
        {
            std::cout << "Folder created successfully: " << folderPath << std::endl;
        }
    }

    //& Allocate Console (for thread):

    AllocConsole();
    std::cout << "Server TCP/IP for incoming ESP-32 messages: ON PORT: 8888 " << std::endl;

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

    //Listen with SOCKET s
    listen(s, 3);

    //Accept Incoming

    std::vector<cv::Mat> tempMatrixBuffer = {};

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

        std::vector<unsigned char> imgCharBuffer = {};


        do
        {
            // memset(recvbuf, 0, sizeof(recvbuf)); //Reset recvbuf by setting it all of it to 0
            char recvbuf[4096] = {};

            int iResult = recv(clientS, recvbuf, sizeof(recvbuf), 0); //

            //while iResult isn't 0, imgCharBuffer keeps recieving pushes

            if (iResult > 0)
            {
                send(clientS, recvbuf, iResult, 0);


                //In this else statement I'm loading in the actual IMG in chunks, this else statement is called (4-6) times per iMG load
                for (int z = 0; z < iResult; z++) {
                    imgCharBuffer.push_back((unsigned char)recvbuf[z]);
                }
                std::cout << "Pushed another chunk into the Matrix Buffer" << std::endl;

            }


            else if (imgCharBuffer.size() != 0) {

                //Assert for the case where Angle Information is being sent in after the last IMG:

                if (imgCharBuffer.size() < 3) {
                    
                    std::cout << "########## IMAGE ACKNOWLEDGEMENT ##########" << std::endl;

                    // Angle String:
                    std::string angleStr(imgCharBuffer.begin(), imgCharBuffer.end());



                    std::cout << "Matrix Buffer Size: " << tempMatrixBuffer.size() << std::endl;

                    for (int i = 0; i < tempMatrixBuffer.size(); i++) {
                        const cv::Mat& mat = tempMatrixBuffer[i];
                        saveMat(mat, folderPath, i, angleStr);
                    }

                    tempMatrixBuffer = {};
                        
                    break;

                }

                std::cout << "Iterating thru vec<char>" << imgCharBuffer.size() << std::endl;



                for (auto c : imgCharBuffer) {
                    std::cout << c;
                }

                auto casted = cv::imdecode(imgCharBuffer, cv::IMREAD_COLOR); //Potential Errors with this 

                

                //Now casted is a Mat type (cv::Mat)
                tempMatrixBuffer.push_back(casted);
                
                std::cout << "CASTED TO MATRIX !" << std::endl;
               
                bufferCount = 0;
                showMat(casted);

                //Read for the Angle

                closesocket(clientS);
                
                break;

            }

            //Nothing is being sent from the ESP's End
            else {
                std::printf("elsing\n");
                closesocket(clientS);
                break;
            }

            bufferCount++;
        } while (true);

    } while (true);

}