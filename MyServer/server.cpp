#include <QCoreApplication>
#include <vector>
#include <algorithm>
#include <functional>
#include "util.h"


std::vector<SOCKET> g_clients;

int processor(SOCKET sockClient)
{
    // 接收消息并反馈
    DataHeader header = {};
    int nLen = recv(sockClient, (char*)&header, sizeof(DataHeader), 0);
    if(nLen <= 0)
    {
        LOG << "recv()<=0 \n";
        return -1;
    }

    LOG << "recv: cmd-" << header.cmd << "len-" << header.len << endl;
    switch(header.cmd)
    {
    case CMD_LOGIN:
    {
        Login login{};
        recv(sockClient, (char*)&login+sizeof(DataHeader), sizeof(Login)-sizeof(DataHeader), 0);
        LOG << "recv: login    username-" << login.userName << "pwd-" << login.password << endl;
        LoginResult ret{1};
        send(sockClient, (char*)&ret, sizeof(LoginResult), 0);
    }
        break;
    case CMD_LOGOUT:
    {
        Logout logout{};
        recv(sockClient, (char*)&logout+sizeof(DataHeader), sizeof(Logout)-sizeof(DataHeader), 0);
        LOG << "recv: logout    username-" << logout.useName << endl;
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

    return 0;
}

void closeSocket(SOCKET s)
{
#ifdef _WIN32
    closesocket(s);
#else
    close(s);
#endif
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
#ifdef _WIN32
    // 初始化库
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
    // 建立socket
    SOCKET sockServer = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    // 绑定地址
    sockaddr_in addrServer{};
    addrServer.sin_family = AF_INET;
    addrServer.sin_port = htons(SERVER_PORT);
#ifdef _WIN32
    addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP); // INADDR_ANY
#else
    addrServer.sin_addr.s_addr = inet_addr(SERVER_IP);
#endif
    if(SOCKET_ERROR == bind(sockServer, (sockaddr*)&addrServer, sizeof(sockaddr_in)))
    {
        LOG << "bind() failed." << errno << endl;
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
    fd_set fdRead;
    fd_set fdWrite;
    fd_set fdExp;
    while(true)
    {
        FD_ZERO(&fdRead);
        FD_ZERO(&fdWrite);
        FD_ZERO(&fdExp);

        FD_SET(sockServer, &fdRead);
        FD_SET(sockServer, &fdWrite);
        FD_SET(sockServer, &fdExp);

        for(int i=g_clients.size()-1; i>=0; i--)
        {
            FD_SET(g_clients[i], &fdRead);
        }

        timeval t{0, 0};
        //        LOG << "start select.";
        int ret = select(sockServer+1, &fdRead, &fdWrite, &fdExp, 0);//&t);
        //        LOG << "end   select.";
        if(ret < 0)
        {
            LOG << "select() failed.";
            break;
        }

        // 是否有客户端接入
        if( FD_ISSET(sockServer, &fdRead))
        {
            FD_CLR(sockServer, &fdRead);
            sockaddr_in addrClient{};
            int nAddrClientLen = sizeof(sockaddr_in);
            SOCKET sockClient = INVALID_SOCKET;
#ifdef _WIN32
            sockClient = accept(sockServer, (sockaddr*)&addrClient, &nAddrClientLen);
#else
            sockClient = accept(sockServer, (sockaddr*)&addrClient, (socklen_t *)&nAddrClientLen);
#endif
            if(INVALID_SOCKET == sockClient)
            {
                LOG << "accept() failed.\n" << errno;
            }
            else
            {
                for(auto s: g_clients)
                {
                    NewUserJoin userJoin;
                    send(s, (const char*)&userJoin, sizeof(NewUserJoin), 0);
                }
                g_clients.push_back(sockClient);
                LOG << "client accepted: socket-" << hex<< (int)sockClient
                    << "\tip-" << dec << inet_ntoa(addrClient.sin_addr)
                    << ":" << addrClient.sin_port<< endl;
            }
        }
        // 处理可读的socket
        for (int n = (int)g_clients.size()-1; n >= 0; n--)
        {
            if(FD_ISSET(g_clients[n], &fdRead))
            {
                if(-1 == processor(g_clients[n]))
                {
                    closeSocket(g_clients[n]);
                    auto iter = g_clients.begin()+n;
                    //                    if(iter!=g_clients.end())
                    {
                        g_clients.erase(iter);
                    }
                }
            }
        }

    }

    for( auto s: g_clients)
    {
        closeSocket(s);
    }
#ifdef _WIN32
    closesocket(sockServer);
    WSACleanup();
#else
    close(sockServer);
#endif


    LOG << "exit" ;



    return 0;
    //    return a.exec();
}
