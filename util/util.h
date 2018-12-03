#ifndef MY_UTIL_H
#define MY_UTIL_H

#ifdef _WIN32
        #define WIN32_LEAN_AND_MEAN
//	#include<windows.h>
        #include<WinSock2.h>
        #pragma comment(lib,"ws2_32.lib")
#else
        #include<unistd.h> //uni std
        #include<arpa/inet.h>

        #define SOCKET int
        #define INVALID_SOCKET  (SOCKET)(~0)
        #define SOCKET_ERROR            (-1)
#endif


#include <QDebug>
#include <string>
#include <iostream>
#include <iomanip>
#include <thread>

//#pragma comment(lib, "ws2_32.lib")
#pragma execution_character_set("utf-8")

#define LOG qDebug()

using std::endl;
using std::string;
using std::cin;
using std::hex;
using std::dec;


enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
    CMD_NEW_USER_JOIN,
    CMD_ERROR
};
struct DataHeader
{
    int len;
    int cmd;
};
struct Login: public DataHeader
{
    Login()
    {
        len = sizeof(Login);
        cmd = CMD_LOGIN;
    }
    char userName[32];
    char password[32];
};
struct LoginResult: public DataHeader
{
    LoginResult(int ret=0)
    {
        len = sizeof(LoginResult);
        cmd = CMD_LOGIN_RESULT;
        result = ret;
    }
    int result;
};
struct Logout: public DataHeader
{
    Logout()
    {
            len = sizeof(Logout);
            cmd = CMD_LOGOUT;
    }
    char useName[32];
};
struct LogoutResult: public DataHeader
{
    LogoutResult(int ret=0)
    {
            len = sizeof(LogoutResult);
            cmd = CMD_LOGIN_RESULT;
            result = ret;
    }
    int result;
};

struct NewUserJoin: public DataHeader
{
    NewUserJoin(int s=0)
    {
        len = sizeof(NewUserJoin);
        cmd = CMD_NEW_USER_JOIN;
        scok = s;
    }
    int scok;
};

const char *SERVER_IP = "127.0.0.1";
//const char *SERVER_IP = "45.62.112.210";//"127.0.0.1";//"192.168.1.198";
const int SERVER_PORT = 8899;

#endif
