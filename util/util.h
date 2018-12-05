#ifndef MY_UTIL_H
#define MY_UTIL_H

#ifdef _WIN32
        #define FD_SETSIZE      1024
        #define WIN32_LEAN_AND_MEAN
        #include<windows.h>
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



#define LOG std::cout
//#define LOG qDebug()

using std::endl;
using std::string;
using std::cin;
using std::hex;
using std::dec;


//const char *SERVER_IP = "127.0.0.1";
const char *SERVER_IP = "192.168.31.55";
//const char *SERVER_IP = "45.62.112.210";//"127.0.0.1";//"192.168.1.198";
const int SERVER_PORT = 8899;

#endif
