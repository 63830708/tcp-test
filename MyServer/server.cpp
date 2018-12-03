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
    while(true)
    {
        DataHeader header = {};
        int nLen = recv(sockClient, (char*)&header, sizeof(DataHeader), 0);
        if(nLen <= 0)
        {
            LOG << "recv()<=0 \n";
            break;
        }

        LOG << "recv: cmd-" << header.cmd << "len-" << header.len;
        switch(header.cmd)
        {
        case CMD_LOGIN:
        {
            Login login{};
            recv(sockClient, (char*)&login+sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
                LOG << "recv: login    username-" << login.userName << "pwd-" << login.password;
            LoginResult ret{1};
            send(sockClient, (char*)&ret, sizeof(LoginResult), 0);
        }
            break;
        case CMD_LOGOUT:
        {
            Logout logout{};
            recv(sockClient, (char*)&logout+sizeof(DataHeader), sizeof(Logout)-sizeof(DataHeader), 0);
                LOG << "recv: logout    username-" << logout.useName;
            LogoutResult ret{1};
            send(sockClient, (char*)&ret, sizeof(LogoutResult), 0);
        }
            break;
        default:
            header.cmd = CMD_ERROR;
            header.len = 0;
            send(sockClient, (char*)&header, sizeof(DataHeader), 0);
            break;
        }

    }

    closesocket(sockClient);

    WSACleanup();

    LOG << "exit" ;





    return 0;
//    return a.exec();
}
