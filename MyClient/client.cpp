#include <QCoreApplication>
#include "util.h"






int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if( INVALID_SOCKET == sock)
    {
        LOG << "socket() failed.\n";
        return -1;
    }
    else
    {
        LOG << "socket() succeed.\n";
    }

    sockaddr_in  addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.S_un.S_addr = inet_addr(SERVER_IP);
    int ret = connect(sock, (sockaddr*)&addr, sizeof(sockaddr_in));
    if(SOCKET_ERROR == ret)
    {
        LOG << "connect() failed.\n";
        return -1;
    }
    else
    {
        LOG << "connect() succeed.\n";
    }

    while(true)
    {
//        char buf[1024] = {0};
        string buf;
        cin >> buf;
        if("exit" == buf)
        {
            LOG << "about to exit.\n";
            break;
        }
//        else if(buf.substr(0,1)=="0")
//        {
//            send(sock, buf.c_str(), 0, 0);
//        }
        else
        {
            send(sock, buf.c_str(), buf.length(), 0);
        }

        char recvBuf[1024] = {0};
        int nLen = recv(sock, recvBuf, 1024, 0);
        if(nLen>0)
        {
            Data *p = (Data*)recvBuf;
            LOG << nLen <<"\t" << p->age << p->name ;
        }
    }

//    closesocket(sock);

    WSACleanup();

    LOG << "exit.\n";
    return 0;
//    return a.exec();
}
