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
    CMD_LOGOUT,
    CMD_ERROR
};
struct DataHeader
{
    int len;
    int cmd;
};
struct Login
{
    char userName[32];
    char password[32];
};
struct LoginResult
{
    int result;
};
struct Logout
{
    char useName[32];
};
struct LogoutResult
{
    int result;
};



const char *SERVER_IP = "127.0.0.1";//"192.168.1.198";
const int SERVER_PORT = 8899;

#endif
