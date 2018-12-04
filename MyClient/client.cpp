#include <QCoreApplication>
#include "util.h"
#include "MessageHeader.hpp"
#include "easytcpclient.hpp"

bool g_bRun = false;
void cmdThread(EasyTcpClient* client)
{
    while(true)
    {
        string buf;
        cin >> buf;
        if( buf=="exit")
        {
            client->close();
            LOG << "thread exit.\n";
            break;
        }
        else if(buf=="login")
        {
            Login login;
            strcpy(login.userName, "cwj");
            strcpy(login.password, "cwj123");
            client->sendData(&login);
        }
        else if(buf=="logout")
        {
            Logout logout;
            strcpy(logout.useName, "cwj");
            client->sendData(&logout);
        }
        else
        {
            LOG << "cmd error.\n";
        }
    }
}

int main(int argc, char *argv[])
{
    EasyTcpClient client;
    client.connect(SERVER_IP, SERVER_PORT);

    std::thread t0(cmdThread, &client);
    t0.detach();

    while(client.isRun())
    {
        client.onRun();
    }

    client.close();

    LOG << "exit.\n";
    return 0;
    //    return a.exec();
}
