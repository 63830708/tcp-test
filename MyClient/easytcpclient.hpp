#ifndef _EasyTcpClient_hpp_
#define _EasyTcpClient_hpp_

#include "util.h"

#include "MessageHeader.hpp"

#define BUF_SIZE 4096

class EasyTcpClient
{
    SOCKET sock;
public:
    EasyTcpClient()
    {
        sock = INVALID_SOCKET;
    }
    virtual ~EasyTcpClient()
    {
        close();
    }

    void InitSocket()
    {
#ifdef _WIN32
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2,2), &wsaData);
#endif
        if( INVALID_SOCKET != sock)
        {
            LOG << "sock already open\n";
            close();
        }

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if( INVALID_SOCKET == sock)
        {
            LOG << "socket() failed.\n";
        }
        else
        {
            LOG << "socket() succeed.\n";
        }
    }

    int connect(const char* ip, unsigned short port)
    {
        if (INVALID_SOCKET == sock)
        {
            InitSocket();
        }
        sockaddr_in  addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
#ifdef _WIN32
        addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
        addr.sin_addr.s_addr = inet_addr(ip);
#endif

        int ret = ::connect(sock, (sockaddr*)&addr, sizeof(sockaddr_in));
        if(SOCKET_ERROR == ret)
        {
            LOG << "connect() failed.\n";
        }
        else
        {
            LOG << "connect() succeed.\n";
        }

        return ret;
    }

    void close()
    {
        if(INVALID_SOCKET != sock)
        {
#ifdef _WIN32
            closesocket(sock);
            WSACleanup();
#else
            close(sock);
#endif
            sock = INVALID_SOCKET;
        }
    }

    bool isRun()
    {
        return sock != INVALID_SOCKET;
    }

    int nCount = 0;
    bool onRun()
    {
        if( isRun())
        {
            fd_set fdReads;
            FD_ZERO(&fdReads);
            FD_SET(sock, &fdReads);
//            timeval t{0, 0};
            int ret = select(sock, &fdReads, 0, 0, 0);//&t);
            LOG << "select() ret=" <<ret << "\tcount="<< nCount++ << endl;
            if(ret < 0)
            {
                LOG << "select() failed.\n";
                close();
                return false;
            }

            if(FD_ISSET(sock, &fdReads))
            {
                FD_CLR(sock, &fdReads);
                if( -1 == recvData(sock))
                {
                    close();
                    return false;
                }
            }
        }
        return true;
    }

#define RECV_BUFF_SIZE  10240
    char szRecv[RECV_BUFF_SIZE];
    char szMsgBuf[RECV_BUFF_SIZE*10];
    int lastPos = 0;
    int recvData(SOCKET s)
    {
        int nLen = recv(s, szRecv, RECV_BUFF_SIZE, 0);
        LOG << "recv len=" << nLen << endl;
        if(nLen <= 0)
        {
            LOG << "recv()<=0 \n";
            return -1;
        }

        memcpy(szMsgBuf+lastPos, szRecv, nLen);
        lastPos += nLen;
        while(lastPos >= sizeof(DataHeader))
        {
            DataHeader *pHeader = (DataHeader*)szMsgBuf;
            if(lastPos >= pHeader->len)
            {
                int nSize = lastPos - pHeader->len;
                onMsg(pHeader);
                memcpy(szMsgBuf, szMsgBuf+pHeader->len, nSize);
                lastPos = nSize;
            }
            else
            {
                break;
            }
        }

        return 0;
    }

    void onMsg(DataHeader *header)
    {
        switch (header->cmd)
        {
        case CMD_LOGIN_RESULT:
        {
            LoginResult *loginRet = (LoginResult*)header;
            LOG << "recv: login result-" << loginRet->len << endl;
        }
            break;
        case CMD_LOGOUT_RESULT:
        {
            LogoutResult *logoutRet = (LogoutResult*)header;
            LOG << "recv: logout result-" << logoutRet->len << endl;
        }
            break;
        case CMD_NEW_USER_JOIN:
        {
            NewUserJoin *userJoin = (NewUserJoin*)header;
            LOG << "recv: newUser    username-" << userJoin->len << endl;
        }
            break;
        default:
            LOG << "recv: unknown result-" << header->cmd << endl;
            break;
        }
    }

    int sendData(DataHeader *header)
    {
        if(isRun() && header)
        {
            return send(sock, (const char*)header, header->len, 0);
        }
        return SOCKET_ERROR;
    }


};

#endif
