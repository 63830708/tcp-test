#include <QCoreApplication>
#include "util.h"


int processor(SOCKET sock)
{
    DataHeader header = {};
    int nLen = recv(sock, (char*)&header, sizeof(DataHeader), 0);
    if(nLen <= 0)
    {
        LOG << "recv()<=0 \n";
        return -1;
    }

    LOG << "recv: cmd-" << header.cmd << "len-" << header.len << endl;
    switch(header.cmd)
    {
    case CMD_LOGIN_RESULT:
    {
        LoginResult loginRet{};
        recv(sock, (char*)&loginRet+sizeof(DataHeader), sizeof(LoginResult)-sizeof(DataHeader), 0);
        LOG << "recv: login result-" << loginRet.result << endl;
    }
        break;
    case CMD_LOGOUT_RESULT:
    {
        LogoutResult logoutRet{1};
        recv(sock, (char*)&logoutRet+sizeof(DataHeader), sizeof(LogoutResult)-sizeof(DataHeader), 0);
        LOG << "recv: logout result-" << logoutRet.result << endl;
    }
        break;
    case CMD_NEW_USER_JOIN:
    {
        NewUserJoin userJoin{};
        recv(sock, (char*)&userJoin+sizeof(DataHeader), sizeof(NewUserJoin)-sizeof(DataHeader), 0);
        LOG << "recv: newUser    username-" << userJoin.scok << endl;
    }
        break;
    default:
        break;
    }

    return 0;
}




bool g_bRun = false;
void cmdThread(SOCKET sock)
{
    while(true)
    {
        string buf;
        cin >> buf;
        if( buf=="exit")
        {
            g_bRun = false;
            LOG << "thread exit.\n";
            break;
        }
        else if(buf=="login")
        {
            Login login;
            strcpy(login.userName, "lyd");
            strcpy(login.password, "lydmm");
            send(sock, (const char*)&login, sizeof(Login), 0);
        }
        else if(buf=="logout")
        {
            Logout logout;
            strcpy(logout.useName, "lyd");
            send(sock, (const char*)&logout, sizeof(Logout), 0);
        }
        else
        {
            LOG << "cmd error.\n";
        }
    }
}

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

    g_bRun = true;
    std::thread t0(cmdThread, sock);
    t0.detach();

    while(g_bRun)
    {
        fd_set fdReads;
        FD_ZERO(&fdReads);
        FD_SET(sock, &fdReads);
        timeval t{0, 0};
        int ret = select(sock, &fdReads, 0, 0, &t);
        if(ret < 0)
        {
            LOG << "select() failed.\n";
            break;
        }

        if(FD_ISSET(sock, &fdReads))
        {
            FD_CLR(sock, &fdReads);
            if( -1 == processor(sock))
            {
                closesocket(sock);
                break;
            }
        }
    }

    closesocket(sock);

    WSACleanup();

    LOG << "exit.\n";
    return 0;
    //    return a.exec();
}
