#include <QCoreApplication>
#include "util.h"
#include "MessageHeader.hpp"
#include "easytcpclient.hpp"
#include <memory>

bool g_bRun = true;
void cmdThread()
{
    while(true)
    {
        string buf;
        cin >> buf;
        if(buf == "exit")
        {
            g_bRun = false;
            break;
        }
    }
}

//void cmdThread(EasyTcpClient* client)
//{
//    while(true)
//    {
//        string buf;
//        cin >> buf;
//        if( buf=="exit")
//        {
//            client->close();
//            LOG << "thread exit.\n";
//            break;
//        }
//        else if(buf=="login")
//        {
//            Login login;
//            strcpy(login.userName, "cwj");
//            strcpy(login.password, "cwj123");
//            client->sendData(&login);
//        }
//        else if(buf=="logout")
//        {
//            Logout logout;
//            strcpy(logout.useName, "cwj");
//            client->sendData(&logout);
//        }
//        else
//        {
//            LOG << "cmd error.\n";
//        }
//    }
//}


const int MAX_COUNT = 1;
int main(int argc, char *argv[])
{
    std::vector<std::shared_ptr<EasyTcpClient> > vec;
    vec.reserve(MAX_COUNT);
    for(int i=0; i<MAX_COUNT; ++i)
    {
        auto s = std::make_shared<EasyTcpClient>();
        if(SOCKET_ERROR == s->connect(SERVER_IP, SERVER_PORT))
        {
            LOG << "conn failed: " << i << endl;
            continue;
        }
        vec.push_back(s);
    }


//    EasyTcpClient client;
//    client.connect(SERVER_IP, SERVER_PORT);

    std::thread t0(cmdThread);
    t0.detach();

    Login login;
    strcpy(login.userName, "cwj");
    strcpy(login.password, "cwj123");

    int count = 0;
    while(g_bRun)
    {
        for(auto s: vec)
        {
            s->sendData(&login) ;
        }
        LOG << "loop \t" <<count++ << endl;
    }

    for(auto s: vec)
    {
        s->close();
    }

    vec.clear();

    LOG << "exit.\n";
    string str;
    cin >> str;
    return 0;
    //    return a.exec();
}
