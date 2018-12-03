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
        else if("login" == buf)
        {
            Login login{};
            strcpy(login.userName, "cwj");
            strcpy(login.password, "cwj123");
            send(sock, (const char*)&login, sizeof(Login), 0);
            LoginResult loginRet{};
            recv(sock, (char*)&loginRet, sizeof(LoginResult), 0);
            LOG << "LoginResult: " << loginRet.result;
        }
        else if("logout" == buf)
        {
            Logout logout = {};
            strcpy(logout.useName, "cwj");
            send(sock, (const char*)&logout, sizeof(Logout), 0);
            LogoutResult logoutRet{};
            recv(sock, (char*)&logoutRet, sizeof(LogoutResult), 0);
            LOG << "LogoutResult: " << logoutRet.result;
        }
        else
        {
            LOG << "error command.\n";
        }
    }

    closesocket(sock);

    WSACleanup();

    LOG << "exit.\n";
    return 0;
//    return a.exec();
}
