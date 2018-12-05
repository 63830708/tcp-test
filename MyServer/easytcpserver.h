#ifndef _EasyTcpServer_hpp_
#define _EasyTcpServer_hpp_

#include "util.h"
#include "MessageHeader.hpp"
#include "Timestamp.hpp"
#define RECV_BUFF_SZIE 10240

class ClientSocket
{
public:
    ClientSocket(SOCKET s=INVALID_SOCKET)
    {
        sock = s;
        memset(szMsgBuf, 0, sizeof(szMsgBuf));
        _lastPos = 0;
    }
    SOCKET socket(){return sock;}
    char* msgBuf() { return szMsgBuf;}

    int getLastPos()
    {
        return _lastPos;
    }
    void setLastPos(int pos)
    {
        _lastPos = pos;
    }

private:
    SOCKET sock;
    char szMsgBuf[RECV_BUFF_SZIE*10];
    int _lastPos;
};

class EasyTcpServer
{
    SOCKET sock;
    std::vector<ClientSocket*> g_clients;
    Timestamp time;
    int recvCount;
public:
    EasyTcpServer()
    {
        sock = INVALID_SOCKET;
        recvCount = 0;
    }
    virtual ~EasyTcpServer()
    {
        close();
    }

    void initSocket()
    {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
        if(INVALID_SOCKET != sock)
        {
            close();
        }

        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(INVALID_SOCKET==sock)
        {
            LOG << "socket() failed.\n";
        }
        else
        {
            LOG << "socket() succeed.\n";
        }
    }

    int bind(const char* ip, unsigned short port)
    {
        if(INVALID_SOCKET==sock)
        {
            initSocket();
        }

        sockaddr_in addrServer{};
        addrServer.sin_family = AF_INET;
        addrServer.sin_port = htons(SERVER_PORT);
#ifdef _WIN32
        if(ip)
            addrServer.sin_addr.S_un.S_addr = inet_addr(SERVER_IP); // INADDR_ANY
        else
            addrServer.sin_addr.S_un.S_addr = INADDR_ANY; //
#else
        if(ip)
            addrServer.sin_addr.s_addr = inet_addr(SERVER_IP);
        else
            addrServer.sin_addr.s_addr = INADDR_ANY;
#endif
        int ret = ::bind(sock, (sockaddr*)&addrServer, sizeof(sockaddr_in));
        if(SOCKET_ERROR == ret)
        {
            LOG << "bind() failed." << errno << endl;
        }
        else
        {
            LOG << "bind() succeed.\n";
        }
        return ret;
    }

    int listen(int n)
    {
        int ret = ::listen(sock, n);
        if(SOCKET_ERROR == ret)
        {
            LOG << "listen() failed." << errno << endl;
        }
        else
        {
            LOG << "listen() succeed.\n";
        }
        return ret;
    }

    SOCKET accept()
    {
        sockaddr_in addrClient{};
        int nAddrClientLen = sizeof(sockaddr_in);
        SOCKET sockClient = INVALID_SOCKET;
#ifdef _WIN32
        sockClient = ::accept(sock, (sockaddr*)&addrClient, &nAddrClientLen);
#else
        sockClient = ::accept(sock, (sockaddr*)&addrClient, (socklen_t *)&nAddrClientLen);
#endif
        if(INVALID_SOCKET == sockClient)
        {
            LOG << "accept() failed.\n" << errno;
        }
        else
        {
            NewUserJoin userJoin;
            sendDataToAll(&userJoin);
            g_clients.push_back(new ClientSocket(sockClient));
            LOG << "client accepted: socket-" << hex<< (int)sockClient
                << "\tip-" << dec << inet_ntoa(addrClient.sin_addr)
                << ":" << addrClient.sin_port<<"\t"<< g_clients.size()<<endl;

        }
        return sockClient;
    }

    void close()
    {
        if(INVALID_SOCKET != sock)
        {
            for(auto s: g_clients)
            {
                closeSocket(s->socket());
                delete s;
            }
            g_clients.clear();

            closeSocket(sock);
            sock = INVALID_SOCKET;
#ifdef _WIN32
            WSACleanup();
#endif
        }
    }

    int nCount = 0;
    bool onRun()
    {
        if(isRun())
        {
            fd_set fdRead;
            fd_set fdWrite;
            fd_set fdExp;
            FD_ZERO(&fdRead);
            FD_ZERO(&fdWrite);
            FD_ZERO(&fdExp);
            FD_SET(sock, &fdRead);
            FD_SET(sock, &fdWrite);
            FD_SET(sock, &fdExp);

            SOCKET maxSock = sock;
            for(int i=g_clients.size()-1; i>=0; i--)
            {
                FD_SET(g_clients[i]->socket(), &fdRead);
                if(maxSock<g_clients[i]->socket())
                    maxSock = g_clients[i]->socket();
            }
//            LOG << "slect() start..";
//            timeval t = { 1,0 };
            int ret = select(maxSock+1, &fdRead, 0,0, 0);// &fdWrite, &fdExp, 0);  //&t);
//            LOG << "slect() end....";
            if(ret < 0)
            {
                LOG << "select() failed.";
                close();
                return false;
            }
            if( FD_ISSET(sock, &fdRead))
            {
                FD_CLR(sock, &fdRead);
                accept();
//                return true;
            }
            for (int n = (int)g_clients.size()-1; n >= 0; n--)
            {
                if(FD_ISSET(g_clients[n]->socket(), &fdRead))
                {
                    if(-1 == recvData(g_clients[n]))
                    {
                        closeSocket(g_clients[n]->socket());
                        auto iter = g_clients.begin()+n;
                        //                    if(iter!=g_clients.end())
                        {
                            g_clients.erase(iter);
                        }
                    }
                }
            }
        }
        return  true;
    }

    bool isRun()
    {
        return sock != INVALID_SOCKET;
    }

    char szRecv[RECV_BUFF_SZIE];
    int recvData(ClientSocket* s)
    {
        int nLen = recv(s->socket(), szRecv, RECV_BUFF_SZIE, 0);
        if(nLen <= 0)
        {
            LOG << "recv<=0\n";
            return -1;
        }

        memcpy(s->msgBuf()+s->getLastPos(), szRecv, nLen);
        s->setLastPos(s->getLastPos()+nLen);
        int cur = 0;
        while(s->getLastPos()-cur >= sizeof(DataHeader))
        {
            DataHeader *pHeader = (DataHeader*)(s->msgBuf()+cur);
            if(s->getLastPos()-cur >= pHeader->len)
            {
                int nSize = s->getLastPos() - pHeader->len;
                onMsg(s->socket(), pHeader);
                memcpy(s->msgBuf(), s->msgBuf()+pHeader->len, nSize);
                s->setLastPos( nSize);
//                cur += pHeader->len;
                if(pHeader->len!=72)
                {
                    LOG << "error len\n";
                }
//                onMsg(s->socket(), pHeader);
            }
            else
            {                
                break;
            }
        }
        if(cur)
        {
            memcpy(s->msgBuf(), s->msgBuf()+cur, s->getLastPos()-cur);
            s->setLastPos( s->getLastPos()-cur);
        }
        return 0;
    }

    virtual void onMsg(SOCKET s, DataHeader *pHeader)
    {
        ++recvCount;
        auto t = time.elaspedS();
        if(t>=1.0)
        {
            LOG << "time: "<<t << "\tclients: "<< g_clients.size() <<"\tpackage: "<< recvCount<< endl;
            time.start();
            recvCount = 0;
        }
        switch (pHeader->cmd)
        {
        case CMD_LOGIN:
        {
            Login *login = (Login*)pHeader;
//            LOG << "recv: login result-" << login->len << endl;
//            LoginResult ret;
//            sendData(s, &ret);
        }
            break;
        case CMD_LOGOUT:
        {
            Logout *logout = (Logout*)pHeader;
            LOG << "recv: logout result-" << logout->len << endl;
            LogoutResult ret;
            sendData(s, &ret);
        }
            break;
        default:
            LOG << "recv: unknown request-" << pHeader->cmd << endl;

            break;
        }
    }

    int sendData(SOCKET s, DataHeader* header)
    {
        if (isRun() && header)
        {
            return send(s, (const char*)header, header->len, 0);
        }
        return SOCKET_ERROR;
    }

    void sendDataToAll(DataHeader* header)
    {
        for (int n = (int)g_clients.size() - 1; n >= 0; n--)
        {
            sendData(g_clients[n]->socket(), header);
        }
    }

    void closeSocket(SOCKET s)
    {
#ifdef _WIN32
        closesocket(s);
#else
        close(s);
#endif
    }
};









#endif
