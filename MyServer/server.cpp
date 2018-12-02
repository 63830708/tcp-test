#include <QCoreApplication>
#include "util.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 初始化库
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    // 建立socket
    SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // 绑定地址
    sockaddr_in addrServer{};
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(SERVER_PORT);
    addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP); // INADDR_ANY
    if(SOCKET_ERROR == bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr_in)))
    {
        LOG << "bind() failed." << errno;
        return -1;
    }
    else
    {
        LOG << "bind() succeed.\n";
    }

    // 监听
    if(SOCKET_ERROR == listen(sockServer, 8))
    {
        LOG << "listen() failed.\n";
        return -1;
    }
    else
    {
        LOG << "listen() succeed.\n";
    }
    // 等待并接受客户端连接
    sockaddr_in addrClient{};
    int nAddrLen = sizeof(sockaddr_in);
    SOCKET sockClient = INVALID_SOCKET;
    sockClient = accept(sockServer, (sockaddr*)&addrClient, &nAddrLen);
    if(INVALID_SOCKET == sockClient)
    {
        LOG << "accept() failed.\n";
        return -1;
    }
    else
    {
        LOG << "client accepted: socket-" << hex<< (int)sockClient
            << "\tip-" << dec << inet_ntoa(addrClient.sin_addr)
            << ":" << addrClient.sin_port;
    }

    // 接收消息并反馈
    char recvBuf[1024] = {0};
    string buf;
    buf.reserve(1024);
    while(true)
    {
        int nLen = recv(sockClient, recvBuf, 1024, 0);
        if(nLen <= 0)
        {
            LOG << "recv()<=0 \n";
            break;
        }

        LOG << "recv:"<<recvBuf;
        if(strcmp(recvBuf, "getInfo") == 0)
        {
            Data p{80, "zhangsan"};
            send(sockClient, (const char*)&p, sizeof(Data), 0);
        }
        else
        {
            const char msg[]{"?"};
            send(sockClient, msg, sizeof(msg), 0);
        }
    }

    closesocket(sockClient);

    WSACleanup();

    LOG << "exit" ;





    return 0;
//    return a.exec();
}
