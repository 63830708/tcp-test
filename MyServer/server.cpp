#include <QCoreApplication>
#include <vector>
#include <algorithm>
#include <functional>
#include <thread>
#include "util.h"
#include "MessageHeader.hpp"
#include "easytcpserver.h"


bool g_bRun = true;
void cmdThread()
{
    while(g_bRun)
    {
        string buf;
        cin >> buf;
        if(buf=="exit")
        {
            g_bRun = false;
            break;
        }
    }
}

int main(int argc, char *argv[])
{

    EasyTcpServer server;

    if(SOCKET_ERROR == server.bind(SERVER_IP, SERVER_PORT))
    {
//        LOG << "bind() failed." << errno << endl;
        return -1;
    }
//    else
//    {
//        LOG << "bind() succeed.\n";
//    }

    // 监听
    if(SOCKET_ERROR == server.listen(50))
    {
//        LOG << "listen() failed.\n";
        return -1;
    }
//    else
//    {
//        LOG << "listen() succeed.\n";
//    }

    std::thread t0(cmdThread);
    t0.detach();


    while(g_bRun)
    {
        if(server.isRun())
        {
            server.onRun();
        }
        else
        {

        }
    }

    server.close();

    LOG << "exit" ;



    return 0;
    //    return a.exec();
}
