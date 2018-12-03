#ifndef MY_UTIL_H
#define MY_UTIL_H


#include <WinSock2.h>
#include <QDebug>
#include <string>
#include <iostream>
#include <iomanip>

//#pragma comment(lib, "ws2_32.lib")
#pragma execution_character_set("utf-8")

#define LOG qDebug()


using std::string;
using std::cin;


enum CMD
{
    CMD_LOGIN,
    CMD_LOGIN_RESULT,
    CMD_LOGOUT,
    CMD_LOGOUT_RESULT,
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



const char *SERVER_IP = "127.0.0.1";//"192.168.1.198";
const int SERVER_PORT = 8899;

#endif
